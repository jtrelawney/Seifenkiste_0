/*
 * portdtwiandint.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: jconnor
 */

#include <portdtwiandint.h>

extern struct state_def G_state;
extern void G_emergency_routine();

// emergency routine which the ISR calls
inline static void emergency_call(unsigned int emergency_button_duty){
	//Serial.print("d=");
	//Serial.println(emergency_button_duty);
	if ( emergency_button_duty > emergency_signal_threshhold ) {
		G_state.RC_emergency = true;
		G_emergency_routine();
	} else G_state.RC_emergency = false;
}

// emergency ISR
inline static void emergency_button_ISR(){

	unsigned long time_stamp = micros();

	// if ISC20 bit is set in control register, then this interrupt was caused by a rising flank
	byte rising_edge_set = EICRA &= (1<<ISC20);

	if (rising_edge_set>0){
		// remember time stamp and toggle to falling edge
		rising_flank_time_stamp = time_stamp;
		EICRA &= ~(1<<ISC20);
		//Serial.print("r=");
		//Serial.println(rising_flank_time_stamp);
	} else {
		// need a rising flank before the falling flank is captured
		if (rising_flank_time_stamp > 0) falling_flank_time_stamp = time_stamp;

		// determine the signal_duty and toggle to rising edge
		emergency_button_duty = falling_flank_time_stamp - rising_flank_time_stamp;
		EICRA |= (1<<ISC20);
		//Serial.print("f=");
		//Serial.println(falling_flank_time_stamp);
		emergency_call(emergency_button_duty);
	}
}

// point INT2 ISR to the emergency_button_routine
ISR(INT2_vect) { emergency_button_ISR(); }

void port_d_interrupt_off(){
	Serial.println("disable INT2");
	EIMSK &= ~(1<<INT2);
}

void port_d_interrupt_on(){
	Serial.println("enable INT2");
	EIMSK |= (1<<INT2);
}

void port_d_config(){

	Serial.println("\n\n-------------------------------------------------------------");
	Serial.println("connect pin 19 (RXD1) to receiver button signal");

	port_d_interrupt_off();

	// general idea: - couldn't find a register which allows to check for the interrupt bit of INT2, hence can't check in ISR what flank it is
	// start cycle with rising edge, so at first entry of INT2 the interrupt cannot be caused by falling edge
	// then toggle the ISC20 bit, ie. the next interrupt will be caused by a falling edge and complete the duty cycle

	// set the control bit pattern for INT2 to "11", which means a rising edge triggers interrupt
	// the signal is constant ~ 1000 ticks, with the emergency button pressed it will be 2000
	Serial.println("set INT2 sense control to 'falling edge'");
	Serial.print("old EICRA = "); Serial.println(EICRA,BIN);

	// set to rising edge -> control pattern set to 11
	EICRA |= (1<<ISC21) | (1<<ISC20);	// set bit 4 and bit 5
	Serial.print("new EICRA = "); Serial.println(EICRA,BIN);

	// clear the INT2 interrupt flag, so that when enabling the interrupt it doesn't trigger right away
	Serial.println("clear INTF2 interrupt flag");
	Serial.print("old EIFR = "); Serial.println(EIFR,BIN);
	EIFR |= (1<<INTF2);
	Serial.print("new EIFR = "); Serial.println(EIFR,BIN);

	// enable global interrupts:
	port_d_interrupt_on();

	Serial.println("-------------------------------------------------------------");

}

