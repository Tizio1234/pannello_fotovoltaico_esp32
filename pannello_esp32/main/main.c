#include <esp_log.h>
#include <unistd.h>
#include <ldr_adc.h>

static const char* TAG = "PANNELLO ESP32";

void app_main(void)
{
    adc_init();

    while (1)
    {
        adc_reading_t x_reading;
        adc_reading_t y_reading;

        x_adc_read(&x_reading);
        y_adc_read(&y_reading);

        ESP_LOGI(TAG, "Got ADC readings: x raw(%d), x calibrated(%dmV), y raw(%d), y calibrated(%dmV)",
                x_reading.raw,
                x_reading.voltage,
                y_reading.raw,
                y_reading.voltage);

        sleep(2);
    }

    adc_deinit();
}
