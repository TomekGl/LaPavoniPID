/*
 * PID.c
 *
 *  Created on: 17-03-2012
 *      Author: tomek
 */

#include "includes.h"

volatile Tcontroller controller;

void PID_Init(void) {
	controller.SV = 270;
	controller.windup = 1000;
	controller.k_r = 0.1;
	controller.T_d = 1;
	controller.first = 1;
}

int16_t PID_Process(int16_t presentValue) {
	/*
	 * u(t) = k_r * [ e(t) + 1/T_i*(int(e)dt) + T_d*deriv(e(t)) ]
	 */
	static int16_t control=0, previousError,dt;

	dt = system_clock - controller.t_prev;
	controller.t_prev = system_clock;

	controller.PV = presentValue;
	previousError = controller.e;
	controller.e = controller.SV - controller.PV;

	//PROPORTIONAL
	control = controller.e;

	if (!controller.first) {

		//INTEGRAL
		controller.integral = dt *(controller.e + previousError)/2;
		if (0 != controller.windup) {
			if (controller.integral>controller.windup) { controller.integral = controller.windup; }
			else if (controller.integral<(controller.windup*-1)) { controller.integral = controller.windup*-1; }
		}
		control += controller.integral / controller.T_i;
		//DERIVATIVE
		//d=controller.


	} //first
	control*=controller.k_r;

	//controller.previousValue = controller.PV;

	return control;
}
