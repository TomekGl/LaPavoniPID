/*
 * PID.c
 *
 *  Created on: 17-03-2012
 *      Author: tomek
 */

#include "includes.h"

volatile Tcontroller controller;

void PID_Init(void) {
	USART_Put(eeprom_read_byte(0));
	if (0xff==eeprom_read_byte(0)) {
		USART_Puts("Przywracam domyslne\n");
		//Load defaults
		PID_Reset();
	} else {
		//Load saved parameters
		eeprom_read_block((void*)&controller, 0 , 8*2);
	}

	controller.PV = 0;
	controller.e = 0;
	controller.integral = 0;
	controller.derivative = 0;
	controller.previous = 0;
	controller.first = 1;
	controller.t_prev = 0;
	controller.control = 0;
}

void PID_Reset(void) {
	controller.SV = 340;
	controller.k_r = 3;
	controller.T_d = 0;
	controller.T_i  = 0;
	controller.first = 1;
	controller.windup = 100;
	controller.dead = 20;
	controller.limit_bottom = 0;
	controller.limit_top = 255;
}

void PID_SaveSettings(void) {
	eeprom_update_block((const void*)&controller, 0, 8*2);
	eeprom_busy_wait();
}

int16_t PID_Process(int16_t presentValue) {
	/*
	 * u(t) = k_r * [ e(t) + 1/T_i*(int(e)dt) + T_d*deriv(e(t)) ]
	 */
	static int16_t dt;

	//dt = system_clock - controller.t_prev;
	dt = 1; // [s]
	controller.t_prev = system_clock;

	controller.PV = presentValue;

	controller.e = controller.SV - controller.PV;



	//PROPORTIONAL
	controller.control = controller.e;

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
			controller.control += (controller.integral /(controller.T_i))*dt;
		}

		if (0 != controller.T_d) {
			controller.derivative = controller.T_d * (controller.e - controller.previous) / dt;
			controller.control += controller.derivative;
		}

	} //first

	controller.control *= controller.k_r;

	if (controller.control>controller.limit_top) controller.control=controller.limit_top;
	if (controller.control<controller.limit_bottom) controller.control=controller.limit_bottom;

	controller.first = 0;
	controller.previous = controller.e;


	//controller.previousValue = controller.PV;
//BUZZ_PORT |=_BV(BUZZ);
//_delay_ms(10);
//BUZZ_PORT &=~_BV(BUZZ);

	return controller.control;
}
