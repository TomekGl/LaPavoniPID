/*
 * menu.h
 *
 *  Created on: 18-03-2012
 *      Author: tomek
 */

#ifndef MENU_H_
#define MENU_H_

#define REPEATED_FLAG 1<<7
#define REPEATED_2X_FLAG 1<<6

///keys
typedef enum {
	IDLE = 0,
	KEY_UP = 1,
	KEY_DOWN = 2,
	KEY_LEFT = 4,
	KEY_RIGHT = 8
} TKey;

struct Tmenu_position {
	uint8_t first_level;
	uint8_t second_level;
	uint8_t entry_selected;
};

void menu_Init(void);
void MenuProcess(TKey key);
void setInteger(uint8_t keys, uint8_t * value);
void setIntegerReadOnly(uint8_t keys, uint8_t * value);
void setSignedInteger16(uint8_t keys, int16_t * value);
void setBoolean(uint8_t keys, uint8_t * value);
void callAfterConfirm(uint8_t keys, uint8_t * value);

#endif /* MENU_H_ */
