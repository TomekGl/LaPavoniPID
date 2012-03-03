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

void __attribute__ ((naked)) main(void)
{
    buf_init(&USART_buffer_TX);
    buf_init(&USART_buffer_RX);

    //Pullups
    SW1_PORT |= _BV(SW1);
    SW2_PORT |= _BV(SW2);
    SW3_PORT |= _BV(SW3);
    SW4_PORT |= _BV(SW4);

    //uint8_t znak,i;
    unsigned char ret;

 
    USART_Init(51);

//    i2c_init();			// initialize I2C library

    sei();

    USART_Puts("DUPA");

    //GLOWNA PETLA ************************************************************
    for (;;) {
	waitms(500);
	waitms(500);
	if (SW1_PIN && _BV(SW1)) { USART_Puts('1'); } else { USART_Puts('.');}
	waitms(1);
	if (SW2_PIN && _BV(SW2)) { USART_Puts('2'); } else { USART_Puts('.');}
	waitms(1);
	if (SW3_PIN && _BV(SW3)) { USART_Puts('3'); } else { USART_Puts('.');}
	waitms(1);
	if (SW4_PIN && _BV(SW4)) { USART_Puts('4'); } else { USART_Puts('.');}
	waitms(1);
	USART_Puts('\n');

/*	if (buf_getcount(&USART_buffer_RX) > 0) {
	    znak = buf_getbyte(&USART_buffer_RX);
	    USART_Puts(PINC);
	    if ('1' == znak) {
		USART_Transmit(
	    }
	}
*/

    }

}				/* main() */
