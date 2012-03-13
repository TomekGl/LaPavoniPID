#include "includes.h"

/// data i czas kompilacji
const char VERSION[] __attribute__ ((progmem)) = __DATE__ " " __TIME__ ;

ISR(TIMER0_OVF_vect)
{
        system_clock++;
        //tick_flag=1;
        TCNT0 = 255-79;
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

	//SPI
	SPI_DDR |= _BV(SPI_MOSI) | _BV(SPI_SCK);
	SPI_PORT |= _BV(SPI_MISO); //pullup on MISO

	// timer 0 - zegar systemowy 8e6 / 1024 / 79 ~= 100Hz
	TCCR0 |= _BV(CS00) | _BV(CS02);
	TIMSK |= _BV(TOIE0);
	TCNT0 = 255-79;



	USART_Init(38400);
	TC_init();
	DS1307_start();

	//enable interrupts
	sei();

	//LCD
	LCD_Init();
	LCD_Test();

	USART_Puts_P(VERSION);

	volatile uint8_t status;

	//GLOWNA PETLA ************************************************************
	uint8_t bajt;
	int16_t deg;
	uint16_t milideg;
	char screen_buf_1[17];
	strcpy(&screen_buf_1, "###############");

	for (;;) {
		//clear background
		DS1307_read((Tsystime *)&systime);
		prepare_timeline(screen_buf_1);
		LCD_PutStr(screen_buf_1, 50, 0, 0, BLACK, WHITE);


		//display Temp
		if (0==(status=TC_performRead())) {
			TC_getTCTemp(&deg, &milideg);
			LCD_PutStr("TC: ", 110, 5, 0, BLACK, WHITE);
			LCD_PutDecimalSigned(deg, 110, 35, 0, RED,WHITE);
			LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
			LCD_PutDecimalFixedDigits(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE,2);

			TC_getInternalTemp(&deg, &milideg);
			LCD_PutStr("IN: ", 100,5,0,BLACK,WHITE);
			LCD_PutDecimalSigned(deg, 100, 35, 0, GREEN,WHITE);
			LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);
			LCD_PutDecimalFixedDigits(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE,4);


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
    	if (!(SW1_PIN & _BV(SW1))) {
    		BUZZ_PORT |= _BV(BUZZ);
    		LCD_Rectangle(10,20,10,10, GREEN);
    	} else {
    		BUZZ_PORT &= ~_BV(BUZZ);
    	}
/*
    	if (!(SW2_PIN & _BV(SW2))) {
    		LCD_Rectangle(10,10,10,10, BLUE);
    		I2C_PORT &= ~_BV(I2C_SDA);
    		I2C_DDR  |= _BV(I2C_SDA);
    	} else {
    		I2C_DDR  &= ~_BV(I2C_SDA);
    	}

    	if (!(SW3_PIN & _BV(SW3))) {
    		LCD_Rectangle(20,10,10,10, BLUE);
    		I2C_PORT &= ~_BV(I2C_SCK);
    		I2C_DDR  |= _BV(I2C_SCK);
    	} else {
    		I2C_DDR  &= ~_BV(I2C_SCK);
    	}
*/

	_delay_ms(250);
	}



}				/* main() */
