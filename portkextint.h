/*
 * portkextint.h
 *
 *  Created on: Sep 17, 2019
 *      Author: jconnor
 */

#ifndef PORTKEXTINT_H_
#define PORTKEXTINT_H_

#include "Arduino.h"

// for bit manipulation and indexing into the tracking array
#define THROTTLE_BIT 0
#define STEERING_BIT 1

class scaler_class {
private:
	uint16_t neutral_epsilon = 20;	// consider delta around neutral as neutral
	uint16_t min_signal_in = 1000;
	uint16_t neutral_signal_in = 1500;
	uint16_t max_signal_in = 2000;
	uint16_t min_signal_out = 1000;
	uint16_t neutral_signal_out = 1500;
	uint16_t max_signal_out = 2000;
public:
	scaler_class(){};
	void set_input_range(uint16_t min_in, uint16_t max_in) {
		min_signal_in = min_in;
		max_signal_in = max_in;
	};
	void set_output_range(uint16_t min_out, uint16_t max_out) {
		min_signal_out = min_out;
		max_signal_out = max_out;
	};

	uint16_t calc_scaled_duty(uint16_t duty){
		uint16_t result=neutral_signal_in;
		float factor = 0.0;
		bool valid_high_signal = (duty>neutral_signal_in+neutral_epsilon);
		if (valid_high_signal) {
			if (duty>max_signal_in) duty = max_signal_in;
			factor = (float)(duty - neutral_signal_in) / (float)(max_signal_in-neutral_signal_in);
			result = neutral_signal_out + (uint16_t) (factor * (float)( max_signal_out - neutral_signal_out) );
		}
		bool valid_low_signal = (duty<neutral_signal_in-neutral_epsilon);
		if (valid_low_signal) {
			if (duty<min_signal_in) duty = min_signal_in;
			factor = (float)(duty - neutral_signal_in) / (float)(min_signal_in-neutral_signal_in);
			result = neutral_signal_out + (uint16_t) (factor * (float)( min_signal_out - neutral_signal_out) );
		}
		  Serial.print("t = ");
		  Serial.print(millis());
		  Serial.print("   duty = ");
		  Serial.print(duty);
		  Serial.print("   factor = ");
		  Serial.print(factor);
		  Serial.print("   scaled = ");
		  Serial.println(result);
		return result;
	}
};

// the ISR for the PortK external interrupts calls these specific routines
// here port specific evaluations are executed and
// the related external processing routine is called
void port_k_throttle_ISR(uint8_t port_bitpattern);
void port_k_steering_ISR(uint8_t port_bitpattern);

// management functions
void port_k_interrupt_off();
void port_k_interrupt_on();
void port_k_config();

#endif /* PORTKEXTINT_H_ */