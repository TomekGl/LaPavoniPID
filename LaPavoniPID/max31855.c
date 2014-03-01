/**
 @file max31855.c

 @defgroup max31855 MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter driver

 This library provides routines to comunicate with MAX31855 via hardware SPI

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 11-03-2012
*/

#include "includes.h"
#define ADCCS0 ADC_CSPORT&=~_BV(ADC_CS);
#define ADCCS1 ADC_CSPORT|=_BV(ADC_CS);
#define CLK0 LCD_SPIPORT&=~_BV(LCD_SCK);
#define CLK1 LCD_SPIPORT|=_BV(LCD_SCK);

union MAX31855Data read_data;

void TC_Init(struct MAX31855Temp *temp)
{
	ADC_CSDDR |= _BV(ADC_CS);
	ADC_CSPORT |= _BV(ADC_CS); //High - inactive

	temp->LastReadStatus = TC_NOTREADY;
}

TTC_read_status  TC_PerformRead(struct MAX31855Temp *temp) {
	uint8_t status;

	SPCR |= _BV(SPE); // | _BV(MSTR); // Enable Hardware SPI
//	while(!(SPSR & (1<<SPIF)))// wait until previous transmission completed
//		;
	SPSR &= ~_BV(SPI2X);

	LCD_CTRPORT |= _BV(LCD_CS);
	ADCCS0
	_delay_us(1);
	for (uint8_t j=3; j<4; j--) {
		SPDR = 0xff; // send some data
		while(!(SPSR & (1<<SPIF)))// wait until send complete
			;
		read_data.byte[j]=SPDR;
	}
//	SPCR &= ~_BV(SPE);
	ADCCS1
	SPSR |= _BV(SPI2X);
	SPDR = 0xff; //send some data to ensure 1 in SPIF flag for LCD operation

	status = read_data.byte[0]&0x03;

	if (TC_READOK == status)
		TC_DecodeTemp(temp);
	return status;
}

inline void TC_DecodeTemp(struct MAX31855Temp *temp) {
	temp->TC.deg = (read_data.word[1]>>4);
	temp->TC.milideg = ((read_data.word[1]>>2)&0x03)*25;
	temp->TC.value = temp->TC.deg+temp->TC.milideg/100.0;

	temp->Internal.deg = (int16_t)read_data.byte[1]; //12bit internal temp
	temp->Internal.milideg = (uint8_t)(read_data.byte[0]>>4)*625;
	temp->Internal.value = temp->TC.deg+temp->TC.milideg/100.0;

	return;
}
