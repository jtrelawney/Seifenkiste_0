#ifndef COMMON_H_
#define COMMON_H_

#include "Arduino.h"


// starting to implement a status with a global handler routine
struct state_def {
	volatile bool RC_emergency = false;
};

void G_emergency_routine();


#endif /* COMMON_H_ */
