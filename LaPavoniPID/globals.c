#include <stdint.h>
#include "includes.h"

volatile uint32_t system_clock;

//volatile Tcircle_buffer USART_buffer_RX;
volatile uint32_t USART_arrival_time;
//volatile Tcircle_buffer USART_buffer_TX;
//volatile Tsystime systime;


uint8_t tmp_buzz;
uint8_t tmp_out1;
uint8_t tmp_out2;
uint8_t tmp_out3;
volatile uint8_t tmp_in;
volatile uint8_t in_flag;
uint16_t pump_timer;
uint16_t pump_timer_reset_timeout;
uint8_t timer0;
volatile uint8_t output=0, pwm=0;
