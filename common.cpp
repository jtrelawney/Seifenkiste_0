/*
 * utilities.cpp
 *
 *  Created on: Sep 24, 2019
 *      Author: jconnor
 */

#include <common.h>

state_def G_state;

void G_emergency_routine(){
	if ( G_state.RC_emergency == true )	Serial.println("emergency");
};

