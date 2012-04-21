/*
 * PID.c
 *
 *  Created on: 17-03-2012
 *      Author: tomek
 */

#include "includes.h"

volatile Tcontroller controller;
volatile Tcontroller_param controller_param;

void PID_Init(void) {
	USART_Put(eeprom_read_byte(0));
	if (0xff==eeprom_read_byte(0)) {
		USART_Puts("Przywracam domyslne\n");
		//Load defaults
		PID_Reset();
	} else {
		//Load saved parameters
		eeprom_read_block((void*)&controller_param, PID_EEPROM_ADDRESS , sizeof(controller_param));
	}

	memset(&controller, 0, sizeof(controller));
	/*controller.PV = 0;
	controller.e = 0;
	controller.integral = 0;
	controller.derivative = 0;
	controller.e_1 = 0;
	//controller.t_prev = 0;
	controller.y = 0;*/
	controller.first = 1;
}

void PID_Reset(void) {
	controller_param.SV = 940;
	controller_param.k_r = 3;
	controller_param.T_d = 0;
	controller_param.T_i  = 0;
	controller_param.windup = 100;
	controller_param.dead = 20;
	controller_param.limit_bottom = 0;
	controller_param.limit_top = 255;

	controller.first = 1;
}

void PID_SaveSettings(void) {
	eeprom_update_block((const void*)&controller_param, PID_EEPROM_ADDRESS, sizeof(controller_param));
	eeprom_busy_wait();
}


/// Positional PID Algorithm
int16_t PID_Process(int16_t processValue) {
	/*
	 * u(t) = k_r * [ e(t) + 1/T_i*(int(e)dt) + T_d*deriv(e(t)) ]
	 */
	static int16_t dt;

	//dt = system_clock - controller.t_prev;
	dt = 1; // [s]
	//controller.t_prev = system_clock;

	controller.PV = processValue;

	controller.e = controller_param.SV - controller.PV;


    controller.y_1 = controller.y;


	//PROPORTIONAL TERM
	controller.y = controller.e;

	if (!controller.first) {
		//INTEGRAL TERM
		controller.integral += (controller.e + controller.e_1)/2;
		if (0 != controller_param.windup) {
			if (controller.integral>controller_param.windup) { controller.integral = controller_param.windup; }
			else if (controller.integral<(controller_param.windup*-1)) { controller.integral = controller_param.windup*-1; }
		}
		if (0 != controller_param.T_i ) {
			controller.y += (controller.integral /(controller_param.T_i))*dt;
		}
		//DERIVATIVE TERM
		if (0 != controller_param.T_d) {
			controller.derivative = controller_param.T_d * (controller.e - controller.e_1) * dt;
			controller.y += controller.derivative;
		}

	} //first

	controller.y *= controller_param.k_r;
	controller.y /= 10;

	if (controller.y>controller_param.limit_top) controller.y=controller_param.limit_top;
	if (controller.y<controller_param.limit_bottom) controller.y=controller_param.limit_bottom;

	controller.first = 0;
	controller.e_1 = controller.e;

	return controller.y;
}

/// Velocity PID Algorithm
int16_t PID_Process_2(int16_t processValue) {

	controller.PV = processValue;

	//calculate error
	controller.e = controller_param.SV - controller.PV;

	//-----------------------------------------------------------
	// Calculate PID controller:
	// y[k] = y[k-1] + Kc*(PV[k-1] - PV[k] +
	//							Ts*e[k]/Ti +
	// 							Td/Ts*(2*PV[k-1] - PV[k] - PV[k-2]))
	//-----------------------------------------------------------

	/// Type C PID controller http://bestune.50megs.com/typeABC.htm
	//  CO[k] = CO[k-1] + Kp*(PV[k-1]-PV[k]) + Ki*T*e[k] -
	//              (Kd/T)*(PV[k]-2PV[k]+PV[k-1]+PV[k-2])


	if (!controller.first) {
		controller.proportional = controller_param.k_r * (controller.e_1 - controller.e); //TODO kp, proportional from PV
		controller.integral = controller_param.T_i * controller.e;
		//controller.integral /= 100;
		//controller.derivative = 0; //controller_param.T_d *
		controller.y += controller.proportional + controller.integral + controller.derivative;
		/*
		p->pp = p->kc * (xk_1 - xk); // y[k] = y[k-1] + Kc*(PV[k-1] - PV[k])
		p->pi = p->k0 * ek;
		//
		+ Kc*Ts/Ti * e[k]
		p->pd = p->k1 * (2.0 * xk_1 - xk - xk_2);
		*yk += p->pp + p->pi + p->pd;*/

	}
	controller.y_1 = controller.y;
	controller.e_1 = controller.e;
	controller.PV_2 = controller.PV_1;
	controller.PV_1 = controller.PV;
	controller.first = 0;

	if (controller.y>controller_param.limit_top) controller.y=controller_param.limit_top;
	if (controller.y<controller_param.limit_bottom) controller.y=controller_param.limit_bottom;

	return controller.y;
}
