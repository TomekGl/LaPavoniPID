/*! \file menu.h
    \brief Multilevel dynamic menu

    This module provides support multilevel menu dispayed on LCD.
*/

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

///Keys status
typedef enum {
	IDLE = 0,
	KEY_UP = 1,
	KEY_DOWN = 2,
	KEY_LEFT = 4,
	KEY_RIGHT = 8
} TKey;

/// Coordinates of cursor in menu tree
struct Tmenu_position {
	uint8_t first_level;
	uint8_t second_level;
	uint8_t entry_selected;
};
/**
 * \brief Initialize menu
 */
void menu_Init(void);

/**
 * \brief Process button press and redraw new menu state
 * @param key Pressed keys structure
 */
void MenuProcess(TKey key);

/** \brief Callback function  to display/increase/decrease integer variable depending on keys status
 * @param key Pressed keys structure
 * @param *value Pointer to variable
 */
void setInteger(uint8_t keys, uint8_t * value);

/** \brief Callback function to display-only integer variable depending on keys status
 * @param key Pressed keys structure
 * @param *value Pointer to variable
 */
void setIntegerReadOnly(uint8_t keys, uint8_t * value);

/** \brief Callback function to display/increase/decrease signed integer variable depending on keys status
 * @param key Pressed keys structure
 * @param *value Pointer to variable
 */
void setSignedInteger16(uint8_t keys, int16_t * value);

/** \brief Callback function to display/increase/decrease boolean variable depending on keys status
 * @param key Pressed keys structure
 * @param *value Pointer to variable
 */
void setBoolean(uint8_t keys, uint8_t * value);

/** \brief Dedicated callback function to take action after button being pressed on, after confirmation
 * @param key Pressed keys structure
 * @param *value Might be NULL
 */
void callAfterConfirm(uint8_t keys, uint8_t * value);

#endif /* MENU_H_ */
