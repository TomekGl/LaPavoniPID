/**
 @file lcd.c
 @defgroup lcd LCD based on PCF8833 library

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 04-03-2012

 Fonts and char lookup table algorithm was written by Jim Parise, James P Lynch, see comments below
*/
#include "includes.h"


void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t cmd);
void LCD_SetPixel(uint16_t col);

#ifdef MODE8BPP
void LCD_SetPixel8(uint8_t col);
#endif

#define CS0    LCD_CTRPORT&=~_BV(LCD_CS);
#define CS1    {_delay_us(2);LCD_CTRPORT|=_BV(LCD_CS);}
#define CLK0   LCD_SPIPORT&=~_BV(LCD_SCK);
#define CLK1   LCD_SPIPORT|=_BV(LCD_SCK);
#define MOSI0  LCD_SPIPORT&=~_BV(LCD_SDA);
#define MOSI1  LCD_SPIPORT|=_BV(LCD_SDA);
#define RESET0 LCD_CTRPORT&=~_BV(LCD_RESET);
#define RESET1 LCD_CTRPORT|=_BV(LCD_RESET);

uint8_t n=0; //temporary for 12BPP writing
uint8_t s1,s2; //temporary for 12BPP writing

int8_t cursorx=0,cursory=0; //cursors' position for autoincrement strings print

#ifdef LCDTEXT
// *********************************************************************************
//
//			Font tables for Nokia 6610 LCD Display Driver (S1D15G00 Controller)
//
//			FONT6x8 - SMALL font (mostly 5x7)
//			FONT8x8 - MEDIUM font (8x8 characters, a bit thicker)
//			FONT8x16 - LARGE font (8x16 characters, thicker)
//
//			Note: ASCII characters 0x00 through 0x1F are not included in these fonts.
//
// Author: Jim Parise, James P Lynch August 30, 2007
// *********************************************************************************
/*
const unsigned char __attribute__ ((progmem))   FONT6x8[97][8] = {
		0x06,0x08,0x08,0x00,0x00,0x00,0x00,0x00, 	//	columns, rows, num_bytes_per_char
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	space	0x20
		0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00,	//	!
		0x50,0x50,0x50,0x00,0x00,0x00,0x00,0x00,	//	"
		0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00,	//	#
		0x20,0x78,0xA0,0x70,0x28,0xF0,0x20,0x00,	//	$
		0xC0,0xC8,0x10,0x20,0x40,0x98,0x18,0x00,	//	%
		0x40,0xA0,0xA0,0x40,0xA8,0x90,0x68,0x00,	//	&
		0x30,0x30,0x20,0x40,0x00,0x00,0x00,0x00,	//	'
		0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00,	//	(
		0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00,	//	)
		0x00,0x20,0xA8,0x70,0x70,0xA8,0x20,0x00,	//	*
		0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00,	//	0
		0x00,0x00,0x00,0x00,0x30,0x30,0x20,0x40,	//	0
		0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,	//	-0
		0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,	//	.
		0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,	//	/ (forward slash)
		0x70,0x88,0x88,0xA8,0x88,0x88,0x70,0x00,	//	0	0x30
		0x20,0x60,0x20,0x20,0x20,0x20,0x70,0x00,	//	1
		0x70,0x88,0x08,0x70,0x80,0x80,0xF8,0x00,	//	2
		0xF8,0x08,0x10,0x30,0x08,0x88,0x70,0x00,	//	3
		0x10,0x30,0x50,0x90,0xF8,0x10,0x10,0x00,	//	4
		0xF8,0x80,0xF0,0x08,0x08,0x88,0x70,0x00,	//	5
		0x38,0x40,0x80,0xF0,0x88,0x88,0x70,0x00,	//	6
		0xF8,0x08,0x08,0x10,0x20,0x40,0x80,0x00,	//	7
		0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00,	//	8
		0x70,0x88,0x88,0x78,0x08,0x10,0xE0,0x00,	//	9
		0x00,0x00,0x20,0x00,0x20,0x00,0x00,0x00,	//	:
		0x00,0x00,0x20,0x00,0x20,0x20,0x40,0x00,	//	;
		0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x00,	//	<
		0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00,	//	=
		0x40,0x20,0x10,0x08,0x10,0x20,0x40,0x00,	//	>
		0x70,0x88,0x08,0x30,0x20,0x00,0x20,0x00,	//	?
		0x70,0x88,0xA8,0xB8,0xB0,0x80,0x78,0x00,	//	@	0x40
		0x20,0x50,0x88,0x88,0xF8,0x88,0x88,0x00,	//	A
		0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00,	//	B
		0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00,	//	C
		0xF0,0x88,0x88,0x88,0x88,0x88,0xF0,0x00,	//	D
		0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00,	//	E
		0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00,	//	F
		0x78,0x88,0x80,0x80,0x98,0x88,0x78,0x00,	//	G
		0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00,	//	H
		0x70,0x20,0x20,0x20,0x20,0x20,0x70,0x00,	//	I
		0x38,0x10,0x10,0x10,0x10,0x90,0x60,0x00,	//	J
		0x88,0x90,0xA0,0xC0,0xA0,0x90,0x88,0x00,	//	K
		0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00,	//	L
		0x88,0xD8,0xA8,0xA8,0xA8,0x88,0x88,0x00,	//	M
		0x88,0x88,0xC8,0xA8,0x98,0x88,0x88,0x00,	//	N
		0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00,	//	O
		0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00,	//	P	0x50
		0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00,	//	Q
		0xF0,0x88,0x88,0xF0,0xA0,0x90,0x88,0x00,	//	R
		0x70,0x88,0x80,0x70,0x08,0x88,0x70,0x00,	//	S
		0xF8,0xA8,0x20,0x20,0x20,0x20,0x20,0x00,	//	T
		0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,	//	U
		0x88,0x88,0x88,0x88,0x88,0x50,0x20,0x00,	//	V
		0x88,0x88,0x88,0xA8,0xA8,0xA8,0x50,0x00,	//	W
		0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00,	//	X
		0x88,0x88,0x50,0x20,0x20,0x20,0x20,0x00,	//	Y
		0xF8,0x08,0x10,0x70,0x40,0x80,0xF8,0x00,	//	Z
		0x78,0x40,0x40,0x40,0x40,0x40,0x78,0x00,	//	[
		0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,	//	\ (back slash)
		0x78,0x08,0x08,0x08,0x08,0x08,0x78,0x00,	//	]
		0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00,	//	^
		0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,	//	_
		0x60,0x60,0x20,0x10,0x00,0x00,0x00,0x00,	//	`	0x60
		0x00,0x00,0x60,0x10,0x70,0x90,0x78,0x00,	//	a
		0x80,0x80,0xB0,0xC8,0x88,0xC8,0xB0,0x00,	//	b
		0x00,0x00,0x70,0x88,0x80,0x88,0x70,0x00,	//	c
		0x08,0x08,0x68,0x98,0x88,0x98,0x68,0x00,	//	d
		0x00,0x00,0x70,0x88,0xF8,0x80,0x70,0x00,	//	e
		0x10,0x28,0x20,0x70,0x20,0x20,0x20,0x00,	//	f
		0x00,0x00,0x70,0x98,0x98,0x68,0x08,0x70,	//	g
		0x80,0x80,0xB0,0xC8,0x88,0x88,0x88,0x00,	//	h
		0x20,0x00,0x60,0x20,0x20,0x20,0x70,0x00,	//	i
		0x10,0x00,0x10,0x10,0x10,0x90,0x60,0x00,	//	j
		0x80,0x80,0x90,0xA0,0xC0,0xA0,0x90,0x00,	//	k
		0x60,0x20,0x20,0x20,0x20,0x20,0x70,0x00,	//	l
		0x00,0x00,0xD0,0xA8,0xA8,0xA8,0xA8,0x00,	//	m
		0x00,0x00,0xB0,0xC8,0x88,0x88,0x88,0x00,	//	n
		0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00,	//	o
		0x00,0x00,0xB0,0xC8,0xC8,0xB0,0x80,0x80,	//	p	0x70
		0x00,0x00,0x68,0x98,0x98,0x68,0x08,0x08,	//	q
		0x00,0x00,0xB0,0xC8,0x80,0x80,0x80,0x00,	//	r
		0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00,	//	s
		0x20,0x20,0xF8,0x20,0x20,0x28,0x10,0x00,	//	t
		0x00,0x00,0x88,0x88,0x88,0x98,0x68,0x00,	//	u
		0x00,0x00,0x88,0x88,0x88,0x50,0x20,0x00,	//	v
		0x00,0x00,0x88,0x88,0xA8,0xA8,0x50,0x00,	//	w
		0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00,	//	x
		0x00,0x00,0x88,0x88,0x78,0x08,0x88,0x70,	//	y
		0x00,0x00,0xF8,0x10,0x20,0x40,0xF8,0x00,	//	z
		0x10,0x20,0x20,0x40,0x20,0x20,0x10,0x00,	//	{
		0x20,0x20,0x20,0x00,0x20,0x20,0x20,0x00,	//	|
		0x40,0x20,0x20,0x10,0x20,0x20,0x40,0x00,	//	}
		0x40,0xA8,0x10,0x00,0x00,0x00,0x00,0x00,	//	~
		0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00};	//	DEL
*/

const unsigned char __attribute__ ((progmem)) FONT8x8[97][8] = {
	0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,	//	columns, rows, num_bytes_per_char
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	space	0x20
	0x30,0x78,0x78,0x30,0x30,0x00,0x30,0x00,	//	!
	0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,	//	"
	0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00,	//	#
	0x18,0x3E,0x60,0x3C,0x06,0x7C,0x18,0x00,	//	$
	0x00,0x63,0x66,0x0C,0x18,0x33,0x63,0x00,	//	%
	0x1C,0x36,0x1C,0x3B,0x6E,0x66,0x3B,0x00,	//	&
	0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,	//	'
	0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00,	//	(
	0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00,	//	)
	0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,	//	*
	0x00,0x30,0x30,0xFC,0x30,0x30,0x00,0x00,	//	0
	0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30,	//	0
	0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,	//	-0
	0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,	//	.
	0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00,	//	/ (forward slash)
	0x3E,0x63,0x63,0x6B,0x63,0x63,0x3E,0x00,	//	0	0x30
	0x18,0x38,0x58,0x18,0x18,0x18,0x7E,0x00,	//	1
	0x3C,0x66,0x06,0x1C,0x30,0x66,0x7E,0x00,	//	2
	0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00,	//	3
	0x0E,0x1E,0x36,0x66,0x7F,0x06,0x0F,0x00,	//	4
	0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00,	//	5
	0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00,	//	6
	0x7E,0x66,0x06,0x0C,0x18,0x18,0x18,0x00,	//	7
	0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00,	//	8
	0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00,	//	9
	0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00,	//	:
	0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30,	//	;
	0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00,	//	<
	0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,	//	=
	0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00,	//	>
	0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00,	//	?
	0x3E,0x63,0x6F,0x69,0x6F,0x60,0x3E,0x00,	//	@	0x40
	0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00,	//	A
	0x7E,0x33,0x33,0x3E,0x33,0x33,0x7E,0x00,	//	B
	0x1E,0x33,0x60,0x60,0x60,0x33,0x1E,0x00,	//	C
	0x7C,0x36,0x33,0x33,0x33,0x36,0x7C,0x00,	//	D
	0x7F,0x31,0x34,0x3C,0x34,0x31,0x7F,0x00,	//	E
	0x7F,0x31,0x34,0x3C,0x34,0x30,0x78,0x00,	//	F
	0x1E,0x33,0x60,0x60,0x67,0x33,0x1F,0x00,	//	G
	0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,	//	H
	0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,	//	I
	0x0F,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,	//	J
	0x73,0x33,0x36,0x3C,0x36,0x33,0x73,0x00,	//	K
	0x78,0x30,0x30,0x30,0x31,0x33,0x7F,0x00,	//	L
	0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00,	//	M
	0x63,0x73,0x7B,0x6F,0x67,0x63,0x63,0x00,	//	N
	0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,	//	O
	0x7E,0x33,0x33,0x3E,0x30,0x30,0x78,0x00,	//	P	0x50
	0x3C,0x66,0x66,0x66,0x6E,0x3C,0x0E,0x00,	//	Q
	0x7E,0x33,0x33,0x3E,0x36,0x33,0x73,0x00,	//	R
	0x3C,0x66,0x30,0x18,0x0C,0x66,0x3C,0x00,	//	S
	0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00,	//	T
	0x66,0x66,0x66,0x66,0x66,0x66,0x7E,0x00,	//	U
	0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,	//	V
	0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00,	//	W
	0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00,	//	X
	0x66,0x66,0x66,0x3C,0x18,0x18,0x3C,0x00,	//	Y
	0x7F,0x63,0x46,0x0C,0x19,0x33,0x7F,0x00,	//	Z
	0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,	//	[
	0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00,	//	\ (back slash)
	0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,	//	]
	0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,	//	^
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,	//	_
	0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,	//	`	0x60
	0x00,0x00,0x3C,0x06,0x3E,0x66,0x3B,0x00,	//	a
	0x70,0x30,0x3E,0x33,0x33,0x33,0x6E,0x00,	//	b
	0x00,0x00,0x3C,0x66,0x60,0x66,0x3C,0x00,	//	c
	0x0E,0x06,0x3E,0x66,0x66,0x66,0x3B,0x00,	//	d
	0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00,	//	e
	0x1C,0x36,0x30,0x78,0x30,0x30,0x78,0x00,	//	f
	0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x7C,	//	g
	0x70,0x30,0x36,0x3B,0x33,0x33,0x73,0x00,	//	h
	0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00,	//	i
	0x06,0x00,0x06,0x06,0x06,0x66,0x66,0x3C,	//	j
	0x70,0x30,0x33,0x36,0x3C,0x36,0x73,0x00,	//	k
	0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,	//	l
	0x00,0x00,0x66,0x7F,0x7F,0x6B,0x63,0x00,	//	m
	0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00,	//	n
	0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00,	//	o
	0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78,	//	p	0x70
	0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F,	//	q
	0x00,0x00,0x6E,0x3B,0x33,0x30,0x78,0x00,	//	r
	0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00,	//	s
	0x08,0x18,0x3E,0x18,0x18,0x1A,0x0C,0x00,	//	t
	0x00,0x00,0x66,0x66,0x66,0x66,0x3B,0x00,	//	u
	0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00,	//	v
	0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00,	//	w
	0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00,	//	x
	0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x7C,	//	y
	0x00,0x00,0x7E,0x4C,0x18,0x32,0x7E,0x00,	//	z
	0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00,	//	{
	0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x0C,0x00,	//	|
	0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00,	//	}
	0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00,	//	~
	0x1C,0x36,0x36,0x1C,0x00,0x00,0x00,0x00		//	DEL
} ;



const unsigned char __attribute__ ((progmem)) FONT8x16[97][16] = {
	0x08,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	columns, rows, num_bytes_per_char
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	space	0x20
	0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,	//	!
	0x00,0x63,0x63,0x63,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	"
	0x00,0x00,0x00,0x36,0x36,0x7F,0x36,0x36,0x36,0x7F,0x36,0x36,0x00,0x00,0x00,0x00,	//	#
	0x0C,0x0C,0x3E,0x63,0x61,0x60,0x3E,0x03,0x03,0x43,0x63,0x3E,0x0C,0x0C,0x00,0x00,	//	$
	0x00,0x00,0x00,0x00,0x00,0x61,0x63,0x06,0x0C,0x18,0x33,0x63,0x00,0x00,0x00,0x00,	//	%
	0x00,0x00,0x00,0x1C,0x36,0x36,0x1C,0x3B,0x6E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,	//	&
	0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	'
	0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00,0x00,0x00,	//	(
	0x00,0x00,0x18,0x0C,0x0C,0x06,0x06,0x06,0x06,0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,	//	)
	0x00,0x00,0x00,0x00,0x42,0x66,0x3C,0xFF,0x3C,0x66,0x42,0x00,0x00,0x00,0x00,0x00,	//	*
	0x00,0x00,0x00,0x00,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,	//	+
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,	//	,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	-
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,	//	.
	0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x38,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,	//	/ (forward slash)
	0x00,0x00,0x3E,0x63,0x63,0x63,0x6B,0x6B,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,	//	0	0x30
	0x00,0x00,0x0C,0x1C,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3F,0x00,0x00,0x00,0x00,	//	1
	0x00,0x00,0x3E,0x63,0x03,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00,	//	2
	0x00,0x00,0x3E,0x63,0x03,0x03,0x1E,0x03,0x03,0x03,0x63,0x3E,0x00,0x00,0x00,0x00,	//	3
	0x00,0x00,0x06,0x0E,0x1E,0x36,0x66,0x66,0x7F,0x06,0x06,0x0F,0x00,0x00,0x00,0x00,	//	4
	0x00,0x00,0x7F,0x60,0x60,0x60,0x7E,0x03,0x03,0x63,0x73,0x3E,0x00,0x00,0x00,0x00,	//	5
	0x00,0x00,0x1C,0x30,0x60,0x60,0x7E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,	//	6
	0x00,0x00,0x7F,0x63,0x03,0x06,0x06,0x0C,0x0C,0x18,0x18,0x18,0x00,0x00,0x00,0x00,	//	7
	0x00,0x00,0x3E,0x63,0x63,0x63,0x3E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,	//	8
	0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x3F,0x03,0x03,0x06,0x3C,0x00,0x00,0x00,0x00,	//	9
	0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,	//	:
	0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,	//	;
	0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00,	//	<
	0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,	//	=
	0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00,	//	>
	0x00,0x00,0x3E,0x63,0x63,0x06,0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x00,0x00,0x00,0x00,	//	?
	0x00,0x00,0x3E,0x63,0x63,0x6F,0x6B,0x6B,0x6E,0x60,0x60,0x3E,0x00,0x00,0x00,0x00,	//	@	0x40
	0x00,0x00,0x08,0x1C,0x36,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x00,0x00,0x00,0x00,	//	A
	0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x33,0x33,0x33,0x33,0x7E,0x00,0x00,0x00,0x00,	//	B
	0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x60,0x60,0x61,0x33,0x1E,0x00,0x00,0x00,0x00,	//	C
	0x00,0x00,0x7C,0x36,0x33,0x33,0x33,0x33,0x33,0x33,0x36,0x7C,0x00,0x00,0x00,0x00,	//	D
	0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00,	//	E
	0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,	//	F
	0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x6F,0x63,0x63,0x37,0x1D,0x00,0x00,0x00,0x00,	//	G
	0x00,0x00,0x63,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00,	//	H
	0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,	//	I
	0x00,0x00,0x0F,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,	//	J
	0x00,0x00,0x73,0x33,0x36,0x36,0x3C,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00,	//	K
	0x00,0x00,0x78,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00,	//	L
	0x00,0x00,0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00,	//	M
	0x00,0x00,0x63,0x63,0x73,0x7B,0x7F,0x6F,0x67,0x63,0x63,0x63,0x00,0x00,0x00,0x00,	//	N
	0x00,0x00,0x1C,0x36,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x00,0x00,0x00,0x00,	//	O
	0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,	//	P
	0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x63,0x6B,0x6F,0x3E,0x06,0x07,0x00,0x00,	//	Q
	0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00,	//	R	0x50
	0x00,0x00,0x3E,0x63,0x63,0x30,0x1C,0x06,0x03,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,	//	S
	0x00,0x00,0xFF,0xDB,0x99,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,	//	T
	0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,	//	U
	0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x08,0x00,0x00,0x00,0x00,	//	V
	0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x36,0x00,0x00,0x00,0x00,	//	W
	0x00,0x00,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x3C,0x66,0xC3,0xC3,0x00,0x00,0x00,0x00,	//	X
	0x00,0x00,0xC3,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,	//	Y
	0x00,0x00,0x7F,0x63,0x43,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00,	//	Z
	0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00,0x00,0x00,	//	[
	0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x07,0x03,0x01,0x00,0x00,0x00,0x00,	//	\ (back slash)
	0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x00,0x00,	//	]
	0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	^
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,	//	_
	0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	`	0x60
	0x00,0x00,0x00,0x00,0x00,0x3C,0x46,0x06,0x3E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,	//	a
	0x00,0x00,0x70,0x30,0x30,0x3C,0x36,0x33,0x33,0x33,0x33,0x6E,0x00,0x00,0x00,0x00,	//	b
	0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x60,0x60,0x60,0x63,0x3E,0x00,0x00,0x00,0x00,	//	c
	0x00,0x00,0x0E,0x06,0x06,0x1E,0x36,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,	//	d
	0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x7E,0x60,0x63,0x3E,0x00,0x00,0x00,0x00,	//	e
	0x00,0x00,0x1C,0x36,0x32,0x30,0x7C,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,	//	f
	0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00,0x00,	//	g
	0x00,0x00,0x70,0x30,0x30,0x36,0x3B,0x33,0x33,0x33,0x33,0x73,0x00,0x00,0x00,0x00,	//	h
	0x00,0x00,0x0C,0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00,	//	i
	0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00,	//	j
	0x00,0x00,0x70,0x30,0x30,0x33,0x33,0x36,0x3C,0x36,0x33,0x73,0x00,0x00,0x00,0x00,	//	k
	0x00,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00,	//	l
	0x00,0x00,0x00,0x00,0x00,0x6E,0x7F,0x6B,0x6B,0x6B,0x6B,0x6B,0x00,0x00,0x00,0x00,	//	m
	0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x33,0x33,0x00,0x00,0x00,0x00,	//	n
	0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00,	//	o
	0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x3E,0x30,0x30,0x78,0x00,0x00,	//	p	0x70
	0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00,0x00,	//	q
	0x00,0x00,0x00,0x00,0x00,0x6E,0x3B,0x33,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00,	//	r
	0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x38,0x0E,0x03,0x63,0x3E,0x00,0x00,0x00,0x00,	//	s
	0x00,0x00,0x08,0x18,0x18,0x7E,0x18,0x18,0x18,0x18,0x1B,0x0E,0x00,0x00,0x00,0x00,	//	t
	0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00,	//	u
	0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x36,0x36,0x1C,0x1C,0x08,0x00,0x00,0x00,0x00,	//	v
	0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x00,0x00,0x00,0x00,	//	w
	0x00,0x00,0x00,0x00,0x00,0x63,0x36,0x1C,0x1C,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,	//	x
	0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x3F,0x03,0x06,0x3C,0x00,0x00,	//	y
	0x00,0x00,0x00,0x00,0x00,0x7F,0x66,0x0C,0x18,0x30,0x63,0x7F,0x00,0x00,0x00,0x00,	//	z
	0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00,	//	{
	0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,	//	|
	0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00,	//	}
	0x00,0x00,0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//	~
	0x00,0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};	//	DEL

#endif

void LCD_Init() {
	//Configure outputs
	LCD_SPIDDR |= _BV(LCD_SDA) | _BV(LCD_SCK); //Data line
	LCD_CTRDDR |= _BV(LCD_CS) | _BV(LCD_RESET); //Clock line

	//Backlight permanently on
	LCD_BLDDR |= _BV(LCD_BL);
	LCD_BLPORT |= _BV(LCD_BL);

	//Post power-up procedure
	CS0
	MOSI0
	CLK1

	RESET1
	RESET0
	_delay_ms(10);
	RESET1

	CLK1
	MOSI1
	CLK1
	_delay_ms(1);
	//Software Reset
	LCD_SendCommand(SWRESET);

	//Sleep Out
	LCD_SendCommand(SLEEP_OUT);

	//Booster ON
	LCD_SendCommand(BSTROFF);
	_delay_ms(5);

	//Normal display mode
	LCD_SendCommand(NORON);

	//Display inversion off
	LCD_SendCommand(INVOFF);

	//Data order
	LCD_SendCommand(DOR);

	//Memory data access control
	LCD_SendCommand(MADCTL);
	LCD_SendData(_BV(MAD_V)|_BV(MAD_MX)|_BV(MAD_MY));


#ifdef MODE16BPP
	LCD_SendCommand(COLMOD);
	LCD_SendData(COLMOD_16BPP);   //16-Bit per Pixel
#endif
#ifdef MODE12BPP
	LCD_SendCommand(COLMOD);
	LCD_SendData(COLMOD_12BPP);   //12-Bit per Pixel (default)
#endif
#ifdef MODE8BPP
	LCD_SendCommand(COLMOD);
	LCD_SendData(COLMOD_8BPP);
#endif

#ifdef MODE8BPP
	/* if 256-color mode, bytes represent RRRGGGBB; the following
	maps to 4-bit color for each value in range (0-7 R/G, 0-3 B) */
	LCD_SendCommand(RGBSET);      // 256-color position set
	LCD_SendData( 0x00);        // 000 RED
	LCD_SendData( 0x02);        // 001
	LCD_SendData( 0x04);        // 010
	LCD_SendData( 0x06);        // 011
	LCD_SendData( 0x08);        // 100
	LCD_SendData( 0x0a);        // 101
	LCD_SendData( 0x0c);        // 110
	LCD_SendData( 0x0f);        // 111
	LCD_SendData( 0x00);        // 000 GREEN
	LCD_SendData( 0x02);        // 001
	LCD_SendData( 0x04);        // 010
	LCD_SendData( 0x06);        // 011
	LCD_SendData( 0x08);        // 100
	LCD_SendData( 0x0a);        // 101
	LCD_SendData( 0x0c);        // 110
	LCD_SendData( 0x0f);        // 111
	LCD_SendData( 0x00);        //  00 BLUE
	LCD_SendData( 0x06);        //  01
	LCD_SendData( 0x09);        //  10
	LCD_SendData( 0x0f);        //  11
#endif

	//Set Constrast
	LCD_SendCommand(SETCON);
	LCD_SendData(63);

	//Column Adress Set
	LCD_SendCommand(CASET);
	LCD_SendData(0);
	LCD_SendData(131);

	//Page Adress Set
	LCD_SendCommand(PASET);
	LCD_SendData(0);
	LCD_SendData(131);

	//Memory Write
	LCD_SendCommand(RAMWR);

	//Clear screen
	for (int i=132*132; i>0; i--) {
		LCD_SetPixel(WHITE);
	}

	//Display On
	LCD_SendCommand(DISPON);
	LCD_SendCommand(NOP); //NOP

	CS1;

}

void LCD_Reset() {
	//Post power-up procedure
	CS0
	MOSI0
	CLK1

	RESET1
	RESET0
	_delay_ms(10);
	RESET1

	CLK1
	MOSI1
	CLK1
	_delay_ms(1);

	//Software Reset
	LCD_SendCommand(SWRESET);

	//Sleep Out
	LCD_SendCommand(SLEEP_OUT);

	//Booster ON
	LCD_SendCommand(BSTROFF);
	_delay_ms(5);

	//Normal display mode
	LCD_SendCommand(NORON);

	//Display inversion off
	LCD_SendCommand(INVOFF);

	//Data order
	LCD_SendCommand(DOR);

	//Memory data access control
	LCD_SendCommand(MADCTL);
	LCD_SendData(_BV(MAD_V)|_BV(MAD_MX)|_BV(MAD_MY));


#ifdef MODE16BPP
	LCD_SendCommand(COLMOD);
	LCD_SendData(COLMOD_16BPP);   //16-Bit per Pixel
#endif
#ifdef MODE12BPP
	LCD_SendCommand(COLMOD);
	LCD_SendData(COLMOD_12BPP);   //12-Bit per Pixel (default)
#endif
#ifdef MODE8BPP
	LCD_SendCommand(COLMOD);
	LCD_SendData(COLMOD_8BPP);
#endif

#ifdef MODE8BPP
	/* if 256-color mode, bytes represent RRRGGGBB; the following
	maps to 4-bit color for each value in range (0-7 R/G, 0-3 B) */
	LCD_SendCommand(RGBSET);      // 256-color position set
	LCD_SendData( 0x00);        // 000 RED
	LCD_SendData( 0x02);        // 001
	LCD_SendData( 0x04);        // 010
	LCD_SendData( 0x06);        // 011
	LCD_SendData( 0x08);        // 100
	LCD_SendData( 0x0a);        // 101
	LCD_SendData( 0x0c);        // 110
	LCD_SendData( 0x0f);        // 111
	LCD_SendData( 0x00);        // 000 GREEN
	LCD_SendData( 0x02);        // 001
	LCD_SendData( 0x04);        // 010
	LCD_SendData( 0x06);        // 011
	LCD_SendData( 0x08);        // 100
	LCD_SendData( 0x0a);        // 101
	LCD_SendData( 0x0c);        // 110
	LCD_SendData( 0x0f);        // 111
	LCD_SendData( 0x00);        //  00 BLUE
	LCD_SendData( 0x06);        //  01
	LCD_SendData( 0x09);        //  10
	LCD_SendData( 0x0f);        //  11
#endif

	//Set Constrast
	LCD_SendCommand(SETCON);
	LCD_SendData(63);

	/*	//Column Adress Set
		LCD_SendCommand(CASET);
		LCD_SendData(0);
		LCD_SendData(131);

		//Page Adress Set
		LCD_SendCommand(PASET);
		LCD_SendData(0);
		LCD_SendData(131);

		//Memory Write
		LCD_SendCommand(RAMWR);

		//Test-Picture
		for (int i=0;i<132*132;i++) {
			setPixel(WHITE);
		}
	*/
	//Display On
	LCD_SendCommand(DISPON);
	LCD_SendCommand(NOP); //NOP

	CS1;

}

void LCD_Blank() {
	CS0;

	//Column Adress Set
	LCD_SendCommand(CASET);
	LCD_SendData(0);
	LCD_SendData(131);

	//Page Adress Set
	LCD_SendCommand(PASET);
	LCD_SendData(0);
	LCD_SendData(131);

	//Memory Write
	LCD_SendCommand(RAMWR);
	int i;

	//fill with white
	for (i=132*132; i>0; i--) {
		LCD_SetPixel(WHITE);
	}
	LCD_SendCommand(NOP);
	CS1
}

//send Command
void LCD_SendCommand(uint8_t data) {
	while(!(SPSR & (1<<SPIF)))
		;// wait until send complete

	SPCR &= ~_BV(SPE); // | _BV(MSTR));// | _BV(SPR1) | _BV(SPR0));
    CLK0
	MOSI0 //MSB is 0 for sending command
	CLK1
	CLK0
	SPCR |= _BV(SPE); // | _BV(MSTR);// Enable Hardware SPI
	SPDR = data; // send data

}

//send Data
void LCD_SendData(uint8_t data) {
	while(!(SPSR & (1<<SPIF)))
		;// wait until send complete
	SPCR &= ~_BV(SPE);
	CLK0
	MOSI1 //MSB is 1 for sending data
	CLK1
	CLK0
	SPCR |= _BV(SPE); // Enable Hardware SPI
	SPDR = data; // send data
}

#ifdef MODE8BPP
void LCD_SetPixel8(uint8_t col) {
	LCD_SendData(col);
}
#endif

void LCD_SetPixel(uint16_t col) {
#ifdef MODE16BPP
	LCD_SendData((uint8_t)(col>>8));
	LCD_SendData((uint8_t)(col&0xff));
#endif
#ifdef MODE12BPP
	if (n==0) {
		s1=((uint8_t)(col>>4));
		s2=((uint8_t)((col&0x0f)<<4));
		n=1;
	} else {
		n=0;
		LCD_SendData(s1);
		LCD_SendData(s2|(col>>8));
		LCD_SendData((uint8_t)(col&0xff));
	}
#endif
#ifdef MODE8BPP
	LCD_SendData((uint8_t)col);
#endif
}

void LCD_Rectangle(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint16_t foreground) {
	CS0;
	// Column address set
	LCD_SendCommand(PASET);
	LCD_SendData(x);
	if (x+height-1<132) {
		LCD_SendData(x + height - 1);
	} else {
		LCD_SendData(131);
	}

	// Page address set
	LCD_SendCommand(CASET);
	LCD_SendData(y);
	if (y+width-1<132) {
		LCD_SendData(y + width - 1);
	} else {
		LCD_SendData(131);
	}

	LCD_SendCommand(RAMWR);
	for (uint16_t i=width*height; i>0; i--) {
		LCD_SetPixel(foreground);
	}
	LCD_SendCommand(NOP);
	CS1;
}

#ifdef LCDTEXT
void LCD_PutStr(char *s, uint8_t x, uint8_t y, uint8_t size, int foreground, int background) {
	if (x!=LCD_AUTOINCREMENT) cursorx=x;
	if (y!=LCD_AUTOINCREMENT) cursory=y;
	// loop until null-terminator is seen
	while (0 != *s) {
		// draw the character
		LCD_PutChar(*s++, cursorx, cursory, size, foreground, background);
	}
}
void LCD_PutStr_P(const char *s, uint8_t x, uint8_t y, uint8_t size, int foreground, int background) {
	register char c;

	if (x!=LCD_AUTOINCREMENT) cursorx=x;
	if (y!=LCD_AUTOINCREMENT) cursory=y;

	// loop until null-terminator is seen
	while (0 != (c = pgm_read_byte(s++))) {
		// draw the character
		if (c=='\r') continue;
		LCD_PutChar(c, cursorx, cursory, size, foreground, background);
	}
}

void LCD_SkipLine(uint8_t size) {
	if (size == 0) //SMALL
		cursory = cursory + 8;
	else if (size == 1) //MEDIUM
		cursory = cursory + 8;
	else
		cursory = cursory + 8;

}

void LCD_PutChar(char c, uint8_t x, uint8_t y, uint8_t size, int foreground, int background) {
	//autoincrement previous position if coordinates are not set
	if (x==LCD_AUTOINCREMENT) {
		x=cursorx;
	}
	if (y==LCD_AUTOINCREMENT) {
		y=cursory;
	}

	//extern const unsigned char FONT6x8[97][8];
	extern const unsigned char FONT8x8[97][8];
	extern const unsigned char FONT8x16[97][16];
	int i,j;
	unsigned int nCols, nRows, nBytes;
	unsigned char PixelRow, Mask;
	unsigned int Word0,Word1;
	unsigned char *pFont, *pChar;
	unsigned char *FontTable[] = {/*(unsigned char *)FONT6x8 ,*/ (unsigned char *)FONT8x8, (unsigned char *)FONT8x16 };


	// get pointer to the beginning of the selected font table
	pFont = (unsigned char *)FontTable[size];
	// get the nColumns, nRows and nBytes
	nCols = pgm_read_byte(pFont);
	nRows = pgm_read_byte(pFont + 1);
	nBytes = pgm_read_byte(pFont + 2);
	// get pointer to the last byte of the desired character
	pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;

	if (c == '\n') {
		y = 140; //out of screen
		goto MOVECURSOR;
	}

	CS0;

	// Row address set
	LCD_SendCommand(PASET);
	LCD_SendData(x);
	LCD_SendData(x + nRows - 1);
	// Column address set
	LCD_SendCommand(CASET);
	LCD_SendData(y);
	LCD_SendData(y + nCols - 1);
	// WRITE MEMORY
	LCD_SendCommand(RAMWR);
	// loop on each row, working backwards from the bottom to the top
	for (i = nRows - 1; i >= 0; i--) {
		// copy pixel row from font table and then decrement row
		PixelRow = pgm_read_byte(pChar--);
		// loop on each pixel in the row (left to right)
		// Note: we do two pixels each loop
		Mask = 0x80;
		for (j = 0; j < nCols; j+=2) {
			// if pixel bit set, use foreground color; else use the background color
			// now get the pixel color for two successive pixels
			//if ((PixelRow & Mask) == 0) { setPixel(0,0,0); } else { setPixel(255,255,255); }
			if ((PixelRow & Mask) == 0)
				Word0 = background;
			//setPixel(0,0,0);
			else
				Word0 = foreground;
			//setPixel(255,255,255);
			Mask = Mask >> 1;
			if ((PixelRow & Mask) == 0)
				Word1 = background;
			//setPixel(0,0,0);
			else
				Word1 = foreground;
			//setPixel(255,255,255);
			Mask = Mask >> 1;
#ifdef MODE12BPP
			// use this information to output three data bytes
			LCD_SendData((Word0 >> 4) & 0xFF);
			LCD_SendData(((Word0 & 0xF) << 4) | ((Word1 >> 8) & 0xF));
			LCD_SendData(Word1 & 0xFF);
#endif
#ifdef MODE16BPP
			LCD_SendData((uint8_t)(Word0>>8));
			LCD_SendData((uint8_t)(Word1&0xff));
#endif
		}
	}
	// terminate the Write Memory command
	LCD_SendCommand(NOP);
	CS1;

MOVECURSOR:
	// increment the y position
	if (size == 0) //SMALL
		y = y + 8;
	else if (size == 1) //MEDIUM
		y = y + 8;
	else
		y = y + 8;

	// line wrap if y exceeds 131
	if (y > 131-8) {
		y=0;
		x-=8;
	};
	if ((x<0)||(x>240)) {
		x=131-8;
	}
	cursory=y;
	cursorx=x;
}

void LCD_PutDecimal(uint32_t value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background) {
	char str[11];

	/* convert unsigned integer into string */
	ultoa(value, str, 10);

	LCD_PutStr(str, x, y, size, foreground, background);
}

void LCD_PutDecimalFixedDigits(uint32_t value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background, uint8_t digits) {
	char str[11];

	/* convert unsigned integer into string */
	ultoa(value, str, 10);
	uint8_t n=0;
	//count number of digits
	while (0 != str[++n])
		;
	//fill preceding digits
	if (digits>n) {
		LCD_PutChar('0',x,y,size,foreground,background);
		for (n=digits-n; n>1; n--) {
			LCD_PutChar('0',LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, size,foreground,background);
		}
		LCD_PutStr(str, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, size, foreground, background);
	} else {
		LCD_PutStr(str, x, y, size, foreground, background);
	}
}
void LCD_PutDecimalSigned(int32_t value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background) {
	char str[11];

	/* convert signed integer into string */
	ltoa(value, str, 10);

	LCD_PutStr(str, x, y, size, foreground, background);
}

void LCD_PutDouble(double value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background) {

	char str[12];

	/* convert signed integer into string */
	dtostrf(value, -3, 2, str);

	LCD_PutStr(str, x, y, size, foreground, background);
}

#endif //LCDTEXT

