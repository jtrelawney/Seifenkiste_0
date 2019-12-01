/*
 * controllercalibration.h
 *
 *  Created on: Oct 8, 2019
 *      Author: jconnor
 */

#ifndef CONTROLLERCALIBRATION_H_
#define CONTROLLERCALIBRATION_H_

#include <pwm.h>
#include <portkextint.h>

class controller_calibration {
public:
	controller_calibration();
	virtual ~controller_calibration();
};

class RC_calibration_class {
private:
	unsigned long signal_timestamp = 0, duty_timestamp = 0;
	unsigned long signal_summe = 0, duty_summe = 0;
	unsigned long signal_mini = 999999, signal_maxi = 0;
	unsigned long duty_mini = 999999, duty_maxi = 0;
	unsigned long signal_count = 0, duty_count = 0;

public:
	RC_calibration_class();
	void monitor_calibration_port(unsigned int number_iterations);
	void monitor_calibration_port_for_pwm(unsigned int number_iterations);

	void print_stats();
	virtual ~RC_calibration_class();
};

#endif /* CONTROLLERCALIBRATION_H_ */
