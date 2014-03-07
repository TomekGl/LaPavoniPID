/**
 @file menu.c
 @defgroup menu Multilevel menu library
 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 18-03-2012

 @note Due to used progmem handling via pointer to const char, this code compiles only with gcc version 4.5.3 (GCC) or older!
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

#ifndef __FLASH
  #error "What am I doing using such an out-dated compiler?"

#endif
// menu strings
const __flash  uint8_t menu_entry_0[]  = "PID Settings" ;
const __flash  uint8_t menu_entry_0_0[] = "SP[oC]: " ;
const __flash  uint8_t menu_entry_0_1[] = "Gain[]: " ;
const __flash  uint8_t menu_entry_0_2[] = "T_d[/s]: " ;
const __flash  uint8_t menu_entry_0_3[] = "T_i[s]: " ;
const __flash  uint8_t menu_entry_0_4[] = "Windup: " ;
/*const __flash  uint8_t menu_entry_0_5[] = "Deadband: " ;*/

const __flash  uint8_t menu_entry_1[] = "I/O " ;
const __flash  uint8_t menu_entry_1_0[] = "Buzzer: " ;
const __flash  uint8_t menu_entry_1_1[] = "Output1: " ;
const __flash  uint8_t menu_entry_1_2[] = "Output2: " ;
const __flash  uint8_t menu_entry_1_3[] = "Output3: " ;
const __flash  uint8_t menu_entry_1_4[] = "Input: " ;

const __flash  uint8_t menu_entry_2[] = "Status " ;
const __flash  uint8_t menu_entry_2_0[] = "UART debug: " ;
const __flash  uint8_t menu_entry_2_1[] = "Save settings " ;
const __flash  uint8_t menu_entry_2_2[] = "Backlight " ;
const __flash  uint8_t menu_entry_2_3[] = "Alpha: " ;

const __flash  uint8_t menu_entry_3[] = "Pre-infusion " ;
const __flash  uint8_t menu_entry_3_0[] = "Time x0.1:" ;
const __flash  uint8_t menu_entry_3_1[] = "Duty cycle:";
const __flash  uint8_t menu_entry_3_2[] = "Valve off dly:";

const __flash  uint8_t menu_entry_4[] = "Flow correction " ;
const __flash  uint8_t menu_entry_4_0[] = "k[imp/ml]:" ;
const __flash  uint8_t menu_entry_4_1[] = "P_heater[W]:";

const __flash uint8_t* const __flash menu_first_level[] = {
		menu_entry_0,
		menu_entry_1,
		menu_entry_2,
		menu_entry_3,
		menu_entry_4

};

const __flash uint8_t * const __flash menu_second_level[] =  {
	menu_entry_0_0, menu_entry_0_1, menu_entry_0_2, menu_entry_0_3, menu_entry_0_4,
	menu_entry_1_0, menu_entry_1_1, menu_entry_1_2, menu_entry_1_3, menu_entry_1_4,
	menu_entry_2_0, menu_entry_2_1, menu_entry_2_2, menu_entry_2_3,
	menu_entry_3_0, menu_entry_3_1, menu_entry_3_2,
	menu_entry_4_0, menu_entry_4_1

};

///array of pointers on callback functions
const FuncPtr functions[]  = {
	(void*)&setDouble, (void*)&setDouble, (void*)&setDouble, (void*)&setDouble, (void*)&setDouble, /*(void*)&setDouble,*/
	(void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setInteger,
	(void*)&setBoolean, (void*)&callAfterConfirm, (void*)&setInteger, (void*)&setDouble,
	(void*)&setInteger, (void*)&setInteger, (void*)&setInteger,
	(void*)&setDouble, (void*)&setDouble
};

///array of pointers on variables connected with item
const void * variables[] = {
	(void *)&(controller_param.SV), (void *)&(controller_param.k_r), (void *)&(controller_param.T_d), (void *)&(controller_param.T_i), (void *)&(controller_param.windup), /*(void *)&(controller_param.dead),*/
	(void *)&(controller_param.buzzer_enabled), (void *)&tmp_out1,(void *)&tmp_out2, (void *)&tmp_out3, (void *)&output,
	(void *)&(controller_param.serial_debug), (void*)&PID_SaveSettings, (void *)&(controller_param.lcd_brightness), 	(void *)&(controller_param.alpha),
	(void *)&(controller_param.preinfusion_time), (void*)&(controller_param.preinfusion_duty_cycle), (void *)&(controller_param.preinfusion_valve_off_delay),
	(void *)&(controller_param.flowratefactor), (void *)&(controller_param.heater_power)
};

///map of menu structure
const uint8_t submenu_entries_count[] = { 5,5,4,3,2 };

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

uint8_t Menu_isNotSelected() {
	return (NOT_SELECTED == menu_position.second_level);
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
		*value = 1;
		break;
	case KEY_DOWN:
		*value = 0;
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
	} else if (key & KEY_UP) {
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
	} else 	if (key & KEY_RIGHT) {
		if (NOT_SELECTED == menu_position.second_level) {
			menu_position.second_level = 1;
		} else {
			menu_position.entry_selected = 1;
		}
	} else if (key & KEY_LEFT) {
		if (NOT_SELECTED != menu_position.second_level && 0 == menu_position.entry_selected) {
			menu_position.second_level = NOT_SELECTED;
		} else {
			menu_position.entry_selected = 0;
		}
	}

	if (! menu_position.entry_selected) {
		//redraw menu
		LCD_Rectangle(MENU_X_POS-16-8*MENU_ROWS,0,56,132,MENUCOLOR_BACKGROUND);
	}

	if (NOT_SELECTED == menu_position.second_level) {
		//dispay first level menulist
		for (uint8_t i = 0; i<(sizeof(menu_first_level)/sizeof(menu_first_level[0])); i++) {
			LCD_PutStr_P((char *)(menu_first_level[i]), MENU_X_POS-i*8, 2, 0, MENUCOLOR_TEXT, (i==menu_position.first_level)?MENUCOLOR_CURSOR:MENUCOLOR_BACKGROUND);
		}

	} else {
		// submenu
		LCD_PutStr_P((char *)(menu_first_level[menu_position.first_level]), MENU_X_POS, 2, 0, MENUCOLOR_HEADER, MENUCOLOR_BACKGROUND);
//		n = submenu_entries_count[menu_position.first_level]);
//		if (n > MENU_ROWS) {
//			n = MENU_ROWS;
//		}
		for (uint8_t i = 0; i<submenu_entries_count[menu_position.first_level]; i++) {
			if (menu_position.entry_selected && menu_position.second_level!=i+1) {
				//do not redraw another items in edit mode
				/*LCD_SkipLine(0);*/ //increment cursor by 1 row
				continue;
			}
			//print entry description
			LCD_PutStr_P((char *)(menu_second_level[Menu_ObjectIndex(menu_position.first_level, i+1)]),
					MENU_X_POS-8-i*8, 10, 0,
					MENUCOLOR_TEXT, (i==menu_position.second_level-1)?MENUCOLOR_CURSOR:MENUCOLOR_BACKGROUND);

			//acquire pointer to callback function
			CallbackFunction=(FuncPtr)functions[Menu_ObjectIndex(menu_position.first_level, i+1)];
			//execute if not null
			if (CallbackFunction!=0) {
				CallbackFunction((menu_position.entry_selected && menu_position.second_level==i+1)?key:0,
						variables[Menu_ObjectIndex(menu_position.first_level, i+1)] );
			}

		}

	}
}
