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


//	PORTD |= _BV(0);

	PORTC |= (_BV(OUT1)|_BV(OUT2)|_BV(OUT3));

	//Unused connector
	PORTA |= 0xff;
	PORTB |= 0x0f;

	//Buzzer
	BUZZ_DDR |= _BV(BUZZ);
	//BUZZ_PORT |= _BV(BUZZ);

	//SPI
	SPI_DDR |= _BV(SPI_MOSI) | _BV(SPI_SCK);
	SPI_PORT |= _BV(SPI_MISO); //pullup on MISO


	USART_Init(38400);
	TC_init();

	//enable interrupts
	sei();

	//LCD
	LCD_Init();
	LCD_Test();

	USART_Puts_P(VERSION);
_delay_ms(10);
	uint8_t j=0, c=0;

/*	int32_t iks = 1600;
	USART_TransmitBinary(iks>>8);
	USART_TransmitBinary(iks&0xff);
    USART_TransmitDecimalSigned(iks); USART_Puts("\r\n");
    _delay_ms(10);
	iks = -250;
	USART_TransmitBinary(iks>>8);
	USART_TransmitBinary(iks&0xff);
    USART_TransmitDecimalSigned(iks); USART_Puts("\r\n");
*/
	volatile uint8_t status;
	//GLOWNA PETLA ************************************************************
	uint8_t bajt;
	int16_t deg;
	uint16_t milideg;

	for (;;) {
		//clear background
		LCD_Rectangle(100,0,10,100,RED);
		LCD_Rectangle(5,10,5,10,BLUE);

		//display Temp
		if (0==(status=TC_performRead())) {
			TC_getTCTemp(&deg, &milideg);
			LCD_PutStr("TC: ", 110, 5, 0, BLACK, WHITE);
			LCD_PutDecimalSigned(deg, 110, 35, 0, RED,WHITE);
			LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
			LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);

			TC_getInternalTemp(&deg, &milideg);
			LCD_PutStr("IN: ", 100,5,0,BLACK,WHITE);
			LCD_PutDecimalSigned(deg, 100, 35, 0, GREEN,WHITE);
			LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);
			LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);


		} else {
			LCD_PutStr("TC ERR:", 120,0,0,RED,WHITE);
			if (!(0==(TC_READOC&status))) {
				LCD_PutStr("Open!", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
			}
			if (!(0==TC_READSCG)) {
				LCD_PutStr("Short-!", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
			}
			if (!(0==TC_READSCV)) {
				LCD_PutStr("Short+!", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
			}
		}
		USART_Puts("\r\n");

		if (0!=buf_getcount(&USART_buffer_RX)) {
    		bajt = buf_getbyte(&USART_buffer_RX);
    		USART_Put(bajt);
    		USART_StartSending();
    		if ('B'==bajt) {
    			TC_performRead();
    			TC_debug();
    		}
    		if ('A'==bajt) {
    			_delay_ms(10);

    		}

    	}
    /*	if (!(SW1_PIN & _BV(SW1))) {
    		BUZZ_PORT |= _BV(BUZZ);
    		//	LCD_Rectangle(10,10,10,10, GREEN);
    	} else {
    		BUZZ_PORT ^= ~_BV(BUZZ);
    	}*/
    /*	if (!(SW2_PIN & _BV(SW2))) {
    		SPI_PORT |= _BV(SPI_SCK);
    		USART_Put('2');USART_StartSending();
    		//LCD_Rectangle(10,10,10,10, BLUE);
    	} else {
    		SPI_PORT &= ~_BV(SPI_SCK);
    	}
    	if (!(SW3_PIN & _BV(SW3))) {
    		ADC_CSPORT |= _BV(ADC_CS);
    		USART_Put('3');USART_StartSending();
    		//LCD_Rectangle(10,10,10,10, BLUE);
    	} else {
    		ADC_CSPORT &= ~_BV(ADC_CS);
    	}
    	if (!(SPI_PIN & SPI_MISO)) {
    		BUZZ_PORT |= _BV(BUZZ);
    		USART_Put('M');USART_StartSending();
    	} else {
    		BUZZ_PORT ^= ~_BV(BUZZ);

    	}
    */
	_delay_ms(500);
	}



}				/* main() */
