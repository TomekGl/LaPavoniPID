#ifndef LCD_H_
#define LCD_H_

/**
 @file lcd.h

 @defgroup lcd LCD based on PCF8833 library
 @code #include <lcd.h> @endcode

 @brief Routines to support LCD based on PCF8833 using hardware SPI

 This library provides routines to support LCD based on PCF8833 using hardware SPI. This display was commonly
 used in Nokia 6100/6610/5100 phones. It is 128x128 pixel with 8/12/16bit per pixel.

 @note Chipset support 132x132 matrix, but only 128x128 is visible.

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 04-03-2012

 @par Screen organization
     * 131,0                131,131
   X *--------------------*
     |                    |
     |                    |
   P |                    |
   a |                    |
   g |                    |
   e |                    |
   s |                    |
     |                    |
     |                    |
     |                    |
     *--------------------* Y
    0,0     Cols        0,131


*/

/**@{*/


/// Compile-in fonts and text-related functions
#define LCDTEXT


/// Define display based on Phillips chipset
#define PCF8833

/// This value passed as a coordinate parameter means to increase position from previous one
#define LCD_AUTOINCREMENT 255

/// Define pixel depth
//#define MODE16BPP
#define MODE12BPP
//#define MODE8BPP

#define LCD_SPIPORT PORTB
#define LCD_SPIDDR DDRB
#define LCD_SCK PB7
#define LCD_SDA PB5

#define LCD_CTRPORT PORTD
#define LCD_CTRDDR DDRD
#define LCD_CS PD2
#define LCD_RESET PD4

#define LCD_BLPORT PORTD
#define LCD_BL PD7
#define LCD_BLDDR DDRD
#define LCD_BLPIN PIND

/* Const */
#ifdef PCF8833
/**
 * \defgroup lcd_pcf_functions LCD controller functions list for Philips PCF8833
 * @{
 * Command list from pdf
 */
#define NOP			0x00 ///<No operation
#define SWRESET		0x01 ///<Software reset
#define BSTROFF		0x02 ///<Booster voltage off
#define BSTRON		0x03 ///<Booster voltage on
#define RDDIDIF		0x04 ///<Read display identification
#define RDDST		0x09 ///<Read display status
#define SLEEP_IN	0x10
#define SLEEP_OUT	0x11
#define PTLON		0x12 ///<Partial mode on
#define NORON		0x13 ///<Normal display mode on
#define INVOFF		0x20 ///<Display inversion off
#define INVON		0x21 ///<Display inversion on
#define DALO		0x22 ///<All pixel off
#define DAL			0x23 ///<All pixel on
#define SETCON		0x25 ///<Set contrast
#define DISPOFF		0x28 ///<Display off
#define DISPON		0x29 ///<Display on
#define CASET		0x2a ///<Column address set
#define PASET		0x2b ///<Page address set
#define RAMWR		0x2c ///<Memory write
#define RGBSET		0x2d ///<Colour set
#define MADCTL		0x36 ///<Memory data access control
#define COLMOD		0x3a ///<Interface pixel format

#define COLMOD_8BPP		0x02 ///<8-bit/pixel mode
#define COLMOD_12BPP	0x03 ///<12-bit/pixel mode
#define COLMOD_16BPP	0x05 ///<16-bit/pixel mode

#define MAD_RGB		3 ///<BGR
#define MAD_LAO		4 ///<line address order (bottom to top)
#define MAD_V		5 ///<Vertical (rotated, 90deg)
#define MAD_MX		6 ///<Mirror X
#define MAD_MY		7 ///<Mirror Y
/**@}*/
#endif

#ifdef EPSON
/**
 * \defgroup lcd_epson_functions LCD controller functions for Epson
 * @{
 */
#define DISON       0xaf
#define DISOFF      0xae
#define DISNOR      0xa6
#define DISINV      0xa7
#define COMSCN      0xbb
#define DISCTL      0xca
#define SLPIN       0x95
#define SLPOUT      0x94
#define PASET       0x75
#define CASET       0x15
#define DATCTL      0xbc
#define RGBSET8     0xce
#define RAMWR       0x5c
#define RAMRD       0x5d
#define PTLIN       0xa8
#define PTLOUT      0xa9
#define RMWIN       0xe0
#define RMWOUT      0xee
#define ASCSET      0xaa
#define SCSTART     0xab
#define OSCON       0xd1
#define OSCOFF      0xd2
#define PWRCTR      0x20
#define VOLCTR      0x81
#define VOLUP       0xd6
#define VOLDOWN     0xd7
#define TMPGRD      0x82
#define EPCTIN      0xcd
#define EPCOUT      0xcc
#define EPMWR       0xfc
#define EPMRD       0xfd
#define EPSRRD1     0x7c
#define EPSRRD2     0x7d
#define NOP         0x25
/**@}*/
#endif

/**
 * \defgroup lcd_colors Predefined colors
 * @{
 */
#define WHITE   0xFFF ///< white
#define BLACK   0x000 ///< black
#define RED     0xF00 ///< red
#define GREEN   0x0F0 ///< green
#define BLUE    0x00F ///< blue
#define CYAN    0x0FF ///< cyan
#define MAGENTA 0xF0F ///< magenta
#define YELLOW  0xFF0 ///< yellow
#define BROWN   0xB22 ///< brown
#define ORANGE  0xFA0 ///< orange
#define PINK    0xF6A ///< pink
/**@}*/


/* Functions */

//! Initialize display
void LCD_Init();

//! Try to reset display
void LCD_Reset();

//! Draw a rectangle
/**
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Width
 * @param height Height
 * @param foreground Color
 */
void LCD_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t foreground);

//! Clear screen
void LCD_Blank();


#ifdef LCDTEXT

//! Print single char
/**
 * @param c Character
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 */
void LCD_PutChar(char c, uint8_t x, uint8_t y, uint8_t size, int foreground, int background);

//! Print null-terminated string from RAM
/**
 * @param *s Pointer to null-terminated string in RAM
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 */
void LCD_PutStr(char *s, uint8_t x, uint8_t y, uint8_t size, int foreground, int background);

//! Print null-terminated string from RAM
/**
 * @param *s Pointer to null-terminated string in program memory
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 */
void LCD_PutStr_P(const char *s, uint8_t x, uint8_t y, uint8_t size, int foreground, int background);

//! Print unsigned integer as string
/**
 * @param value Pointer to null-terminated string in program memory
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 */
void LCD_PutDecimal(uint32_t value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background);

//! Print signed integer as string
/**
 * @param value Pointer to integer
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 */
void LCD_PutDecimalSigned(int32_t value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background);

//! Print unsigned integer as string, right aligned
/**
 * @param value Pointer to unsigned integer
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 * @param digits Lenght of string, including leading spaces
 */
void LCD_PutDecimalFixedDigits(uint32_t value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background, uint8_t digits);

//! Print double as string
/**
 * @param value Pointer to double
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Font size index
 * @param foreground Foreground color
 * @param background Background color
 */
void LCD_PutDouble(double value, uint8_t x, uint8_t y, uint8_t size, int foreground, int background);

#endif

/** @} */

#endif /* LCD_H_ */
