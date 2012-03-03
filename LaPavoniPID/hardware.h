#ifndef HARDWARE_H_
#define HARDWARE_H_

#ifndef F_CPU
/** częstotliwość procesora */
#define F_CPU 8000000UL
#endif

#define BUZZ_PORT PORTD
#define BUZZ PD5
#define BUZZ_DDR DDRD
#define BUZZ_PIN PIND

#define LCDBL_PORT PORTD
#define LCDBL PD7
#define LCDBL_DDR DDRD
#define LCDBL_PIN PIND

#define SQW_PIN PIND
#define SQW PD3
#define SQW_DDR DDRD 
#define SQW_PORT PORTD

#define SW1_PIN PINC
#define SW1 PC3
#define SW1_DDR DDRC
#define SW1_PORT PORTC

#define SW2_PIN PIND
#define SW2 PD6
#define SW2_DDR DDRD
#define SW2_PORT PORTD

#define SW3_PIN PINC
#define SW3 PC2
#define SW3_DDR DDRC
#define SW3_PORT PORTC

#define SW4_PIN PINC
#define SW4 PC4
#define SW4_DDR DDRC
#define SW4_PORT PORTC


#endif /*HARDWARE_H_*/
