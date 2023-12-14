#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <unistd.h>

#include <ldr_adc.h>
#include <servo.h>

#include <math.h>

#define MIN(x, y) (x > y ? y : x)
#define MAX(x, y) (x > y ? x : y)

static const char* TAG = "PANNELLO ESP32";

static const float COEFF = 4.884004884e-4f;
static const float OFFSET = -1.0f;

static const float P_GAIN = 10.0f;

float raw_to_cte(int raw);

float raw_to_cte(int raw){
    return raw * COEFF + OFFSET;
}

void app_main(void)
{
    adc_init();
    servo_init();

    ESP_LOGI(TAG, "ESP32 esp-iot-solution servo example");

    float angle = 60.0f;

    x_servo_set_angle(angle);

    vTaskDelay(100);

    while (1)
    {
        adc_reading_t x_reading;
        x_adc_read(&x_reading);

        ESP_LOGI(TAG, "Read x adc values: raw(%d), converted(%dmV)", x_reading.raw, x_reading.voltage);

        float cte = raw_to_cte(x_reading.raw);
        
        angle = MIN(MAX((angle + P_GAIN * cte), 0.0f), 120.0f);

        ESP_LOGI(TAG, "Servo angle set to %f degrees", angle);
        x_servo_set_angle(angle);
        
        vTaskDelay(100);
    }

    servo_deinit();
    adc_deinit();
}
