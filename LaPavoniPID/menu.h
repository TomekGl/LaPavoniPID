#ifndef MENU_H_
#define MENU_H_

/**
 @file menu.h

 @defgroup menu Multilevel menu library
 @code #include <menu.h> @endcode

 @brief multilevel menu library

 This module provides support multilevel menu dispayed on LCD.

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 18-03-2012

 Using this library requires creates menu structure and connect variable and callback function with menu entries.
 @code
 const char menu_entry_0[] __attribute__ ((progmem)) = "Submenu1" ;
 const char menu_entry_0_0[] __attribute__ ((progmem)) = "Integer: " ;
 const char menu_entry_1[] __attribute__ ((progmem)) = "Submenu2 " ;
 const char menu_entry_1_0[] __attribute__ ((progmem)) = "Integer RO: " ;
 const char menu_entry_1_1[] __attribute__ ((progmem)) = "Boolean: " ;

  //submenus
  const char *menu_first_level[] __attribute__ ((progmem)) = {
		menu_entry_0,
		menu_entry_1,
  };
  //submenu etries
  const char  *menu_second_level[] __attribute__ ((progmem)) =  {
		menu_entry_0_0,
		menu_entry_1_0, menu_entry_1_1
  };

  //functions called when entry is displayed/edited
  const FuncPtr functions[] __attribute__ ((progmem)) = {
		(void*)&setSignedInteger16,
		(void*)&getIntegerReadOnly, (void*)&setBoolean,
  };
  //variables being able to edit in menu entry
  const void * variables[] = {
		(void *)&(var1),
		(void *)&var2, (void *)&var3,
  };

  //menu map
  const uint8_t submenu_entries_count[] = {1,2 };
 @endcode
*/

/**@{*/

/// this flag represent long pressing of button
#define REPEATED_FLAG 1<<7
/// this flag represent very long pressing of button
#define REPEATED_2X_FLAG 1<<6

/// Start drawing menu at row:
#define MENU_X_POS 64

///Max menu rows displayed
#define MENU_ROWS 4

// Colors of menu elements
#define MENUCOLOR_CURSOR BLUE ///< cursor color
#define MENUCOLOR_BACKGROUND BLACK ///< cursor color
#define MENUCOLOR_TEXT WHITE ///< menu element text color
#define MENUCOLOR_VALUE RED ///< value text color
#define MENUCOLOR_HEADER ORANGE ///< section label color



///Keys status masks
typedef enum {
	IDLE = 0, ///< no button pressed
	KEY_UP = 1, ///< cursor up pressed
	KEY_DOWN = 2, ///< cursor down pressed
	KEY_LEFT = 4, ///< cursor left pressed
	KEY_RIGHT = 8 ///< cursor right pressed
} TKey;

/// Coordinates of cursor in menu tree
struct Tmenu_position {
	uint8_t first_level; ///< Current first-level position
	uint8_t second_level; ///< Current second-level position
	uint8_t entry_selected; ///< Whether entry is in edit mode
};
/**
 * \brief Initialize menu
 */
void menu_Init(void);

/**
 * \brief Process button press and redraw new menu state if necessary
 * @param key Pressed keys structure
 */
void MenuProcess(TKey key);


/******* CALLBACKS ********/

/** \brief Callback function  to display/increase/decrease integer variable depending on keys status
 * @param keys Pressed keys structure
 * @param *value Pointer to variable
 */
void setInteger(uint8_t keys, uint8_t * value);

/** \brief Callback function to display-only integer variable depending on keys status
 * @param keys Pressed keys structure
 * @param *value Pointer to variable
 */
void getIntegerReadOnly(uint8_t keys, uint8_t * value);

/** \brief Callback function to display/increase/decrease signed integer variable depending on keys status
 * @param keys Pressed keys structure
 * @param *value Pointer to variable
 */
void setSignedInteger16(uint8_t keys, int16_t * value);

/** \brief Callback function to display/increase/decrease boolean variable depending on keys status
 * @param keys Pressed keys structure
 * @param *value Pointer to variable
 */
void setBoolean(uint8_t keys, uint8_t * value);

/** \brief Dedicated callback function to take action after button being pressed on, after confirmation
 * @param keys Pressed keys structure
 * @param *value Might be NULL
 */
void callAfterConfirm(uint8_t keys, uint8_t * value);

/** @} */

#endif /* MENU_H_ */
