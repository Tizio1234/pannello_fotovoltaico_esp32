#ifndef _SERVO_H_
#define _SERVO_H_

#include <iot_servo.h>

//#define Y_SERVO

void servo_init(void);
void servo_deinit(void);

void x_servo_set_angle(float angle);
#ifdef Y_SERVO
void y_servo_set_angle(float angle);
#endif

#endif