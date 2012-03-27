#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>
#include "includes.h"

extern volatile uint32_t system_clock;

//extern volatile Tcircle_buffer USART_buffer_RX;
extern volatile uint32_t USART_arrival_time;
//extern volatile Tcircle_buffer USART_buffer_TX;
//extern volatile Tsystime systime;

extern uint8_t tmp_buzz;
extern uint8_t tmp_out1;
extern uint8_t tmp_out2;
extern uint8_t tmp_out3;
extern uint8_t tmp_in;

#endif				/*GLOBALS_H_ */
