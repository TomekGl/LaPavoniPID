#include "includes.h"

/// data i czas kompilacji
const char VERSION[] __attribute__ ((progmem)) = __DATE__ " " __TIME__;


void waitms(uint16_t ms)
{
	uint8_t j, k;
	uint16_t i;
	for (i = 0; i < ms; i++)
		for (j = 0; j < 255; j++)
			for (k = 0; k < 16; k++)
				asm("nop  ");
	return;
}


ISR(TIMER0_OVF_vect)
{
}

ISR(INT1_vect)
{
}

void __attribute__ ((naked)) main(void) {

	//Pullups
	SW1_PORT |= _BV(SW1);
	SW2_PORT |= _BV(SW2);
	SW3_PORT |= _BV(SW3);
	SW4_PORT |= _BV(SW4);


	PORTD |= _BV(0);

	PORTC |= (_BV(OUT1)|_BV(OUT2)|_BV(OUT3));

	PORTA |= 0xff;
	PORTB |= 0x0f;

	//SS as output
	ADC_CSDDR |= _BV(ADC_CS);
	ADC_CSPORT |= _BV(ADC_CS);


	//enable interrupts
	sei();

	//LCD
	LCD_Init();
	LCD_Test();

	uint8_t j=0, c=0;

	volatile uint8_t status;
	//GLOWNA PETLA ************************************************************

	for (;;) {
    	LCD_Rectangle(10,50,16,8, YELLOW);
	}



}				/* main() */
