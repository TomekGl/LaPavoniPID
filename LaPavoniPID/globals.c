#include <stdint.h>
#include "includes.h"

volatile uint32_t system_clock;

uint8_t tmp_out1;
uint8_t tmp_out2;
uint8_t tmp_out3;
volatile uint8_t tmp_in;
volatile uint8_t in_flag;
uint16_t pump_timer;
uint16_t pump_timer_reset_timeout;
uint8_t timer0;
volatile uint8_t output=0, pwm=0;
double floatpv = 0;
double floattest = 3.14159;
volatile int16_t flow_meter_pulses = 0;
