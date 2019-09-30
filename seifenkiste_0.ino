#include <pwm.h>
#include <portkextint.h>
#include <common.h>

void setup() {
	pinMode(13, OUTPUT);
	Serial.begin(115200);

	// execute port configuration with disabled global interrupts:
	cli();
	port_k_config();
	pwm_Timer4_config();
	sei();

	// optional: monitoring
	int option = 0;

	// option 1: monitor and print the stats (eg. for RC signal)
	if (option == 1) {
		delay(1000);
		calibration_class calib;
		calib.monitor_calibration_port(100);
		calib.print_stats();
		delay(1000);
	}

	// option 2: monitor the signal stasts after adjusting the pwm duration (eg. for RC signal)
	calibration_class calib;
	if (option == 2) {
		delay(1000);
		calibration_class calib;
		calib.monitor_calibration_port_for_pwm(100);
		delay(1000);
	}
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

  bool PRINT = true;

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
