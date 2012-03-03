#include <stdint.h>
#include "includes.h"

volatile uint32_t system_clock;

volatile Tcircle_buffer USART_buffer_RX;
volatile uint32_t USART_arrival_time;
volatile Tcircle_buffer USART_buffer_TX;

