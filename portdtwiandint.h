/*
 * portdtwiandint.h
 *
 *  Created on: Oct 9, 2019
 *      Author: jconnor
 */

#ifndef PORTDTWIANDINT_H_
#define PORTDTWIANDINT_H_

#include "Arduino.h"
#include <common.h>

// to store signal timestamps and the duty
volatile static unsigned long rising_flank_time_stamp = 0, falling_flank_time_stamp = 0;
volatile static unsigned int emergency_button_duty = 0;

// standard duty is 1000, with button pressed 2000, // unforunately this doesn't work : and if duty is 1500, then the RC is turned off
static const unsigned int emergency_signal_threshhold = 1800;// , rc_turned_off_signal_threshhold = 1200;

// the ISR for the PortD external interrupt INT2 calls ISR(INT2_vect)
// this calls this specific routine which checks whether the emergency button was pressed
static void emergency_button_ISR();

// this is the handler routine when an emergency button was pressed
static void emergency_call(unsigned int emergency_button_duty);

// management functions
void port_d_interrupt_off();
void port_d_interrupt_on();
void port_d_config();
//void port_d_get_data(const port_k_connected_device_types port_no, unsigned long *signal_time_stamp, unsigned int *signal_duration, unsigned long *duty_time_stamp, unsigned int *duty_duration);

#endif /* PORTDTWIANDINT_H_ */
