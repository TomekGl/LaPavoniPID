/*
 * PID.h
 *
 *  Created on: 17-03-2012
 *      Author: tomek
 */

#ifndef PID_H_
#define PID_H_


struct Tcontroller_tag { //BCD
	int16_t PV; //Present value
	int16_t SV; //Set value
	int16_t e;  //Error

	int16_t k_r; //Proportional gain
    int16_t T_i; //Integral time
    int16_t T_d; //Derivative time

    int16_t integral, derivative;
    int16_t previous;
    int16_t t_prev;
    uint8_t first;

    int16_t dead; //Dead zone
    int16_t windup; //Integral windup limit
    int16_t limit_bottom; //negative output limit
    int16_t limit_top; //positive output limit

};

typedef struct Tcontroller_tag Tcontroller;

extern volatile Tcontroller controller;
void PID_Init(void);
int16_t PID_Process(int16_t presentValue);
#endif /* PID_H_ */
