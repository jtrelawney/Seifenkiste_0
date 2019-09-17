#ifndef PWM_H_
#define PWM_H_

#include "Arduino.h"

const static uint16_t PWM_signal_duration=37000;
static volatile uint16_t PWM_throttle_duty=1500;
static volatile uint16_t PWM_steering_duty=1500;
static volatile unsigned long PWM_int_time=500;

void pwm_interrupt_off();
void pwm_interrupt_on();
void pwm_Timer4_config();
void set_steering_duty(uint16_t value);
void set_throttle_duty(uint16_t value);
unsigned int get_interrupt_timestamp();

#endif /* PWM_H_ */

