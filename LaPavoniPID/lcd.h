/*
 * lcd.h
 * LCD based on PCF8833
  *  Created on: 04-03-2012
 *      Author: tomek
 *
 */

#ifndef LCD_H_
#define LCD_H_

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

void LCD_Init();
void LCD_Test(uint8_t mode);

#endif /* LCD_H_ */
