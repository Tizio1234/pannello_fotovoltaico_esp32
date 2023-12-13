#include <esp_log.h>
#include <unistd.h>
#include <ldr_adc.h>
#include <servo.h>

static const char* TAG = "PANNELLO ESP32";

void app_main(void)
{
    //adc_init();
    servo_init();

    ESP_LOGI(TAG, "ESP32 esp-iot-solution servo example");

    float angle = 0.0f;
    float step = 10.0f;

    while (1)
    {
        /*adc_reading_t x_reading;
        adc_reading_t y_reading;

        x_adc_read(&x_reading);
        y_adc_read(&y_reading);

        ESP_LOGI(TAG, "Got ADC readings: x raw(%d), x calibrated(%dmV), y raw(%d), y calibrated(%dmV)",
                x_reading.raw,
                x_reading.voltage,
                y_reading.raw,
                y_reading.voltage);

        sleep(2);*/
        ESP_LOGI(TAG, "Servo angle set to %f degrees", angle);
        x_servo_set_angle(angle);

        angle += step;
        if (angle < 0.0f || angle > 120.0f)
        {
            step = -step;
            angle += step * 2;
        }
        sleep(1);
    }

    servo_deinit();
    //adc_deinit();
}
