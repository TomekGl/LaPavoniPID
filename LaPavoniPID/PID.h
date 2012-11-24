/*! \file PID.h
    \brief Implementation of PID controller

*/

/*
 * PID.h
 *
 *  Created on: 17-03-2012
 *      Author: tomek
 */

#ifndef PID_H_
#define PID_H_

/** \defgroup PID Implementation of PID controller
 @{ */

///location of saved settings in EEPROM
#define PID_EEPROM_ADDRESS 0

///current version of data structure
#define PID_EEPROM_VERSION 6

///parameters of controller
struct Tcontroller_param_tag {
 /* controller parameters */
	int8_t version; ///< Header
	int16_t SV; ///< Set point
	int16_t k_r; ///< Regulator's gain
	int16_t k_p; ///< Proportional gain
    int16_t T_i; ///< Integral time
    int16_t T_d; ///< Derivative time
    int16_t T_s; ///< Time constant
    int16_t dead; ///< Dead band
    int16_t windup; ///< Integral anti-windup limit
    int16_t limit_bottom; ///< negative output limit
    int16_t limit_top; ///< positive output limit
    int8_t  alpha; ///< Filter coefficient 0 < alpha < 128
};

///internal runtime values of controller
struct Tcontroller_tag {
  /* run-time variables */
	int16_t PV; ///< Process value
	int16_t PV_1; ///< PV[t-1]
	int16_t PV_2; // PV[t-2]
	int16_t e;  //Error
	int16_t e_1;
	int16_t y; //Output
	int16_t y_1; //previous output value
    int16_t proportional, integral, derivative;
    uint8_t firstpass;
};

typedef struct Tcontroller_tag Tcontroller;
typedef struct Tcontroller_param_tag Tcontroller_param;

extern volatile Tcontroller controller;
extern volatile Tcontroller_param controller_param;


/**
 * Initialize internal structures of PID controller
 */
void PID_Init(void);

/**
 * Reset PID setting to defaults
 */
void PID_Reset(void);

/**
 * Save current PID settings into EEPROM
 */
void PID_SaveSettings(void);

/**
 * Compute next output
 * @param processValue last measured process value
 */
int16_t PID_Process(int16_t processValue);
int16_t PID_Process_2(int16_t processValue);

/** @} */

#endif /* PID_H_ */
