#ifndef PWM_H_
#define PWM_H_

#include "Arduino.h"

const static unsigned int PWM_target_signal_duration=18100;
static volatile unsigned int PWM_throttle_duty=1500;
static volatile unsigned int PWM_steering_duty=1500;

static volatile unsigned int PWM_actual_signal_duration=0;
static volatile unsigned long PWM_int_time_stamp=0;

//static volatile float PWM_scaling_factor = 2.0;


void pwm_interrupt_off();
void pwm_interrupt_on();
void pwm_Timer4_config();

void pwm_set_signal_duration(unsigned int duration);
void pwm_set_steering_duty(unsigned int value);
void pwm_set_throttle_duty(unsigned int value);

//void pwm_update(unsigned int duration);

unsigned long pwm_get_interrupt_timestamp();
unsigned long pwm_get_signal_duration();
unsigned int pwm_get_steering_duty();
unsigned int pwm_get_throttle_duty();

#endif /* PWM_H_ */

