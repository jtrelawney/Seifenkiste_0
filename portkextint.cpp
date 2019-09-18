/*
 * portkextint.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: jconnor
 */

#include "portkextint.h"

// general description
// when in interrupt occurs on PortK the ISR is called and
// determines the pin that caused the interrupt (by tracking the PortK bit pattern)
// for this pin a specific function is called


// in the case of the receiver the duty cycle is determined by measuring the time between flanks
// the duty cycle is then passed to the pwm module, hence the external definitions
// IMPORTANT: these routines should be as short possible and defined inline
extern void set_throttle_duty(uint16_t duty);
extern void set_steering_duty(uint16_t duty);

// stores the interrupt bit pattern for later comparison
volatile uint8_t PORTK_prev_bitpattern=0;

// stores interrupt timing
static volatile struct PORTK_status_data_type {
	uint16_t t_rising_flank=0;
	uint16_t t_falling_flank=0;
} PORTK_data[8];

static scaler_class throttle_scaler, steering_scaler;


void port_k_interrupt_off(){
	Serial.print("receiver interrupts off");
	PCICR &= ~(1<<PCIE2);
}

void port_k_interrupt_on(){
	Serial.print("receiver interrupts on");
	PCICR |= (1<<PCIE2);
}

void port_k_config(){

	Serial.println("\n\n-------------------------------------------------------------");
	port_k_interrupt_off();

	//Port K = Mega Pin 8 = PK0
	//Port K = Mega Pin 9 = PK1

	//DDRD - The Port D Data Direction Register - read/write
	//PORTD - The Port D Data Register - read/write
	//PIND - The Port D Input Pins Register - read only

	//clear bit0 and bit1 as input on portK -> Mega pin 8,9 as interrupt input
	Serial.print("old DDRK = "); Serial.println(DDRK,BIN);
	DDRK &= ~((1<<PK0) | (1<<PK1));
	Serial.print("new DDRK = "); Serial.println(DDRK,BIN);

	//activate internal pull up resistor
	Serial.print("old PORTK = "); Serial.println(PORTK,BIN);
	PORTK |= (1<<PK0) | (1<<PK1);
	Serial.print("new PORTK = "); Serial.println(PORTK,BIN);

	//enable interrupts PCINT16 and PCINT17 in pin change mask register for pcint23:16
	Serial.print("old PCMSK2 = "); Serial.println(PCMSK2,BIN);
	PCMSK2 |= (1<<PCINT16) | (1<<PCINT17);
	Serial.print("new PCMSK2 = "); Serial.println(PCMSK2,BIN);
	Serial.println("-------------------------------------------------------------");

	port_k_interrupt_on();

	// define asymmetric steering in and limited output pwm for both
	// values empirically determined
	steering_scaler.set_input_range(1140,1700);
	throttle_scaler.set_input_range(1000,1900);
	steering_scaler.set_output_range(1300, 1700);
	throttle_scaler.set_output_range(1300, 1700);
}

inline void port_k_throttle_ISR(uint8_t port_bitpattern){

	if (port_bitpattern & (1<<THROTTLE_BIT) )
	{
		//store rising flank time stamp
		PORTK_data[THROTTLE_BIT].t_rising_flank = micros();
	}
	else // interrupt was caused by falling flank!
	{
		// store falling flank time stamp
		PORTK_data[THROTTLE_BIT].t_falling_flank = micros();

		// calculate and set duty
		uint16_t throttle_duty = PORTK_data[THROTTLE_BIT].t_falling_flank - PORTK_data[THROTTLE_BIT].t_rising_flank;
		throttle_duty = throttle_scaler.calc_scaled_duty(throttle_duty);
		set_throttle_duty(throttle_duty);
	}
}

inline void port_k_steering_ISR(uint8_t port_bitpattern){

	if (port_bitpattern & (1<<STEERING_BIT) )
	{
		//store rising flank timestamp
		PORTK_data[STEERING_BIT].t_rising_flank = micros();
	}
	else // interrupt was caused by falling flank!
	{
		// store falling flank time stamp
		PORTK_data[STEERING_BIT].t_falling_flank = micros();

		// calculate and set duty
		uint16_t steering_duty = PORTK_data[STEERING_BIT].t_falling_flank - PORTK_data[STEERING_BIT].t_rising_flank;
		steering_duty = steering_scaler.calc_scaled_duty(steering_duty);
		set_steering_duty(steering_duty);
	}
}


// the interrupt routine called when one of the pins on portK observes a flank
// tracks the bit pattern on the port to determine the pin which triggered the interrupt
// then calls the specific routine
ISR(PCINT2_vect) {

	// read pin K
	uint8_t port_bitpattern=PINK;
	// XOR with previous reading to isolate the bits that changed
	uint8_t trigger_bitpattern=(PORTK_prev_bitpattern^port_bitpattern);
	// store the current pattern for next call
	PORTK_prev_bitpattern=port_bitpattern;

	// was interrupt caused by bit0 (analog port 8), which is the throttle signal
	if (trigger_bitpattern & (1<<THROTTLE_BIT) )	port_k_throttle_ISR(port_bitpattern);

	//was interrupt cause by bit1 (analog port 9), which is the steering signal
	if (trigger_bitpattern & (1<<STEERING_BIT) ) port_k_steering_ISR(port_bitpattern);

	// was interrupt caused by bit ...  (analog port ... ), which is the ... signal
}



