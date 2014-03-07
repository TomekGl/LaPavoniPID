/**
 @file PID.c

 @defgroup PID Implementation of PID controller
 @code #include <LCD.h> @endcode

 @brief Implementation of PID controller


 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 17-03-2012
*/
#include "includes.h"

volatile Tcontroller controller;
volatile Tcontroller_param controller_param;

void PID_Init(void) {
	USART_Put(eeprom_read_byte(0));
	if (PID_EEPROM_VERSION != eeprom_read_byte(0)) {
		LCD_PutStr("Eeprom bad version\n", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLACK, WHITE);
		//Load defaults
		PID_Reset();
		LCD_PutStr("Default restored\n", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLACK, WHITE);
	} else {
		//Load saved parameters
		eeprom_read_block((void*)&controller_param, PID_EEPROM_ADDRESS, sizeof(controller_param));
	}

	memset((void *)&controller, 0, sizeof(controller));
	/*controller.PV = 0;
	controller.e = 0;
	controller.integral = 0;
	controller.derivative = 0;
	controller.e_1 = 0;
	//controller.t_prev = 0;
	controller.y = 0;*/
	controller.firstpass = 1;
}

void PID_Reset(void) {
	controller_param.version = PID_EEPROM_VERSION;
	controller_param.SV = 93.5;
	controller_param.k_r = 3.5;
	controller_param.T_d = 0;
	controller_param.T_i  = 1;
	controller_param.windup = 10;
	controller_param.dead = 0;
	controller_param.limit_bottom = 0;
	controller_param.limit_top = 255;
	controller_param.alpha = 0.92;
	controller.firstpass = 1;

	//FIXME make separate struct
	controller_param.buzzer_enabled = 1;
	controller_param.lcd_brightness = 160;
	controller_param.preinfusion_duty_cycle = 4;
	controller_param.preinfusion_time = 40;
	controller_param.preinfusion_valve_off_delay = 30;
	controller_param.flowratefactor = 10.69; //4.380; [pulses/ml]
	controller_param.heater_power = 1000; // [W]

}

void PID_SaveSettings(void) {
	eeprom_update_block((const void*)&controller_param, PID_EEPROM_ADDRESS, sizeof(controller_param));
	eeprom_busy_wait();
}


/// Positional PID Algorithm
int16_t PID_Process(double processValue) {
	/**
	 * u(t) = k_r * [ e(t) + 1/T_i*(int(e)dt) + T_d*deriv(e(t)) ]
	 */
	static double dt;

	//dt = system_clock - controller.t_prev;
	dt = 1.0; // [s]
	//controller.t_prev = system_clock;

	controller.PV = processValue;

	controller.e = controller_param.SV - controller.PV;


	controller.y_1 = controller.y;


	//PROPORTIONAL TERM
	controller.y = controller.e;

	if (!controller.firstpass) {
		//INTEGRAL TERM
		controller.integral += (controller.e + controller.e_1)/2;
		if (0 != controller_param.windup) {
			if (controller.integral>controller_param.windup) {
				controller.integral = controller_param.windup;
			}
			else if (controller.integral<(controller_param.windup*-1)) {
				controller.integral = controller_param.windup*-1;
			}
		}
		//0.00 in menu can be not exactly euqal zero, check range
		if (!(controller_param.T_i < 0.1 && controller_param.T_i > -0.1)) {
			controller.y += (controller.integral /(controller_param.T_i))*dt;
		}
		//DERIVATIVE TERM
		//0.00 in menu means "term disabled", but can be not exactly euqal zero, so check range
		if (!(controller_param.T_d < 0.1 && controller_param.T_d > -0.1)) {
			controller.derivative = controller_param.T_d * (controller.e - controller.e_1) * dt;
			controller.y += controller.derivative;
		}

	} //first

	controller.y *= controller_param.k_r;
	//controller.y /= 10;

	if (controller.y>controller_param.limit_top) controller.y=controller_param.limit_top;
	if (controller.y<controller_param.limit_bottom) controller.y=controller_param.limit_bottom;

	controller.firstpass = 0;
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


	if (!controller.firstpass) {
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
	controller.firstpass = 0;

	if (controller.y>controller_param.limit_top) controller.y=controller_param.limit_top;
	if (controller.y<controller_param.limit_bottom) controller.y=controller_param.limit_bottom;

	return controller.y;
}

/// PID Algorithm
//TODO
int16_t PID_Process_3(float processValue) {

	/*
	 * function u=pid(x)
	global Tpid Tf t0 t1 tf k
	global sp ek uk uk_1 yk yk_1 vk vk_1
	% Digital filter and PID control
	% Designed by BESTune Comp.
	% WWW: http://bestune.50megs.com
	% Copyright 1998-2002
	t=x(6);        % Read time
	if t==0
	k=0;
	Tpid=1;     % PID loop update time
	t0=0;
	t1=0;
	uk=x(8);    % Initial CO value
	yk=x(1);    % Initial PV value
	sp=x(2);    % Initial setpoint
	ek=sp-yk;
	Tf=0.1;     % Filter time constant
	tf=200;     % Simulation stop time
	else           % Filtering yk
	T0=t-t0;
	x yk=yk*Tf/(Tf+T0)+x(1)*T0/(Tf+T0);
	t0=t;
	end
	% End of the filter.
	% The filtered yk will be used to
	% (1)calculate PID control and
	% (2)tune the PID controller
	% Beginning of the PID controller
	if t-t1>=Tpid | x(2)~=sp   %Every Tpid sec calculate the new CO value
	sp=x(2);
	T=t-t1;
	if T>0.0001             %Avoid division by T=0
	  if x(7)==1 & k>=3    % Then use PID control
	     Kp=x(3);
	     Ki=x(4);
	     Kd=x(5);
	     ek=sp-yk;
	     vk=(yk-yk_1)/T;
	     ak=(vk-vk_1)/T;
	     Du=(Ki*ek-Kp*vk-Kd*ak)*T;
	     uk=uk_1+Du;
	  else
	     if k>=1 vk=(yk-yk_1)/T; end
	     if k>=2 ak=(vk-vk_1)/T; end
	     uk=x(8);       % Use manual uk value x(8)
	     k=k+1;
	  end
	end
	if uk>5 uk=5;end     % uk must be <= upper limit
	if uk<-5 uk=-5;end   % uk must be >= lower limit

	% Show the control results and update t1, yk_1, vk_1, uk_1
	fprintf('time=%5.2f setpoint=%5.2f pv=%5.2f co=%5.2f\n',t,x(2),yk,uk)
	t1=t;yk_1=yk;vk_1=vk;uk_1=uk;
	end
	% End of the PID controller
	u=uk;
	 */

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


	if (!controller.firstpass) {
		controller.proportional = controller_param.k_r * (controller.e_1 - controller.e); //TODO kp, proportional to PV
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
	controller.firstpass = 0;

	if (controller.y>controller_param.limit_top) controller.y=controller_param.limit_top;
	if (controller.y<controller_param.limit_bottom) controller.y=controller_param.limit_bottom;

	return controller.y;
}

