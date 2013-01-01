#ifndef PID_H_
#define PID_H_

/**
 @file PID.h

 @defgroup PID Implementation of PID controller
 @code #include <LCD.h> @endcode

 @brief Implementation of PID controller


 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 17-03-2012
*/
/** @{ */

///location of saved settings in EEPROM
#define PID_EEPROM_ADDRESS 0

///current version of data structure
#define PID_EEPROM_VERSION 11

///controller presets kept in RAM & EEPROM - structure definition
struct Tcontroller_param_tag {
	/* controller parameters */
	int8_t version; ///< Header
	double SV; ///< Set point (SP is reserved for stack pointer :) )
	double k_r; ///< Regulator's gain
	double k_p; ///< Proportional gain
	double T_i; ///< Integral time
	double T_d; ///< Derivative time
	double T_s; ///< Time constant
	double dead; ///< Dead band
	double windup; ///< Integral anti-windup limit
	double limit_bottom; ///< negative output limit
	double limit_top; ///< positive output limit
	/* filter presets */
	double  alpha; ///< Filter coefficient
	/* pre-infusion presets */
	int8_t  preinfusion_time; ///<pre-infussion time
	int8_t  preinfusion_duty_cycle; ///<pump duty-cycle during pre-infusion
	int8_t  preinfusion_valve_off_delay; ///<valve off delay after finished extraction
	uint8_t  buzzer_enabled; ///< buzzer enabled
	uint8_t  lcd_brightness; ///< LCD brightness
};

///internal runtime variables of controller structure definition
struct Tcontroller_tag {
	/* run-time variables */
	double PV; ///< Process value
	double PV_1; ///< PV[t-1]
	double PV_2; ///< PV[t-2]
	double e;  ///< Error
	double e_1; ///< Previous error
	double y; ///< Output
	double y_1; ///< previous output value
	double proportional; ///< present value of proportional term
	double integral; ///< present value of integral term
	double derivative; ///< present value of derivative term
	uint8_t firstpass; ///< whether first pass
};

///controller presets kept in RAM & EEPROM - type definition
typedef struct Tcontroller_tag Tcontroller;
///internal runtime variables of controller - type definition
typedef struct Tcontroller_param_tag Tcontroller_param;

///controller presets kept in RAM
extern volatile Tcontroller controller;
///internal runtime variables of controller
extern volatile Tcontroller_param controller_param;

//!  Initialize controller
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
int16_t PID_Process(double processValue);

int16_t PID_Process_2(int16_t processValue);

/** @} */

#endif /* PID_H_ */
