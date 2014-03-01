#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>
#include "includes.h"

/// System clock
extern volatile uint32_t SystemClock;

extern uint8_t tmp_out1;
extern uint8_t tmp_out2;
extern uint8_t tmp_out3;
extern uint16_t pump_timer;
extern uint16_t pump_timer_reset_timeout;
extern volatile uint8_t tmp_in;
extern volatile uint8_t in_flag;
extern uint8_t timer0;
extern volatile uint8_t output;
extern volatile uint8_t pwm;
extern double Temperature;
extern volatile int16_t FlowMeterPulses;


#endif				/*GLOBALS_H_ */
