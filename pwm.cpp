#include <pwm.h>

// pwm channels:
// 1. throttle
// 2. steering
// 3. other, not implemented yet

// description registers/functionality etc
// TCNT timer counter
// OC output compare bit (ie. signal visible on output port)
// WGM waveform generation mode (ie. set to fast pwm and defines how ICR and OCR shape the signal)
// COM Compare output mode (ie. how the OC bit behaves)
// ICR input compare register (ie. used to define signal frequency)
// OCR output compare register (ie. used to define duty cycle)
// TIMSK timer interrupt mask (ie. flags that enable/disable the interrupts)
// OCF output compare flag (ie. the flag is set when the TCNT matches the OCR)
// TOV timer overflow flag (ie. the flag that triggers the interrupt, reset automatically during ISR call)

// use TIMER4 on PORT H, which has 3 channels
// Port H = Mega Pin 6 and 7 = PH3 and PH4
// DDRH -  Port H Data Direction Register - read/write
// DDRH set bit 3 and 4 -> output = value of OCA and OCB become visible on pin6 and pin7

// the receiver signal has a constant frequency of 18500us
// the duty signal changes from between 1000us and 2000us with neutral at 1500us (adjustable on the RC)
// the signal phase doesn't change, the duty cycle is always at the beginning of the signal

// -> choose Timer operating mode "fast pwm"
// - single slope where TCNT starts from BOTTOM to TOP and then restarts at BOTTOM
// - WGM wave generation pattern to "fast pwm" with top control by ICR -> set Timer mode = 14
// - COM - Compare output mode set to "clear on match, set on bottom" -> mode "10" table 17.4
// this means the pwm signal (visible on the output port) is 1 when counter starts at 0
// then stays at 1 until the counter hits the OCR value (duty), then changes to 0
// then stays at 0 until the counter hits the ICR value (frequency), then is set back to 1
// the interrupt occurs when the counter overflows (TCNT == ICR )
// during the interrupt the next duty cycle can be set in the 2 OCR registers

// Timer4 has 16bit resolution, ie TCNT counts to 65536
// the processor frequency of 16MHZ, with a pre-scale of 8 this results in 16 000 000/8 = 2 000 000 ticks per sec
// the signal frequency is 1s / 18500us ~ 54hz, these 54hz have to fit into the pre-scaled count
// -> 2 000 000 / 54 ~ 37000 is the ICR value to achieve the desired 54HZ frequency ( and fits in 16bit)
// however the processor does not exactly produce the calculated results:
// see this article http://sobisource.com/arduino-mega-pwm-pin-and-frequency-timer-control/
// for example on port 4 under standard pre-scaler the frequency is 490hz
// hence calibration is necessary

// similarly if the duty of the signal is 2000us then the counter needs to count to 37000 / 18500 * 2000 ~ 4000

// how to scale the observed signal and duty durations from us to arduino ticks on timer 4 ?
// processor frequency F=16MHZ, prescaled by 8 -> 2MHZ
// this is a duration of 1/2MHZ = 0.5 us
// to get to a RC signal duration = 18500 us, this is a 18500us / 0.5us = 37000 counter ticks   (for the chosen prescale value to fit the 16 bit counter)
// ICR = 37000
// to get to a RC duty = 2000 us, this is 2000us / 0.5us = 4000
// OCRA = 2 * signal duration

// furthermore the pwm output is slightly off the expected value, according to this article
// http://sobisource.com/arduino-mega-pwm-pin-and-frequency-timer-control/
// for example on port 4 under standard prescaler the frequency is 490hz
// -> determine the real frequency with osci and apply a scaling factor to achieve the correct output
// (970.8737864/1000.0*(float)(1<<16)); //16777216 =

// to test how big the issue is the calibration routines produced the following results
// measurements made with the calibration port to measure pwm signal based on varying signal duration to confirm the multiplicaton by 2

		/*
		 * result pwm:
calibration   target , average dur, min, max : 17000,8500,-72,68
calibration   target , average dur, min, max : 17500,8750,-66,66
calibration   target , average dur, min, max : 18000,9000,-68,76
calibration   target , average dur, min, max : 18500,9250,-66,66
calibration   target , average dur, min, max : 19000,9500,-68,68
calibration   target , average dur, min, max : 19500,9750,-66,66
calibration   target , average dur, min, max : 20000,10000,-68,68
		 * result channel 1:
calibration   target , average dur, min, max : 17000,18574,-86,62
calibration   target , average dur, min, max : 17500,18572,-56,60
calibration   target , average dur, min, max : 18000,18574,-58,58
calibration   target , average dur, min, max : 18500,18573,-61,63
calibration   target , average dur, min, max : 19000,18573,-57,63
calibration   target , average dur, min, max : 19500,18575,-47,61
calibration   target , average dur, min, max : 20000,18578,-62,58


		 */

// measurements made with the calibration port to measure the RC signal shape

/*
		 * result channel 1 with steering:

measurements at full range
calibration:	signal duration   :   count , avg. duration, min, max : 501,18242,18056,19140
calibration:	duty duration   :   count , avg. duration, min, max : 500,1509,1124,1796

		 * result channel 2 with throttle:
2 measurements idle
calibration:	signal duration   :   avg. duration, min, max : 18211,18056,18880
calibration:	signal duration   :   avg. duration, min, max : 1498,1496,1504
calibration:	signal duration   :   avg. duration, min, max : 18206,18056,18788
calibration:	signal duration   :   avg. duration, min, max : 1498,1492,1500

measurements at full range
calibration:	signal duration   :   avg. duration, min, max : 18210,18056,19184
calibration:	signal duration   :   avg. duration, min, max : 1389,1000,1964
calibration:	signal duration   :   avg. duration, min, max : 18208,18056,19100
calibration:	signal duration   :   avg. duration, min, max : 1432,1000,1960

		 */


// Interrupts will occur when TCNT matches ICR
// ISR routine used: TIMER4_OVF              #COMPA and TIMER4COMPB
// to enable the overflow interrupt enable TOIE4 in TIMSK4

// details: control register TCCR4A and TCCR4B
// set wave generation pattern to "fast pwm" with top control by ICR -> mode 14
// WGM43=1 and WGM42=1 (located in TCCR4B)
// WGM41=1 and WGM40=0 (located in TCCR4A)

// set compare output mode channel to "clear on match, set on bottom" -> mode "10"
// COM4A1=1 and COM4A0=0 (located in TCCR4A)
// COM4B1=1 and COM4B0=0 (located in TCCR4A)

// Timer 4 pre-scaler set to 1/8
// CS42=0 and CS41=1 and CS40=0  (located in TCCR4B)

// enable interrupts for timer4A and timer4B in TIMSK4
// TOIE4 = 1
// OCIE4A = 1 AND OCIE4B = 1   (to disable OCIE4 = 0 )
// set TIFR4 to generally enable the interrupt, else OCF4A=0

void pwm_interrupt_off(){
	// clear the interrupt flag
	TIMSK4 &= ~( (1<<TOIE4) );
	// set interrupt bits to clear the flag
	TIFR4 |= (1<<TOV4);  // | (1<<OCF4B) | (1<<OCF4C);
}

void pwm_interrupt_on(){
	// clear the interrupt flag
	TIMSK4 |= (1<<TOIE4);
	// set interrupt bits to clear the flag
	TIFR4 |= (1<<TOV4);  // | (1<<OCF4B) | (1<<OCF4C);
}

void pwm_set_steering_duty(unsigned int value){
	PWM_steering_duty = value;
	//Serial.print("set   steering = ");
	//Serial.println(value);
}

void pwm_set_throttle_duty(unsigned int value){
	PWM_throttle_duty = value;
	//Serial.print("   throttle = ");
	//Serial.println(value);
}

unsigned long pwm_get_interrupt_timestamp(){ return PWM_int_time_stamp; }
unsigned long pwm_get_signal_duration(){ return PWM_target_signal_duration; }
unsigned int pwm_get_throttle_duty(){ return OCR4A; }
unsigned int pwm_get_steering_duty(){ return OCR4B; }
inline unsigned int scale_duty(unsigned int duty){ return (duty << 1); }; //(unsigned int) ( ((float) duty) * PWM_scaling_factor ); }

// sets the duration, for example during calibration, the PWM_target_signal_duration is const though
void pwm_set_signal_duration(unsigned int target_duration){
	cli();
	ICR4 = target_duration;
	sei();
}


/*
void pwm_update(unsigned int duration){

	if (PWM_actual_signal_duration!=0) {	// check if at least one cycle cycle is complete, this is the case after the duration has been determined at least once

		// use the actual pwm duration to adjust the scaling factor
		int delta = ( PWM_actual_signal_duration - PWM_target_signal_duration);
		int adjusted = PWM_target_signal_duration + delta;
		float PWM_scaling_factor_prep = PWM_scaling_factor * (float) ( PWM_target_signal_duration - delta ) / (float) PWM_target_signal_duration;

		// if necessary update the scaling factor and the signal duration
		if ( PWM_scaling_factor != PWM_scaling_factor_prep ){
			cli();
			PWM_scaling_factor = PWM_scaling_factor_prep;
			ICR4 = scale_duty(PWM_target_signal_duration);
			sei();
		}

		Serial.print("t = ");
		Serial.print(PWM_int_time_stamp);
		Serial.print("   dur = ");
		Serial.print(PWM_actual_signal_duration);
		Serial.print("   delta = ");
		Serial.print(delta);
		Serial.print("   adj = ");
		Serial.print(adjusted);
		Serial.print("   f = ");
		Serial.print(PWM_scaling_factor_prep,5);
		Serial.print("   ICR = ");
		Serial.print(ICR4);
		Serial.print("   dur = ");
		Serial.print(duration);
		Serial.print("   throttle = ");
		Serial.print(PWM_throttle_duty);
		Serial.print("   steer = ");
		Serial.println(PWM_steering_duty);

	}
}
*/
// updates time stamp and determines actual signal duration
// if ready updates the calibration factor and finally updates timer registers
ISR(TIMER4_OVF_vect){

	// determine the actual signal duration
	unsigned long t = micros();
	if (PWM_int_time_stamp!=0) { PWM_actual_signal_duration = t - PWM_int_time_stamp; }
	PWM_int_time_stamp = t;

	// update the duty cycles
	OCR4A = scale_duty(PWM_throttle_duty);
	OCR4B = scale_duty(PWM_steering_duty);
}

void pwm_Timer4_config(){
	pwm_interrupt_off();
	DDRH |= ( (1<<PH3) | (1<<PH4) );
	TCCR4A |= (1<<COM4A1) | (1<<COM4B1) | (1<<WGM41);
	TCCR4A &= ~((1<<COM4A0) | (1<<COM4B0) | (1<<WGM40));
	TCCR4B |= (1<<WGM43) | (1<<WGM42) | (1<<CS41) ;
	TCCR4B &= ~((1<<CS42) | (1<<CS40) );
	ICR4 = scale_duty(PWM_target_signal_duration);
	OCR4A = scale_duty(PWM_throttle_duty);
	OCR4B = scale_duty(PWM_steering_duty);
	//OCR4C=20000; //20.000 ticks = 10 millisecs -> 1sec = count to 100
	pwm_interrupt_on();
}




