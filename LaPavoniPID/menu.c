//*****************************************************************************
//
// File Name	: 'menu.c'
// Title		: Two level menu demo
// Author		: Scienceprog.com - Copyright (C) 2007
// Created		: 2007-03-23
// Revised		: 2007-03-23
// Version		: 1.0
// Target MCU	: Atmel AVR series
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "includes.h"

#define UP 0
#define DOWN 3
#define LEFT 1
#define RIGHT 2
#define LED_R 5
#define LED_G 4
#define LED_B 3
#define BT_PORT PORTC
#define BT_DDR  DDRC
#define BT_PIN  PINC
#define LED_PORT PORTB
#define LED_DDR DDRB

typedef void (*FuncPtr)(void);
//function pointer
FuncPtr FPtr;
//Structure describes current menu and submenu state
struct Menu_State {
	 uint8_t menuNo;//1,2,3,4
	 uint8_t subMenuNo;//1,2,3
} MN;
//flag required for exiting from functions loops
uint8_t Flag=1;
//Menu Strings in flash
const uint8_t MN000[] PROGMEM="Menu Demo\0";
//menu 1
const uint8_t MN100[] PROGMEM="<<One Led>>\0";
//menu 1 submenus
const uint8_t MN101[] PROGMEM="R ON\0";
const uint8_t MN102[] PROGMEM="G ON\0";
const uint8_t MN103[] PROGMEM="B ON\0";
//menu 2
const uint8_t MN200[] PROGMEM="<<All ON/OFF>>\0";
//Submenus of menu 2
const uint8_t MN201[] PROGMEM="All ON\0";
const uint8_t MN202[] PROGMEM="All OFF\0";
//menu 3
const uint8_t MN300[] PROGMEM="<<Auto scroll>>\0";
//Submenus of menu 3
const uint8_t MN301[] PROGMEM="Left\0";
const uint8_t MN302[] PROGMEM="Right\0";
//menu 4
const uint8_t MN400[] PROGMEM="<<Blink All>>\0";
//submenus of menu 4
const uint8_t MN401[] PROGMEM="Blink Fast\0";
const uint8_t MN402[] PROGMEM="Blink Slow\0";
//more menus and submenus can be added.
//Arrays of pointers to menu strings stored in flash
const uint8_t *MENU[] ={
		MN100,	//menu 1 string
		MN200,	//menu 2 string
		MN300,	//menu 3 string
		MN400	//menu 4 string
		};
const uint8_t *SUBMENU[] ={
		MN101, MN102, MN103,	//submenus of menu 1
		MN201, MN202,			//submenus of menu 2
		MN301, MN302,			//submenus of menu 3
		MN401, MN402			//submenus of menu 4
		};

//Menu structure
//[0] -Number of level 0 menu items
//[1]...[n] number of second level menu items
//Eg. MSTR2[1] shows that menu item 1 has 3 submenus
const uint8_t MSTR2[] PROGMEM ={
	4,	//number of menu items
	3,	//Number of submenu items of menu item 1
	2,	//of menu item 2
	2,	//of menu item 3
	2	//of menu item 4
	}; 
//Function prototypes
//Initial menu
void menu_Init(void);
//Functions for each menu item
void func101(void);
void func102(void);
void func103(void);
void func201(void);
void func202(void);
void func301(void);
void func302(void);
void func401(void);
void func402(void);


//Arrray of function pointers in Flash
const FuncPtr FuncPtrTable[] PROGMEM=
    { 	func101, func102, func103,	//functions for submenus of menu 1
		func201, func202, 			//functions for submenus of menu 2
		func301, func302, 			//functions for submenus of menu 3
		func401, func402			//functions for submenus of menu 4
		};

//void CopyStringtoLCD(const uint8_t*, uint8_t, uint8_t) {
void CopyStringtoLCD(const uint8_t *FlashLoc, uint8_t x, uint8_t y) {
	LCD_PutStr_P(FlashLoc,60-(y*10),x*8, 0 , WHITE, BLACK);
}
void LCDclr() {
	LCD_Rectangle(40,0,60,16*8, BLACK);
}

//SubMenu and Function table pointer update
uint8_t MFIndex(uint8_t, uint8_t);
//Timer0 overflow interrupt service routine	
void MenuProcess(void)
{
	//if button UP pressed
	//if (bit_is_clear(BT_PIN, UP))
	if (bit_is_clear(SW1_PIN, SW1))
	{
		if (MN.menuNo<pgm_read_byte(&MSTR2[0]))
		{ 
			MN.menuNo++;
			MN.subMenuNo=1;
		}
		else
		{
			MN.menuNo=1;
		}
		LCDclr();
		//Display menu item
		CopyStringtoLCD(MENU[MN.menuNo-1], 0, 0 );
		//Display submenu item
		CopyStringtoLCD(SUBMENU[MFIndex(MN.menuNo, MN.subMenuNo)], 0, 1 );
		//Assign function to function pointer
		FPtr=(FuncPtr)pgm_read_word(&FuncPtrTable[MFIndex(MN.menuNo, MN.subMenuNo)]);
		//set Flag to 0 menas menu have changed 
		Flag=0;
		//wait for button release
		//loop_until_bit_is_set(BT_PIN, UP);
	}
	//if Button DOWN pressed
	//if (bit_is_clear(BT_PIN, DOWN))
	if (bit_is_clear(SW2_PIN, SW2))
	{
		if (MN.menuNo==1)
		{ 
			MN.menuNo=pgm_read_byte(&MSTR2[0]);
			MN.subMenuNo=1;
		}
		else
		{
			MN.menuNo--;
		}
	LCDclr();
	//
	CopyStringtoLCD(MENU[MN.menuNo-1], 0, 0 );
	CopyStringtoLCD(SUBMENU[MFIndex(MN.menuNo, MN.subMenuNo)], 0, 1 );
	FPtr=(FuncPtr)pgm_read_word(&FuncPtrTable[MFIndex(MN.menuNo, MN.subMenuNo)]);
	Flag=0;
	//loop_until_bit_is_set(BT_PIN, DOWN);
	}
	//If Button RIGHT pressed
	//if (bit_is_clear(BT_PIN, RIGHT))
	if (bit_is_clear(SW4_PIN, SW4))
	{
		if (MN.subMenuNo<pgm_read_byte(&MSTR2[MN.menuNo]))
		{ 
			MN.subMenuNo++;
		}
		else
		{
			MN.subMenuNo=1;
		}
	LCDclr();
	CopyStringtoLCD(MENU[MN.menuNo-1], 0, 0 );
	CopyStringtoLCD(SUBMENU[MFIndex(MN.menuNo, MN.subMenuNo)], 0, 1 );
	FPtr=(FuncPtr)pgm_read_word(&FuncPtrTable[MFIndex(MN.menuNo, MN.subMenuNo)]);	
	Flag=0;
	//loop_until_bit_is_set(BT_PIN, RIGHT);
	}
	//If button LEFT pressed
	//if (bit_is_clear(BT_PIN, LEFT))
	if (bit_is_clear(SW3_PIN, SW3))
	{
		if (MN.subMenuNo==1)
		{ 
			MN.subMenuNo=pgm_read_byte(&MSTR2[MN.menuNo]);
		}
		else
		{
			MN.subMenuNo--;
		}
	LCDclr();
	CopyStringtoLCD(MENU[MN.menuNo-1], 0, 0 );
	CopyStringtoLCD(SUBMENU[MFIndex(MN.menuNo, MN.subMenuNo)], 0, 1 );
	FPtr=(FuncPtr)pgm_read_word(&FuncPtrTable[MFIndex(MN.menuNo, MN.subMenuNo)]);	
	Flag=0;
	//loop_until_bit_is_set(BT_PIN, LEFT);
	}

	FPtr();
}

int dupa(void)
{
	//Welcome demo message
	CopyStringtoLCD(MN000, 0, 0 );
	//delay1s();
	//Init Buttons and LEDs ports
	//ports_Init();
	LCDclr();
	//Initial menu and initial function
	menu_Init();
	//init timer0 for key readings using overflow interrups
	//Timer0_init();
	while(1)
	{
	//set flag to 1
	//when button menu changes flag sets to 0
	Flag=1;
	//execute function that is pointed by FPtr
	FPtr();
	}
	return 0;
}

void menu_Init(void)
{
	CopyStringtoLCD(MN000, 0, 0 );
	_delay_ms(800);
 	MN.menuNo=1;
	MN.subMenuNo=1;
	CopyStringtoLCD(MENU[(MN.menuNo-1)], 0, 0 );
	CopyStringtoLCD(SUBMENU[(MN.subMenuNo-1)], 0, 1 );
	FPtr=(FuncPtr)pgm_read_word(&FuncPtrTable[0]);
}

uint8_t MFIndex(uint8_t mn, uint8_t sb)
{
	uint8_t p=0;//points to menu in table of function pointer 
	for(uint8_t i=0; i<(mn-1); i++)
	{
		p=p+pgm_read_byte(&MSTR2[i+1]);
	}
	p=p+sb-1;
	return p;
}


void func101(void)
{
	USART_Puts("FUNC101");
}
void func102(void)
{
	USART_Puts("FUNC102");
}
void func103(void)
{
	USART_Puts("FUNC103");
}
void func201(void)
{
	USART_Puts("FUNC201");
}
void func202(void)
{
	USART_Puts("FUNC202");
}
void func301(void)
{
	USART_Puts("FUNC301");
}
void func302(void)
{

	USART_Puts("FUNC302");
}
void func401(void)
{
	USART_Puts("FUNC401");
}
void func402(void)
{
	USART_Puts("FUNC402");
}
