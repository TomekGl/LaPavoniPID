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

void TC_Init()
{
	ADC_CSDDR |= _BV(ADC_CS);
	ADC_CSPORT |= _BV(ADC_CS); //High - inactive
}

TTC_read_status  TC_PerformRead() {
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

	return read_data.byte[0]&0x3;
}

//TTC_read_status  TC_PerformRead() {
//	LCD_CTRPORT |= _BV(LCD_CS); //disable lcd CS
//	_delay_ms(1);
//	SPCR |= _BV(SPE); // Enable Hardware SPI
//
//	ADCCS0
//	_delay_us(1);
//	for (uint8_t j=3; j<4; j--) {
//		SPDR = 0xff; // send some data
//		while(!(SPSR & (1<<SPIF)))// wait until send complete
//			;
//		read_data.byte[j]=SPDR;
//	}
////	SPCR &= ~_BV(SPE);
//	ADCCS1
//	SPDR = 0xff; //send some data to ensure 1 in SPIF flag
//	_delay_ms(1);
//	LCD_CTRPORT &= ~_BV(LCD_CS);
//	return read_data.byte[0]&0x3;
//}


/*void TC_debug() {
	for (uint8_t j=0; j<4; j++) {
		USART_TransmitBinary(read_data.byte[j]);
	}
	USART_Put('_');
	USART_TransmitDecimal(read_data.integer);
}
*/


void TC_GetTCTemp(int16_t *deg, uint16_t *milideg) {
	*deg = (read_data.word[1]>>4);
	*milideg = ((read_data.word[1]>>2)&0x03)*25;
	return;
}

void TC_GetInternalTemp(int16_t *deg, uint16_t *milideg) {
	*deg = (int16_t)read_data.byte[1]; //12bit internal temp
	*milideg = (uint8_t)(read_data.byte[0]>>4)*625;
	return;
}

void TC_DecodeTemp(struct MAX31855Temp *temp) {
	temp->TC.deg = (read_data.word[1]>>4);
	temp->TC.milideg = ((read_data.word[1]>>2)&0x03)*25;

	temp->Internal.deg = (int16_t)read_data.byte[1]; //12bit internal temp
	temp->Internal.milideg = (uint8_t)(read_data.byte[0]>>4)*625;

	return;
}
