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
extern void pwm_set_throttle_duty(unsigned int duty);
extern void pwm_set_steering_duty(unsigned int duty);

// stores the interrupt bit pattern for later comparison
static volatile uint8_t PORTK_prev_bitpattern=0;

// creates a scaler object to be used to create linear duty values
//static scaler_class throttle_scaler, steering_scaler;

// in this data structure the ISR stores the interrupt data
// if the duty cycle is complete, the boolean variable is true and the duty duration is valid, the signal duration is from the previous cycle
// if a new signal starts, the boolean variable is false and the signal duration is valid, the duty duration is from the previous cycle
// the timestamp is updated when the signal is complete, hence the
static struct portK_data_tracking_type {
	volatile unsigned long t_rising_flank = 0;	// most recent
	volatile unsigned long t_falling_flank = 0;	// most recent
	volatile unsigned long signal_timestamp = 0;	// time stamp refers to the most recent completed signal
	volatile unsigned int signal_duration = 0;	// valid after rising flank, when signal is complete
	volatile unsigned long duty_timestamp = 0;	// time stamp refers to the most recent completed duty
	volatile unsigned int duty_duration = 0;		// valid after falling flank, signal not yet complete
	scaler_class scaler;				// scaler object
	void (*specific_ISR_function_ptr)(unsigned int) = NULL;		// function ptr to extern function
} PORTK_data[8];

void port_k_get_data(const port_k_connected_device_types port_no, unsigned long *signal_time_stamp, unsigned int *signal_duration, unsigned long *duty_time_stamp, unsigned int *duty_duration){
	*signal_time_stamp = PORTK_data[port_no].signal_timestamp;
	*signal_duration = PORTK_data[port_no].signal_duration;
	*duty_time_stamp = PORTK_data[port_no].duty_timestamp;
	*duty_duration = PORTK_data[port_no].duty_duration;
}

void port_k_interrupt_off(){
	Serial.println("receiver interrupts off");
	PCICR &= ~(1<<PCIE2);
}

void port_k_interrupt_on(){
	Serial.println("receiver interrupts on");
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
	DDRK &= ~((1<<PK0) | (1<<PK1) ); // | (1<<PK7));
	Serial.print("new DDRK = "); Serial.println(DDRK,BIN);

	//activate internal pull up resistor
	Serial.print("old PORTK = "); Serial.println(PORTK,BIN);
	PORTK |= (1<<PK0) | (1<<PK1);
	Serial.print("new PORTK = "); Serial.println(PORTK,BIN);

	//enable interrupts PCINT16 and PCINT17 in pin change mask register for pcint23:16
	Serial.print("old PCMSK2 = "); Serial.println(PCMSK2,BIN);
	PCMSK2 |= (1<<PCINT16) | (1<<PCINT17) | (1<<PCINT23);
	Serial.print("new PCMSK2 = "); Serial.println(PCMSK2,BIN);
	Serial.println("-------------------------------------------------------------");

	PORTK_data[port_k_connected_device_types::throttle].specific_ISR_function_ptr = &pwm_set_throttle_duty;
	PORTK_data[port_k_connected_device_types::steering].specific_ISR_function_ptr = &pwm_set_steering_duty;

	// define asymmetric throttle in (one RC channel has shorter range) and limited output for throttle (to slow car down)
	// values empirically determined
	PORTK_data[port_k_connected_device_types::throttle].scaler.set_input_range(1100,1900);
	PORTK_data[port_k_connected_device_types::throttle].scaler.set_output_range(1400, 1600);
	PORTK_data[port_k_connected_device_types::steering].scaler.set_input_range(1200,1800);
	PORTK_data[port_k_connected_device_types::steering].scaler.set_output_range(1050, 1950);

	port_k_interrupt_on();
}

/*
inline void port_k_throttle_ISR(uint8_t port_bitpattern){

	port_k_connected_device_types throttle = port_k_connected_device_types::throttle;
	unsigned long time_stamp = micros();
	if (port_bitpattern & (1<<throttle) )	// interrupt was caused by rising flank!
	{
		// rising flank : new duty signal starts, first calculate the duration, then store time stamp
		PORTK_data[throttle].signal_duration = time_stamp - PORTK_data[throttle].t_rising_flank;
		PORTK_data[throttle].signal_timestamp = time_stamp;
		PORTK_data[throttle].t_rising_flank = time_stamp;
	}
	else // interrupt was caused by falling flank!
	{
		// falling flank : duty signal complete, store time stamp, calculate the duty duration, then communicate the scaled duty
		PORTK_data[throttle].duty_timestamp = time_stamp;
		PORTK_data[throttle].t_falling_flank = time_stamp;
		unsigned int throttle_duty = PORTK_data[throttle].t_falling_flank - PORTK_data[throttle].t_rising_flank;
		PORTK_data[throttle].duty_duration = throttle_duty;
		throttle_duty = throttle_scaler.calc_scaled_duty(throttle_duty);
		PORTK_data[throttle].specific_ISR_function_ptr(throttle_duty);
	}
}


inline void port_k_steering_ISR(uint8_t port_bitpattern){

	port_k_connected_device_types steering = port_k_connected_device_types::steering;
	unsigned long time_stamp = micros();
	if (port_bitpattern & (1<<steering) ) // interrupt was caused by rising flank!
	{
		// rising flank : new duty signal starts, first calculate the duration, then store time stamp
		PORTK_data[steering].signal_duration = time_stamp - PORTK_data[steering].t_rising_flank;
		PORTK_data[steering].signal_timestamp = time_stamp;
		PORTK_data[steering].t_rising_flank = time_stamp;
	}
	else // interrupt was caused by falling flank!
	{
		// falling flank : duty signal complete, store time stamp, calculate the duty duration, then communicate the scaled duty
		PORTK_data[steering].duty_timestamp = time_stamp;
		PORTK_data[steering].t_falling_flank = time_stamp;
		unsigned int steering_duty = PORTK_data[steering].t_falling_flank - PORTK_data[steering].t_rising_flank;
		PORTK_data[steering].duty_duration = steering_duty;
		steering_duty = steering_scaler.calc_scaled_duty(steering_duty);
		PORTK_data[steering].specific_ISR_function_ptr(steering_duty);
	}
}
*/

inline void port_k_ISR(uint8_t port_bitpattern, port_k_connected_device_types device_port_no){

	unsigned long time_stamp = micros();
	if (port_bitpattern & (1<<device_port_no) ) // interrupt was caused by rising flank!
	{
		// rising flank : new duty signal starts, first calculate the duration, then store time stamp
		PORTK_data[device_port_no].signal_duration = time_stamp - PORTK_data[device_port_no].t_rising_flank;
		PORTK_data[device_port_no].signal_timestamp = time_stamp;
		PORTK_data[device_port_no].t_rising_flank = time_stamp;
	}
	else // interrupt was caused by falling flank!
	{
		// falling flank : duty signal complete, store time stamp, calculate the duty duration, then call the specific ISR (if defined)
		PORTK_data[device_port_no].duty_timestamp = time_stamp;
		PORTK_data[device_port_no].t_falling_flank = time_stamp;
		unsigned int duty = PORTK_data[device_port_no].t_falling_flank - PORTK_data[device_port_no].t_rising_flank;
		PORTK_data[device_port_no].duty_duration = duty;
		duty = PORTK_data[device_port_no].scaler.calc_scaled_duty(duty);
		if (PORTK_data[device_port_no].specific_ISR_function_ptr != NULL) PORTK_data[device_port_no].specific_ISR_function_ptr(duty);
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

	// find which ports have caused the interrupt and call their ISR
	for (unsigned int device_id=0; device_id<8; device_id++){
		bool active = ( trigger_bitpattern & (1<<device_id) );
		if (active == true) port_k_ISR(port_bitpattern, (port_k_connected_device_types) device_id);
	}
}



