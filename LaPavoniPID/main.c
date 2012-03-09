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



	//USART
	USART_Init(38400);

	//enable interrupts
	sei();

	USART_Puts("PID Controller ");
	USART_Puts_P(VERSION);
	USART_Puts("\r\nOK\r\n");
	USART_StartSending();

	//LCD
	LCD_Init();
	LCD_Test();

	uint8_t j=0, c=0;

	LCD_PutStr("PID Controler",124,0,0,GREEN,BLACK);
	LCD_PutStr_P(VERSION,255,255,0,GREEN,BLACK);

	volatile uint8_t status;
	//GLOWNA PETLA ************************************************************

	for (;;) {
    	//LCD_Rectangle(10,50,16,8, YELLOW);
    	//LCDPutChar('0'+j, (j/8)*16, (j%8)*16, 1, 0x0fff, 0x0000);
		//LCD_Rectangle2();
		//if (j++>144) { j=0; }
		//LCD_Test(j);
		//j++;
		//LCD_PutStr("AA",255,255,0,BLACK,BLUE);
		//LCD_Blank();

		//waitms(100);

		//BUZZ_DDR |= _BV(BUZZ);

		/*if (SW1_PIN & _BV(SW1)) {
			BUZZ_PORT |= _BV(BUZZ);
		}
		//USART_Put('.'); } else { USART_Put('1');}
		if (SW2_PIN & _BV(SW2)) {
			BUZZ_PORT &= ~_BV(BUZZ);
		}
		//	USART_Put('.'); } else { USART_Put('2');}
		if (SW3_PIN & _BV(SW3)) { USART_Put('.'); } else { USART_Put('3');}
		if (SW4_PIN & _BV(SW4)) { USART_Put('.'); } else { USART_Put('4');}
		//	USART_TransmitBinary(PINC);
		//	USART_TransmitBinary(PIND);

		USART_Put('\r');
		USART_Put('\n');
		USART_StartSending();
*/

/*		while (!(status = (UCSRA & _BV(RXC))))
					;
			status = UCSRA;
			if (status & (FRAMING_ERROR|PARITY_ERROR|DATA_OVERRUN)) {
				status = UDR; //discard data
			} else {
				LCD_PutChar(UDR, 255, 255, 0, WHITE, BLACK);
			}
*/
	    if (buf_getcount(&USART_buffer_RX)) {
	    	LCD_PutChar(buf_getbyte(&USART_buffer_RX), 255, 255, 0, WHITE, BLACK);
	    }
	    //b = buf_getbyte(&USART_buffer_RX);
	    //setPixel(r,g,b);
	}



}				/* main() */
