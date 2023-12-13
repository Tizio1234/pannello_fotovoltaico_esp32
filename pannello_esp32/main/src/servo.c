#include <servo.h>
#include <iot_servo.h>

static const servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 500,
    .max_width_us = 2500,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
        .servo_pin = {
            27,
#ifdef Y_SERVO
            26,
#endif
        },
        .ch = {
            LEDC_CHANNEL_0,
#ifdef Y_SERVO
            LEDC_CHANNEL_1,
#endif
        },
    },
#ifdef Y_ADC
    .channel_number = 2,
#else
    .channel_number = 1,
#endif
};

void servo_init(void){
    ESP_ERROR_CHECK(iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg));
}

void servo_deinit(void){
    ESP_ERROR_CHECK(iot_servo_deinit(LEDC_LOW_SPEED_MODE));
}

void x_servo_set_angle(float angle){
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
}
#ifdef Y_SERVO
void y_servo_set_angle(float angle){
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 1, angle);
}
#endif
