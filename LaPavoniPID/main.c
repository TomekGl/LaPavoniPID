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

	//Pullups
    SW1_PORT |= _BV(SW1);
    SW2_PORT |= _BV(SW2);
    SW3_PORT |= _BV(SW3);
    SW4_PORT |= _BV(SW4);

    PORTC |= (_BV(OUT1)|_BV(OUT2)|_BV(OUT3));

    PORTA |= 0xff;
    PORTB |= 0x0f;

    LCDBL_DDR |= _BV(LCDBL);

    //USART
    USART_Init(51);

    //enable interrupts
    sei();

    USART_Puts("DUPA");
//    USART_Puts_P(VERSION);

    //GLOWNA PETLA ************************************************************
    for (;;) {
	waitms(50);

	if (SW1_PIN & _BV(SW1)) {
		USART_Put('.');
		LCDBL_PORT |= _BV(LCDBL);
	} else {
		USART_Put('1');
		LCDBL_PORT &= ~_BV(LCDBL);
	}
	if (SW2_PIN & _BV(SW2)) { USART_Put('.'); } else { USART_Put('2');}
	if (SW3_PIN & _BV(SW3)) { USART_Put('.'); } else { USART_Put('3');}
	if (SW4_PIN & _BV(SW4)) { USART_Put('.'); } else { USART_Put('4');}
//	USART_TransmitBinary(PINC);
//	USART_TransmitBinary(PIND);
	USART_Put('\r');
	USART_Put('\n');
	USART_StartSending();

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
