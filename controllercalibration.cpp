/*
 * controllercalibration.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: jconnor
 */

#include <controllercalibration.h>

controller_calibration::controller_calibration() {
	// TODO Auto-generated constructor stub
}

controller_calibration::~controller_calibration() {
	// TODO Auto-generated destructor stub
}

// reads the calibration input port signal for a number of times and records the statistics
// helpful for reading min,max, avg values of the remote control channels
void RC_calibration_class::monitor_calibration_port(unsigned int number_iterations){

	Serial.println("collecting measurement data on calibration port!");

	unsigned long calibration_signal_timestamp, calibration_duty_timestamp;
	unsigned int calibration_duration, calibration_duty;

	for (unsigned int i=0; i<number_iterations; i++){
		// pull data and update stats for every new reading
		bool not_new = true;
		while (not_new == true){
			port_k_get_data(port_k_connected_device_types::RC_calibration, &calibration_signal_timestamp, &calibration_duration, &calibration_duty_timestamp, &calibration_duty);
			if (signal_timestamp != calibration_signal_timestamp){
				// new signal reading
				signal_timestamp = calibration_signal_timestamp;
				signal_summe+=calibration_duration;
				if (calibration_duration<signal_mini) signal_mini = calibration_duration;
				if (calibration_duration>signal_maxi) signal_maxi = calibration_duration;
				signal_count++;
				not_new = false;
			}
			if (duty_timestamp != calibration_duty_timestamp){
				// new duty reading
				duty_timestamp = calibration_duty_timestamp;
				duty_summe+=calibration_duty;
				if (calibration_duty<duty_mini) duty_mini = calibration_duty;
				if (calibration_duty>duty_maxi) duty_maxi = calibration_duty;
				duty_count++;
				not_new = false;
			}

		}
	}
}


// helpful for calibrating the pwm output, it cycles throuhg a range of pwm outputs and collects/prints the corresponding stats
// used to verify that that the conversion from ms (RC) to ticks (mega pwm duration) works
// for me to confirm the scaling factor of 2
void RC_calibration_class::monitor_calibration_port_for_pwm(unsigned int number_iterations){

	Serial.println("managing pwm while collecting measurement data on calibration port!");
	for (int step = 17000; step<20001; step+=500){
		Serial.print("setting target duration = ");
		Serial.println(step);
		pwm_set_signal_duration(step);
		monitor_calibration_port(number_iterations);
		print_stats();
		//delay(200);
	}
}

void RC_calibration_class::print_stats(){

	Serial.print("calibration:	signal duration   :   count , avg. duration, min, max : ");
	Serial.print(signal_count);
	Serial.print(",");
	Serial.print(signal_summe / signal_count);
	Serial.print(",");
	Serial.print(signal_mini);
	Serial.print(",");
	Serial.println(signal_maxi);

	Serial.print("calibration:	duty duration   :   count , avg. duration, min, max : ");
	Serial.print(duty_count);
	Serial.print(",");
	Serial.print(duty_summe / duty_count);
	Serial.print(",");
	Serial.print(duty_mini);
	Serial.print(",");
	Serial.println(duty_maxi);

}

RC_calibration_class::RC_calibration_class():signal_timestamp(0), duty_timestamp(0),signal_summe(0),duty_summe(0),signal_mini(999999),signal_maxi(0),duty_mini(999999),duty_maxi(0),signal_count(0),duty_count(0){
	Serial.println("creating calibration object to monitor stats on extint23 (analog 15 on mega)");
}

RC_calibration_class::~RC_calibration_class() {}

