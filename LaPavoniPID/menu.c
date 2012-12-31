/**
 @file menu.c
 @defgroup menu Multilevel menu library
 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 18-03-2012
*/

#include "includes.h"

/**@{*/

/// meta index of currently selected item, if none
#define NOT_SELECTED 255

/// definition of pointer to function with 2 params
typedef void (*FuncPtr)(uint8_t keys, const void * value );

///pointer to callback function called on every modification of item
FuncPtr CallbackFunction;

/********** MENU CONTENTS CONFIG **********/

/**
 * \defgroup menu_items Menu items definition
 * @{
 */

// menu strings
const char menu_entry_0[] __attribute__ ((progmem)) = "PID Settings" ;
const char menu_entry_0_0[] __attribute__ ((progmem)) = "SP: " ;
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
const char menu_entry_2_0[] __attribute__ ((progmem)) = "TIM0: " ;
const char menu_entry_2_1[] __attribute__ ((progmem)) = "Save settings " ;
const char menu_entry_2_2[] __attribute__ ((progmem)) = "Backlight " ;
const char menu_entry_2_3[] __attribute__ ((progmem)) = "T1: " ;
const char menu_entry_2_4[] __attribute__ ((progmem)) = "T2: " ;

const char menu_entry_3[] __attribute__ ((progmem)) = "Pre-infusion " ;
const char menu_entry_3_0[] __attribute__ ((progmem)) = "Time x0.1:" ;
const char menu_entry_3_1[] __attribute__ ((progmem)) = "Duty cycle:";
const char menu_entry_3_2[] __attribute__ ((progmem)) = "Valve off dly:";


const char *menu_first_level[] __attribute__ ((progmem)) = {
	menu_entry_0,
	menu_entry_1,
	menu_entry_2,
	menu_entry_3
};

const char  *menu_second_level[] __attribute__ ((progmem)) =  {
	menu_entry_0_0, menu_entry_0_1, menu_entry_0_2, menu_entry_0_3, menu_entry_0_4,
	menu_entry_1_0, menu_entry_1_1, menu_entry_1_2, menu_entry_1_3, menu_entry_1_4,
	menu_entry_2_0, menu_entry_2_1, menu_entry_2_2, menu_entry_2_3, menu_entry_2_4,
	menu_entry_3_0, menu_entry_3_1, menu_entry_3_2
};

///array of pointers on callback functions
const FuncPtr functions[] __attribute__ ((progmem)) = {
	(void*)&setDouble, (void*)&setDouble, (void*)&setDouble, (void*)&setDouble, (void*)&setDouble,
	(void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setInteger,
	(void*)&setInteger, (void*)&callAfterConfirm, (void*)&setInteger, (void*)&setDouble, (void*)&setDouble,
	(void*)&setInteger, (void*)&setInteger, (void*)&setInteger
};

///array of pointers on variables connected with item
const void * variables[] = {
	(void *)&(controller_param.SV), (void *)&(controller_param.k_r), (void *)&(controller_param.T_d), (void *)&(controller_param.T_i),(void *)&(controller.y),
	(void *)&(controller_param.buzzer_enabled), (void *)&tmp_out1,(void *)&tmp_out2, (void *)&tmp_out3, (void *)&output,
	(void *)&timer0, (void*)&PID_SaveSettings, (void *)&OCR2, 	(void *)&floatpv, (void *)&floattest,
	(void *)&(controller_param.preinfusion_time), (void*)&(controller_param.preinfusion_duty_cycle), (void *)&(controller_param.preinfusion_valve_off_delay)
};

///map of menu structure
const uint8_t submenu_entries_count[] = { 5,5,5,3 };

/**@}*/

/********** END OF MENU CONTENTS CONFIG **********/

struct Tmenu_position menu_position;

void callAfterConfirm(uint8_t keys, uint8_t * value) {
	switch (keys) {
	case KEY_UP:
		PID_SaveSettings();
		LCD_PutStr("Saved!", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR );
		break;
	case KEY_DOWN:
	case KEY_RIGHT: //option selected
		LCD_PutStr("Press \0x5f", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR);
		break;
	default:
		break;
	}
	return;

}

void setInteger(uint8_t keys, uint8_t * value) {
	if (keys & KEY_UP) {
		if (keys & REPEATED_FLAG) {
			(*value)+=10;
		} else {
			(*value)+=1;
		}
	}
	if (keys & KEY_DOWN) {
		if (keys & REPEATED_FLAG) {
			(*value)-=10;
		} else {
			(*value)-=1;
		}
	}
	LCD_PutDecimal(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR);
	return;
}

void getIntegerReadOnly(uint8_t keys, uint8_t * value) {
	LCD_PutDecimal(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR);
	return;
}

void setDouble(uint8_t keys, double * value) {
	if (keys & KEY_UP) {
		if (keys & REPEATED_2X_FLAG) {
			(*value)+=10;
		} else if (keys & REPEATED_FLAG) {
			(*value)+=1;
		} else {
			(*value)+=0.1;
		}
	}
	if (keys & KEY_DOWN) {
		if (keys & REPEATED_2X_FLAG) {
			(*value)-=10;
		} else if (keys & REPEATED_FLAG) {
			(*value)-=1;
		} else {
			(*value)-=0.1;
		}
	}
	LCD_PutDouble(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR);
	return;
}

void setSignedInteger16(uint8_t keys, int16_t * value) {
	if (keys & KEY_UP) {
		if (keys & REPEATED_2X_FLAG) {
			(*value)+=100;
		} else if (keys & REPEATED_FLAG) {
			(*value)+=10;
		} else {
			(*value)+=1;
		}
	}
	if (keys & KEY_DOWN) {
		if (keys & REPEATED_2X_FLAG) {
			(*value)-=100;
		} else if (keys & REPEATED_FLAG) {
			(*value)-=10;
		} else {
			(*value)-=1;
		}
	}
	LCD_PutDecimalSigned(*value, LCD_AUTOINCREMENT,LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR);
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
	LCD_PutStr((*value==0)?"Off":"On ", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, MENUCOLOR_VALUE, (keys==0)?MENUCOLOR_BACKGROUND:MENUCOLOR_CURSOR);
	return;
}

uint8_t Menu_ObjectIndex(uint8_t first_level, uint8_t second_level) {
	uint8_t index = 0;
	for (uint8_t i = 0; i<first_level; i++) {
		index += submenu_entries_count[i];
	}
	index += second_level-1;
	return index;
}

void Menu_Init(void) {
	menu_position.first_level = 0;
	menu_position.second_level = NOT_SELECTED;
	menu_position.entry_selected = 0;
}

void Menu_Process(TKey key) {
	if (key & KEY_DOWN) {
		if (NOT_SELECTED == menu_position.second_level) { //MAIN MENU
			if (menu_position.first_level<(sizeof(menu_first_level)/sizeof(menu_first_level[0])-1)) {
				menu_position.first_level++;
			} else {
				menu_position.first_level = 0;
			}
		} else if (0 == menu_position.entry_selected) {
			if (menu_position.second_level<submenu_entries_count[menu_position.first_level]) {
				menu_position.second_level++;
			} else {
				menu_position.second_level = 1;
			}
		}
	}
	if (key & KEY_UP) {
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
				menu_position.second_level = submenu_entries_count[menu_position.first_level];
			}
		}
	}
	if (key & KEY_RIGHT) {
		if (NOT_SELECTED == menu_position.second_level) {
			menu_position.second_level = 1;
		} else {
			menu_position.entry_selected = 1;
		}
	}
	if (key & KEY_LEFT) {
		if (NOT_SELECTED != menu_position.second_level && 0 == menu_position.entry_selected) {
			menu_position.second_level = NOT_SELECTED;
		} else {
			menu_position.entry_selected = 0;
		}
	}

	//redraw menu
	LCD_Rectangle(MENU_X_POS-16-8*MENU_ROWS,0,56,132,MENUCOLOR_BACKGROUND);

	if (NOT_SELECTED == menu_position.second_level) {
		//dispay first level menulist
		for (uint8_t i = 0; i<(sizeof(menu_first_level)/sizeof(menu_first_level[0])); i++) {
			LCD_PutStr_P((char *)pgm_read_word(&(menu_first_level[i])), MENU_X_POS-i*8, 2, 0, MENUCOLOR_TEXT, (i==menu_position.first_level)?MENUCOLOR_CURSOR:MENUCOLOR_BACKGROUND);
		}

	} else {
		// submenu
		LCD_PutStr_P((char *)pgm_read_word(&(menu_first_level[menu_position.first_level])), MENU_X_POS, 2, 0, MENUCOLOR_HEADER, MENUCOLOR_BACKGROUND);
//		n = submenu_entries_count[menu_position.first_level]);
//		if (n > MENU_ROWS) {
//			n = MENU_ROWS;
//		}
		for (uint8_t i = 0; i<submenu_entries_count[menu_position.first_level]; i++) {
			LCD_PutStr_P((char *)pgm_read_word(&(menu_second_level[Menu_ObjectIndex(menu_position.first_level, i+1)])), MENU_X_POS-8-i*8, 10, 0, MENUCOLOR_TEXT, (i==menu_position.second_level-1)?MENUCOLOR_CURSOR:MENUCOLOR_BACKGROUND);

			//acquire pointer to callback function
			CallbackFunction=(FuncPtr)pgm_read_word(&functions[Menu_ObjectIndex(menu_position.first_level, i+1)]);
			//execute if not null
			if (CallbackFunction!=0) {
				CallbackFunction((menu_position.entry_selected && menu_position.second_level==i+1)?key:0,
						variables[Menu_ObjectIndex(menu_position.first_level, i+1)] );
			}

		}

	}
}
