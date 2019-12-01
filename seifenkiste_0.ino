#include <pwm.h>
#include <portkextint.h>
#include <common.h>
#include <controllercalibration.h>
#include <portdtwiandint.h>

extern state_def G_state;

// wait for button press and unpress and print marker every second
unsigned long last_timestamp = millis(), current_timestamp;
void wait_for_button(){
	// wait for press
	while ( G_state.RC_emergency == false ){
		current_timestamp=millis();
		if (current_timestamp-last_timestamp > 1000) {
				Serial.print(".");
				last_timestamp = current_timestamp;
		}
	}
	// make sure unpress happened
	while ( G_state.RC_emergency == true ){
		current_timestamp=millis();
		if (current_timestamp-last_timestamp > 1000) {
				Serial.print(".");
				last_timestamp = current_timestamp;
		}
	}
}

// to set the throttle points for neutral, forward and backward on the ESC, a sequence of set key and signals is executed
// first the neutral point is set to 1500, then forward point is set to 2000, last the backward point is set to 1000
// to proceed to the next signal this routine is waiting for the emergency button to be pressed
// the function returns after the sequence is completed
void create_calibration_signals_for_ESC(){

	Serial.println("switch off the RC or it interferes with this procedure");
	port_k_interrupt_off();

	Serial.println("press emergency button to begin");
	wait_for_button();

	Serial.println("\nsetting to neutral signal, press SET key to program, then the emergency button");
	pwm_set_throttle_duty(1500);
	wait_for_button();

	Serial.println("\nnext");
	delay(1000);

	Serial.println("\nsetting forward signal, press SET key to program, then the emergency button");
	pwm_set_throttle_duty(2000);
	wait_for_button();

	Serial.println("\nnext");
	delay(1000);

	Serial.println("\nsetting backward signal, press SET key to program, then the emergency button");
	pwm_set_throttle_duty(1000);
	wait_for_button();

	Serial.println("\nnext");
	delay(1000);

	Serial.println("complete!");
	port_k_interrupt_on();

}


// helps to calibrate the RC by monitoring the signals and gathering and printing stats
// turn off debugging in main or the values will be overwritten
void calibrate_RC(int option, int count){
	if (option == 0) return;

	delay(1000);
	RC_calibration_class calib;

	// option 1: monitor and print the stats (eg. for RC signal)
	if (option == 1) {
		calib.monitor_calibration_port(count);
		calib.print_stats();
	}

	// option 2: monitor the signal stats after adjusting the pwm duration (eg. for RC signal)
	if (option == 2) {
		calib.monitor_calibration_port_for_pwm(count);
	}

	delay(1000);
}

bool PRINT = false;

void setup() {
	pinMode(13, OUTPUT);
	Serial.begin(115200);
	PRINT = false;

	// execute port configuration with disabled global interrupts:
	cli();
	port_k_config();
	port_d_config();
	pwm_Timer4_config();
	sei();

	// optional: monitoring
	calibrate_RC(0,100);
	//create_calibration_signals_for_ESC();
}

unsigned long throttle_signal_timestamp, throttle_duty_timestamp;
unsigned int throttle_duration, throttle_duty;
unsigned long steering_signal_timestamp, steering_duty_timestamp;
unsigned int steering_duration, steering_duty;
unsigned long calibration_signal_timestamp, calibration_duty_timestamp;
unsigned int calibration_duration, calibration_duty;

void loop() {

  digitalWrite(13, HIGH);

  port_k_get_data(port_k_connected_device_types::throttle, &throttle_signal_timestamp, &throttle_duration, &throttle_duty_timestamp, &throttle_duty);
  port_k_get_data(port_k_connected_device_types::steering, &steering_signal_timestamp, &steering_duration, &steering_duty_timestamp, &steering_duty);
  port_k_get_data(port_k_connected_device_types::RC_calibration, &calibration_signal_timestamp, &calibration_duration, &calibration_duty_timestamp, &calibration_duty);

  if (PRINT==true){
	  Serial.print("throttle   t= ");
	  Serial.print(throttle_signal_timestamp);
	  Serial.print("   dur = ");
	  Serial.print(throttle_duration);
	  Serial.print("   t= ");
	  Serial.print(throttle_duty_timestamp);
	  Serial.print("   duty = ");
	  Serial.print(throttle_duty);

	  Serial.print("      steering   t= ");
	  Serial.print(steering_signal_timestamp);
	  Serial.print("   dur = ");
	  Serial.print(steering_duration);
	  Serial.print("   t= ");
	  Serial.print(steering_duty_timestamp);
	  Serial.print("   duty = ");
	  Serial.println(steering_duty);

	  Serial.print("pwm duration, throttle duty, steering duty = ");
	  Serial.print(pwm_get_signal_duration());
	  Serial.print(",");
	  Serial.print(pwm_get_throttle_duty());
	  Serial.print(",");
	  Serial.println(pwm_get_steering_duty());

	  /*
	  Serial.print("calibration   t= ");
	  Serial.print(calibration_signal_timestamp);
	  Serial.print("   dur = ");
	  Serial.print(calibration_duration);
	  Serial.print("   t= ");
	  Serial.print(calibration_duty_timestamp);
	  Serial.print("   duty = ");
	  Serial.println(calibration_duty);
	*/
	  Serial.println();
  }

  digitalWrite(13, LOW);
 }
