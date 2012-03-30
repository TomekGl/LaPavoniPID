/*
 * menu.h
 *
 *  Created on: 18-03-2012
 *      Author: tomek
 */

#ifndef MENU_H_
#define MENU_H_

///keys
typedef enum {
	IDLE,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
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
