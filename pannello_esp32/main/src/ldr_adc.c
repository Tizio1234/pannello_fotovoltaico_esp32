#include <ldr_adc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include <esp_check.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define Y_ADC

#define X_ADC_UNIT          (adc_unit_t)ADC_UNIT_1
#define X_ADC_CHAN          (adc_channel_t)ADC_CHANNEL_7
#define X_ADC_BITWIDTH      (adc_bitwidth_t)ADC_BITWIDTH_DEFAULT
#define X_ADC_ATTEN         (adc_atten_t)ADC_ATTEN_DB_11

#ifdef Y_ADC
#define Y_ADC_UNIT          (adc_unit_t)ADC_UNIT_1
#define Y_ADC_CHAN          (adc_channel_t)ADC_CHANNEL_6
#define Y_ADC_BITWIDTH      (adc_bitwidth_t)ADC_BITWIDTH_DEFAULT
#define Y_ADC_ATTEN         (adc_atten_t)ADC_ATTEN_DB_11
#endif

const static char *TAG = "LDR ADC";

static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void example_adc_calibration_deinit(adc_cali_handle_t handle);

static adc_oneshot_unit_handle_t x_adc_handle;
static adc_cali_handle_t x_adc_cali_chan_handle = NULL;
static bool do_calibration_x;
    

#ifdef Y_ADC
static adc_oneshot_unit_handle_t y_adc_handle;
static adc_cali_handle_t y_adc_cali_chan_handle = NULL;
static bool do_calibration_y;
#endif

void adc_init(void){
    /* X ADC Init */
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = X_ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &x_adc_handle));

    /* X ADC Config */
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = X_ADC_BITWIDTH,
        .atten = X_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(x_adc_handle, X_ADC_CHAN, &config));

    /* X ADC Calibration Init */
    do_calibration_x = example_adc_calibration_init(X_ADC_UNIT, X_ADC_CHAN, X_ADC_ATTEN, &x_adc_cali_chan_handle);

#ifdef Y_ADC
    if (Y_ADC_UNIT != X_ADC_UNIT)
    {
        /* Y ADC Init */
        init_config.unit_id = Y_ADC_UNIT;
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &y_adc_handle));
    } else
    {
        y_adc_handle = x_adc_handle;
    }
    
    /* Y ADC Config */
    config.bitwidth = Y_ADC_BITWIDTH;
    config.atten = Y_ADC_ATTEN;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(y_adc_handle, Y_ADC_CHAN, &config));

    /* X ADC Calibration Init */
    do_calibration_y = example_adc_calibration_init(Y_ADC_UNIT, Y_ADC_CHAN, Y_ADC_ATTEN, &y_adc_cali_chan_handle);
#endif    
}

void adc_deinit(void){
    adc_oneshot_del_unit(x_adc_handle);
    if (do_calibration_x)
    {
        example_adc_calibration_deinit(x_adc_cali_chan_handle);
    }
#ifdef Y_ADC
    if (X_ADC_UNIT != Y_ADC_UNIT)
    {
        adc_oneshot_del_unit(y_adc_handle);
    }
    
    if (do_calibration_y)
    {
        example_adc_calibration_deinit(y_adc_cali_chan_handle);
    }
#endif
}

int x_adc_read(adc_reading_t* adc_reading){
    ESP_RETURN_ON_ERROR(adc_oneshot_read(x_adc_handle, X_ADC_CHAN, &(adc_reading->raw)), TAG, "Failed to read ADC");
    if (do_calibration_x) ESP_RETURN_ON_ERROR(adc_cali_raw_to_voltage(x_adc_cali_chan_handle, adc_reading->raw, &(adc_reading->voltage)), TAG, "Failed to convert ADC raw value to voltage");
    return ESP_OK;
}
#ifdef Y_ADC
int y_adc_read(adc_reading_t* adc_reading){
    ESP_RETURN_ON_ERROR(adc_oneshot_read(y_adc_handle, Y_ADC_CHAN, &(adc_reading->raw)), TAG, "Failed to read ADC");
    if (do_calibration_y) ESP_RETURN_ON_ERROR(adc_cali_raw_to_voltage(y_adc_cali_chan_handle, adc_reading->raw, &(adc_reading->voltage)), TAG, "Failed to convert ADC raw value to voltage");
    else adc_reading->voltage = 1000;
    return ESP_OK;
}
#endif
/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

