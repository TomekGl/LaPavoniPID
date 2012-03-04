/*
 * lcd.c
 *
 *  Created on: 04-03-2012
 *      Author: tomek
 */
#include "includes.h"

void sendCMD(uint8_t cmd);
void sendData(uint8_t cmd);
void shiftBits(uint8_t b);
void setPixel(uint8_t r,uint8_t g,uint8_t b);

#define cbi(reg, bit) (reg&=~(1<<bit))
#define sbi(reg, bit) (reg|= (1<<bit))

#define CS0 cbi(LCD_CTRPORT,LCD_CS);
#define CS1 sbi(LCD_CTRPORT,LCD_CS);
#define CLK0 cbi(LCD_SPIPORT,LCD_SCK);
#define CLK1 sbi(LCD_SPIPORT,LCD_SCK);
#define SDA0 cbi(LCD_SPIPORT,LCD_SDA);
#define SDA1 sbi(LCD_SPIPORT,LCD_SDA);
#define RESET0 cbi(LCD_CTRPORT,LCD_RESET);
#define RESET1 sbi(LCD_CTRPORT,LCD_RESET);

#define uint8_t unsigned char
uint8_t n=0;
uint8_t s1,s2;
uint8_t r,g,b;


void lcdwaitms(int ms) {
  int i;
  for (i=0;i<ms;i++) _delay_ms(1);
}



void LCD_Init() {
	//Configure outputs
	LCD_SPIDDR = _BV(LCD_SDA) | _BV(LCD_SCK); //Port-Direction Setup
	LCD_CTRDDR = _BV(LCD_CS) | _BV(LCD_RESET); //Port-Direction Setup

	//Backlight
	LCD_BLDDR |= _BV(LCD_BL);
	//LCD_BLPORT &= ~_BV(LCD_BL);
	LCD_BLPORT |= _BV(LCD_BL);

	CS0
	  SDA0
	  CLK1

	  RESET1
	  RESET0
	  lcdwaitms(10);
	  RESET1

	  CLK1
	  SDA1
	  CLK1
	  lcdwaitms(10);
	 //Software Reset
	  sendCMD(0x01);

	  //Sleep Out
	  sendCMD(0x11);

	  //Booster ON
	  sendCMD(0x03);

	  lcdwaitms(10);

	  //Display On
	  sendCMD(0x29);

	  //Normal display mode
	  sendCMD(0x13);

	  //Display inversion off
	  sendCMD(0x20);

	  //Data order
	  sendCMD(0xBA);

	  //Memory data access control
	  sendCMD(0x36);
	  sendData(0);
	 //sendData(8|64);   //rgb + MirrorX
	 //sendData(8|128);   //rgb + MirrorY

	#ifdef MODE565
	  sendCMD(0x3A);
	  sendData(5);   //16-Bit per Pixel
	#else
	  //sendCMD(0x3A);
	  //sendData(3);   //12-Bit per Pixel (default)
	#endif


	  //Set Constrast
	  sendCMD(0x25);
	  sendData(63);

	  sendCMD(0x00); //NOP
	  CS1;

}

void LCD_Test(uint8_t mode) {
	CS0;

	//Column Adress Set
	sendCMD(0x2A);
	sendData(0);
	sendData(131);

	//Page Adress Set
	sendCMD(0x2B);
	sendData(0);
	sendData(131);

	//Memory Write
	sendCMD(0x2C);
	int i;
	//Test-Picture

	//red bar
	for (i=0;i<132*33;i++) {
		setPixel(255,0,0);
	}

	//green bar
	for (i=0;i<132*33;i++) {
		setPixel(0,255,0);
	}

	//blue bar
	for (i=0;i<132*33;i++) {
		setPixel(0,0,255);
	}

	//white bar
	for (i=0;i<132*33;i++) {
		setPixel(255,255,255);
	}
	CS1
}

void shiftBits(uint8_t b) {

  CLK0
  if ((b&128)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&64)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&32)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&16)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&8)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&4)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&2)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&1)!=0) SDA1 else SDA0
  CLK1

}
//send data
void sendData(uint8_t data) {

  CLK0
  SDA1                                                 //1 for param
  CLK1

  shiftBits(data);
}

//send cmd
void sendCMD(uint8_t data) {

  CLK0
  SDA0                                                 //1 for cmd
  CLK1

  shiftBits(data);
}

//converts a 3*8Bit-RGB-Pixel to the 2-Byte-RGBRGB Format of the Display
void setPixel(uint8_t r,uint8_t g,uint8_t b) {
#ifndef MODE565
   sendData((r&248)|g>>5);
   sendData((g&7)<<5|b>>3);
#else
  if (n==0) {
    s1=(r & 240) | (g>>4);
    s2=(b & 240);
    n=1;
  } else {
    n=0;
    sendData(s1);
    sendData(s2|(r>>4));
    sendData((g&240) | (b>>4));
  }
#endif
}
