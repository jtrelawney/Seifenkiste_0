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
// the processor frequency of 16MHZ, with a pre-scale of 8 this results in 16000000/8 = 2000000 ticks per sec
// the signal frequency is 1s / 18500us ~ 54hz, these 54hz have to fit into the pre-scaled count
// -> 2000000 / 54 ~ 37000 is the ICR value to achieve the desired 54HZ frequency ( and fits in 16bit)
// however the processor does not exactly produce the calculated results:
// see this article http://sobisource.com/arduino-mega-pwm-pin-and-frequency-timer-control/
// for example on port 4 under standard pre-scaler the frequency is 490hz
// hence calibration is necessary

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

void set_steering_duty(uint16_t value){
	PWM_steering_duty = value;
	  //Serial.print("set   steering = ");
	  //Serial.println(value);
}

void set_throttle_duty(uint16_t value){
	PWM_throttle_duty = value;
	  //Serial.print("   throttle = ");
	  //Serial.println(value);
}

unsigned int get_interrupt_timestamp(){
	return PWM_int_time;
}

uint16_t get_duty_steering(){ return PWM_steering_duty; }
uint16_t get_duty_throttle(){ return PWM_throttle_duty; }


ISR(TIMER4_OVF_vect){
	unsigned long t = millis(); 	//micros()
	OCR4A = PWM_throttle_duty; // *PWMmotor_scalingfactor;
	OCR4B = PWM_steering_duty; // *PWMservo_scalingfactor;
	PWM_int_time = t;
}

void pwm_Timer4_config(){
	pwm_interrupt_off();
	DDRH |= ( (1<<PH3) | (1<<PH4) );
	TCCR4A |= (1<<COM4A1) | (1<<COM4B1) | (1<<WGM41);
	TCCR4A &= ~((1<<COM4A0) | (1<<COM4B0) | (1<<WGM40));
	TCCR4B |= (1<<WGM43) | (1<<WGM42) | (1<<CS41) ;
	TCCR4B &= ~((1<<CS42) | (1<<CS40) );
	ICR4 = PWM_signal_duration; //*PWMscalingfactorsignalwidth;
	OCR4A = PWM_throttle_duty; // *PWMmotor_scalingfactor;
	OCR4B = PWM_steering_duty; // *PWMservo_scalingfactor;
	//OCR4C=20000; //20.000 ticks = 10 millisecs -> 1sec = count to 100
	pwm_interrupt_on();
}




