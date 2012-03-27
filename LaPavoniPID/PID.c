/*
 * PID.c
 *
 *  Created on: 17-03-2012
 *      Author: tomek
 */

#include "includes.h"

volatile Tcontroller controller;

void PID_Init(void) {
	controller.SV = 370;
	controller.windup = 1000;
	controller.k_r = 3;
	controller.T_d = 1;
	controller.T_i  = 2;
	controller.first = 1;
	controller.limit_bottom = 0;
	controller.limit_top = 9999;
}

int16_t PID_Process(int16_t presentValue) {
	/*
	 * u(t) = k_r * [ e(t) + 1/T_i*(int(e)dt) + T_d*deriv(e(t)) ]
	 */
	static int16_t control=0, dt;

	//dt = system_clock - controller.t_prev;
	dt = 1; // [s]
	controller.t_prev = system_clock;

	controller.PV = presentValue;

	controller.e = controller.SV - controller.PV;



	//PROPORTIONAL
	control = controller.e;

	if (!controller.first) {
		//BUZZ_PORT |=_BV(BUZZ);
		//_delay_ms(10);
		//BUZZ_PORT &=~_BV(BUZZ);
		//INTEGRAL
		controller.integral += (controller.e + controller.previous)/2;
		if (0 != controller.windup) {
			if (controller.integral>controller.windup) { controller.integral = controller.windup; }
			else if (controller.integral<(controller.windup*-1)) { controller.integral = controller.windup*-1; }
		}
		if (0 != controller.T_i ) {
			control += (controller.integral /(controller.T_i))*dt;
		}

		if (0 != controller.T_d) {
			controller.derivative = controller.T_d * (controller.e - controller.previous) / dt;
			control += controller.derivative;
		}

	} //first

	control*=controller.k_r;

	if (control>controller.limit_top) control=controller.limit_top;
	if (control<controller.limit_bottom) control=controller.limit_bottom;

	controller.first = 0;
	controller.previous = controller.e;


	//controller.previousValue = controller.PV;
//BUZZ_PORT |=_BV(BUZZ);
//_delay_ms(10);
//BUZZ_PORT &=~_BV(BUZZ);

	return control;
}
