#include "includes.h"

//Start menu at row:
#define X_POS 64

//Max menu rows
#define MENU_ROWS 4

#define NOT_SELECTED 255

/// Colors of menu elements
#define MENUCOLOR_CURSOR BLUE
#define MENUCOLOR_BACKGROUND BLACK
#define MENUCOLOR_TEXT WHITE
#define MENUCOLOR_VALUE RED
#define MENUCOLOR_HEADER ORANGE

typedef void (*FuncPtr)(uint8_t keys, uint8_t * value );

//function pointer
FuncPtr FPtr;

/// menu strings
const char menu_entry_0[] __attribute__ ((progmem)) = "PID Settings" ;
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
const char menu_entry_2_0[] __attribute__ ((progmem)) = "TIM0: " ;
const char menu_entry_2_1[] __attribute__ ((progmem)) = "Save settings " ;
const char menu_entry_2_2[] __attribute__ ((progmem)) = "Backlight " ;


const char *menu_first_level[] __attribute__ ((progmem)) = {
		menu_entry_0,
		menu_entry_1,
		menu_entry_2
};

const char  *menu_second_level[] __attribute__ ((progmem)) =  {
		menu_entry_0_0, menu_entry_0_1, menu_entry_0_2, menu_entry_0_3, menu_entry_0_4,
		menu_entry_1_0, menu_entry_1_1, menu_entry_1_2, menu_entry_1_3,menu_entry_1_4,
		menu_entry_2_0, menu_entry_2_1, menu_entry_2_2
};

const FuncPtr functions[] __attribute__ ((progmem)) = {
		(void*)&setSignedInteger16, (void*)&setSignedInteger16, (void*)&setSignedInteger16, (void*)&setSignedInteger16, (void*)&setSignedInteger16,
		(void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setBoolean, (void*)&setInteger,
		(void*)&setInteger, (void*)&callAfterConfirm, (void*)&setInteger
};

const void * variables[] = {
		(void *)&(controller_param.SV), (void *)&(controller_param.k_r), (void *)&(controller_param.T_d), (void *)&(controller_param.T_i),(void *)&(controller.y),
		(void *)&tmp_buzz, (void *)&tmp_out1,(void *)&tmp_out2, (void *)&tmp_out3,/*(void *)&tmp_in,*/ (void *)&output,
		(void *)&timer0, (void*)&PID_SaveSettings, (void *)&OCR2
};

const uint8_t submenu_index[] = { 5,5,3 };

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
	if (key & KEY_DOWN) {
		if (NOT_SELECTED == menu_position.second_level) { //MAIN MENU
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
				menu_position.second_level = submenu_index[menu_position.first_level];
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

	//redraw menu//48
	LCD_Rectangle(X_POS-16-8*MENU_ROWS,0,56,132,MENUCOLOR_BACKGROUND);

	if (NOT_SELECTED == menu_position.second_level) {
		//dispay first level menulist
		for (uint8_t i = 0; i<(sizeof(menu_first_level)/sizeof(menu_first_level[0])); i++) {
			LCD_PutStr_P((char *)pgm_read_word(&(menu_first_level[i])), X_POS-i*8, 2, 0, MENUCOLOR_TEXT, (i==menu_position.first_level)?MENUCOLOR_CURSOR:MENUCOLOR_BACKGROUND);
		}

	} else {
		// submenu
		LCD_PutStr_P((char *)pgm_read_word(&(menu_first_level[menu_position.first_level])), X_POS, 2, 0, MENUCOLOR_HEADER, MENUCOLOR_BACKGROUND);
//		n = submenu_index[menu_position.first_level]);
//		if (n > MENU_ROWS) {
//			n = MENU_ROWS;
//		}
		for (uint8_t i = 0; i<submenu_index[menu_position.first_level]; i++) {
			//LCD_PutDecimal(resolve_index(menu_position.first_level, i+1), 92-i*8,0,0, MENUCOLOR_VALUE,BLACK);
			LCD_PutStr_P((char *)pgm_read_word(&(menu_second_level[resolve_index(menu_position.first_level, i+1)])), X_POS-8-i*8, 10, 0, MENUCOLOR_TEXT, (i==menu_position.second_level-1)?MENUCOLOR_CURSOR:MENUCOLOR_BACKGROUND);

			FPtr=(FuncPtr)pgm_read_word(&functions[resolve_index(menu_position.first_level, i+1)]);
			if (FPtr!=0) {
				FPtr((menu_position.entry_selected && menu_position.second_level==i+1)?key:0, variables[resolve_index(menu_position.first_level, i+1)] );
			}

		}

	}
}
