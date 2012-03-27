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


#endif /* MENU_H_ */
