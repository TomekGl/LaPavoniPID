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

//Start menu at row:
#define X_POS 62

//Max menu rows
#define MENU_ROWS 4

#define NOT_SELECTED 255

typedef void (*FuncPtr)(uint8_t keys, uint8_t * value );

//function pointer
FuncPtr FPtr;

/// menu strings
const char menu_entry_0[] __attribute__ ((progmem)) = "Ustawienia PID" ;
const char menu_entry_0_0[] __attribute__ ((progmem)) = "SV: " ;
const char menu_entry_0_1[] __attribute__ ((progmem)) = "K_p: " ;
const char menu_entry_0_2[] __attribute__ ((progmem)) = "T_d: " ;
const char menu_entry_0_3[] __attribute__ ((progmem)) = "T_i: " ;
const char menu_entry_0_4[] __attribute__ ((progmem)) = "Integr: " ;

const char menu_entry_1[] __attribute__ ((progmem)) = "I/O " ;
const char menu_entry_1_0[] __attribute__ ((progmem)) = "Buzzer: " ;
const char menu_entry_1_1[] __attribute__ ((progmem)) = "Output1: " ;
const char menu_entry_1_2[] __attribute__ ((progmem)) = "Output2: " ;
const char menu_entry_1_3[] __attribute__ ((progmem)) = "Output3: " ;
const char menu_entry_1_4[] __attribute__ ((progmem)) = "Input: " ;
const char menu_entry_2[] __attribute__ ((progmem)) = "Status " ;
const char menu_entry_2_0[] __attribute__ ((progmem)) = "Zapisz parametry " ;

const char *menu_first_level[] __attribute__ ((progmem)) = {
		menu_entry_0,
		menu_entry_1,
		menu_entry_2
};

const char  *menu_second_level[] __attribute__ ((progmem)) =  {
		menu_entry_0_0, menu_entry_0_1, menu_entry_0_2, menu_entry_0_3, menu_entry_0_4,
		menu_entry_1_0, menu_entry_1_1,menu_entry_1_2, menu_entry_1_3,menu_entry_1_4,
		menu_entry_2_0
};

const FuncPtr functions[] __attribute__ ((progmem)) = {
		(void*)&setSignedInteger16, (void*)&setSignedInteger16, (void*)&setSignedInteger16, (void*)&setSignedInteger16, (void*)&setSignedInteger16,
		(void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setInteger,
		(void*)&callAfterConfirm
};

const uint8_t submenu_index[] = { 5,5,1 };

uint8_t zmienna = 0, zmienna2 = 25;
void * variables[] = {
		(void *)&(controller.SV), (void *)&(controller.k_r), (void *)&(controller.T_d), (void *)&(controller.T_i),(void *)&(controller.integral),
		(void *)&tmp_buzz, (void *)&tmp_out1,(void *)&tmp_out2, (void *)&tmp_out3,(void *)&tmp_in,
		(void*)&PID_SaveSettings
};


struct Tmenu_position menu_position;

void callAfterConfirm(uint8_t keys, uint8_t * value) {
	USART_Puts("conf:");
	USART_TransmitDecimal(keys);
	switch (keys) {
	case KEY_UP:
		PID_SaveSettings();
		LCD_PutStr("Zapisano!", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, RED, (keys==0)?BLACK:BLUE );
		break;
	case KEY_DOWN:
	case KEY_RIGHT: //option selected
		LCD_PutStr("Wcisnij \0x5f", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, RED, (keys==0)?BLACK:BLUE);
		break;
	default:
		break;
	}
	return;

}

void setInteger(uint8_t keys, uint8_t * value) {
	switch (keys) {
	case KEY_UP:
		(*value)++;
		break;
	case KEY_DOWN:
		(*value)--;
		break;
	default:
		break;

	}
	LCD_PutDecimal(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, RED, (keys==0)?BLACK:BLUE);
	return;
}

void getIntegerReadOnly(uint8_t keys, uint8_t * value) {
	LCD_PutDecimal(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, RED, (keys==0)?BLACK:BLUE);
	return;
}

void setSignedInteger16(uint8_t keys, int16_t * value) {
	switch (keys) {
	case KEY_UP:
		(*value)+=20;
		break;
	case KEY_DOWN:
		(*value)-=20;
		break;
	default:
		break;

	}
	LCD_PutDecimalSigned(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, RED, (keys==0)?BLACK:BLUE);
	return;
}

void setBoolean(uint8_t keys, uint8_t * value) {
	switch (keys) {
	case KEY_UP:
	case KEY_DOWN:
		*value = ~(*value);
		break;
	default:
		break;
	}
	LCD_PutStr((*value==0)?"Off":"On ", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, BLACK);
	return;
}

uint8_t resolve_index(uint8_t first_level, uint8_t second_level) {
	uint8_t index = 0;
	for (uint8_t i = 0; i<first_level; i++) {
		index += submenu_index[i];
	}
	index += second_level-1;
	return index;
}

void menu_Init(void) {
	menu_position.first_level = 0;
	menu_position.second_level = NOT_SELECTED;
	menu_position.entry_selected = 0;
}

void MenuProcess(TKey key) {
	switch (key) {
	case KEY_DOWN:
		if (NOT_SELECTED == menu_position.second_level) {
			if (menu_position.first_level<(sizeof(menu_first_level)/sizeof(menu_first_level[0])-1)) {
				menu_position.first_level++;
			} else {
				menu_position.first_level = 0;
			}
		} else if (0 == menu_position.entry_selected) {
			if (menu_position.second_level<submenu_index[menu_position.first_level]) {
				menu_position.second_level++;
			} else {
				menu_position.second_level = 1;
			}
		}
		break;
	case KEY_UP:
		if (NOT_SELECTED == menu_position.second_level) {
			if (menu_position.first_level>0) {
				menu_position.first_level--;
			} else {
				menu_position.first_level = sizeof(menu_first_level)/sizeof(menu_first_level[0])-1;
			}
		} else if (0 == menu_position.entry_selected) {
			if (menu_position.second_level>1) {
				menu_position.second_level--;
			} else {
				menu_position.second_level = submenu_index[menu_position.first_level];
			}
		}
		break;
	case KEY_RIGHT:
		if (NOT_SELECTED == menu_position.second_level) {
			menu_position.second_level = 1;
		} else {
			menu_position.entry_selected = 1;
		}
		break;
	case KEY_LEFT:
		if (NOT_SELECTED != menu_position.second_level && 0 == menu_position.entry_selected) {
			menu_position.second_level = NOT_SELECTED;
		} else {
			menu_position.entry_selected = 0;
		}
		break;
	case 0:
	default:
		break;

	}

	//redraw menu
	LCD_Rectangle(0,X_POS-8*MENU_ROWS,132,48,BLACK);
	uint8_t n = 0;
	if (NOT_SELECTED == menu_position.second_level) {
		//dispay first level menulist
		for (uint8_t i = 0; i<(sizeof(menu_first_level)/sizeof(menu_first_level[0])); i++) {
			LCD_PutStr_P((char *)pgm_read_word(&(menu_first_level[i])), X_POS-i*8, 2, 0, WHITE, (i==menu_position.first_level)?BLUE:BLACK);
		}

	} else {
		// submenu
		LCD_PutStr_P((char *)pgm_read_word(&(menu_first_level[menu_position.first_level])), X_POS, 2, 0, ORANGE, BLACK);
//		n = submenu_index[menu_position.first_level]);
//		if (n > MENU_ROWS) {
//			n = MENU_ROWS;
//		}
		for (uint8_t i = 0; i<submenu_index[menu_position.first_level]; i++) {
			//LCD_PutDecimal(resolve_index(menu_position.first_level, i+1), 92-i*8,0,0, RED,BLACK);
			LCD_PutStr_P((char *)pgm_read_word(&(menu_second_level[resolve_index(menu_position.first_level, i+1)])), X_POS-8-i*8, 10, 0, WHITE, (i==menu_position.second_level-1)?BLUE:BLACK);

			FPtr=(FuncPtr)pgm_read_word(&functions[resolve_index(menu_position.first_level, i+1)]);
			if (FPtr!=0) {
				FPtr((menu_position.entry_selected && menu_position.second_level==i+1)?key:0, variables[resolve_index(menu_position.first_level, i+1)] );
			}

		}

	}
}
