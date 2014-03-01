#include <stdint.h>
#include "includes.h"

volatile uint32_t SystemClock;

uint8_t tmp_out1;
uint8_t tmp_out2;
uint8_t tmp_out3;
volatile uint8_t tmp_in;
volatile uint8_t in_flag;
uint16_t pump_timer;
uint16_t pump_timer_reset_timeout;
uint8_t timer0;
volatile uint8_t output=0, pwm=0;
double Temperature = 0;
volatile int16_t FlowMeterPulses = 0;
