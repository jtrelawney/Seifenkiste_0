//#include "config.h"
#include <pwm.h>
#include <portkextint.h>

void setup() {
	pinMode(13, OUTPUT);
	Serial.begin(115200);

	// execute port configuration with disabled global interrupts:
	cli();
	port_k_config();
	pwm_Timer4_config();
	sei();
}


void loop() {

	/*
  digitalWrite(13, HIGH);

  // read and print current values
  // the portk ISR writes the pwm duty values in the background

  Serial.print("t = ");
  Serial.print(get_interrupt_timestamp());
  Serial.print("   steering = ");
  Serial.print(get_duty_steering());
  Serial.print("   throttle = ");
  Serial.println(get_duty_throttle());
*/
  digitalWrite(13, LOW);
 }
