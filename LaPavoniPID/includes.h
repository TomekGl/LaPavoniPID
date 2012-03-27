//#ifndef INCLUDES_H_
//#define INCLUDES_H_

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>
#include <util/twi.h>

#include "buffers.h"
#include "hardware.h"
//#include "TWI_Master.h"
//#include "i2cmaster.h"
#include "serial.h"
//#include "crc8.h"
#include "delay.h"
#include "globals.h"
#include "lcd.h"
#include "max31855.h"
#include "PID.h"
//#include "ds1307.h"
#include "main.h"
#include "menu.h"

#define cbi(reg, bit) (reg&=~(1<<bit))
#define sbi(reg, bit) (reg|= (1<<bit))
//#endif /*INCLUDES_H_*/
