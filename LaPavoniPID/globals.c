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
volatile uint8_t output, pwm;
double Temperature;
struct MAX31855Temp TemperatureRaw;
volatile int16_t FlowMeterPulses, FlowMeterPulsesPrev, FlowMeterPulsesPrev1s;
