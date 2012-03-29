#include "includes.h"



/// data i czas kompilacji
const char VERSION[] __attribute__ ((progmem)) = __DATE__ " " __TIME__ ;
const char Hello[] __attribute__ ((progmem)) = "Coffee PID controller v0.1\n";
const char STR_PV[]  __attribute__ ((progmem)) = "PV:";

ISR(TIMER0_OVF_vect)
{
	system_clock++;
	//tick_flag=1;
	TCNT0 = 255-79;
}

ISR(INT1_vect)
{
	tmp_in++;
}

void __attribute__ ((naked)) main(void) {
	//Pullups
	SW1_PORT |= _BV(SW1);
	SW2_PORT |= _BV(SW2);
	SW3_PORT |= _BV(SW3);
	SW4_PORT |= _BV(SW4);

	//Input
	MCUCR |= _BV(ISC11); //falling edge
	IN1_PORT |= _BV(IN1);
	GICR |= _BV(INT1);

	//Outputs
	DDRC |= (_BV(OUT1)|_BV(OUT2)|_BV(OUT3));
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

	//enable interrupts
	sei();
	//LCD
	LCD_Init();
	//USART
	USART_Init(115200);
	//Thermocouple
	TC_init();
	//RTC
	//	DS1307_Init();

	USART_Puts_P(Hello);
	USART_Puts_P(VERSION);

	uint8_t status;
	uint8_t bajt;
	int16_t deg;
	uint16_t milideg;
	uint8_t switch_status = 0xff, prev_switch_status = 0xff;
	int16_t pv=0, output=0;

	PID_Init();
	//GLOWNA PETLA ************************************************************
	LCD_PutStr_P(VERSION, 115,5,0,RED,WHITE);

	menu_Init();
	MenuProcess(0);

/*
	for (;;) {


	}
*/
	while (1) {

		//LCD_PutDecimal(buf_getcount((Tcircle_buffer *)&USART_buffer_RX), 8,90, 0, RED, BLACK);
		//LCD_PutDecimal(buf_getcount((Tcircle_buffer *)&USART_buffer_TX), 8,40, 0, RED, BLACK);
		if (0!=buf_getcount((Tcircle_buffer *)&USART_buffer_RX)) {
		    		bajt = buf_getbyte((Tcircle_buffer *)&USART_buffer_RX);
		    		//LCD_PutChar(bajt,0,LCD_AUTOINCREMENT,0,BLACK,WHITE);
		    		USART_Put(bajt);
		    		USART_StartSending();
		    		//if ('A'==bajt) {
		}


		if (tmp_buzz) {
			BUZZ_PORT |= _BV(BUZZ);
		} else {
			BUZZ_PORT &= ~_BV(BUZZ);
		}

		if (tmp_out1) {
			OUT1_PORT |= _BV(OUT1);
		} else {
			OUT1_PORT &= ~_BV(OUT1);
		}
		if (tmp_out2) {
			OUT2_PORT |= _BV(OUT2);
		} else {
			OUT2_PORT &= ~_BV(OUT2);
		}
		if (tmp_out3) {
			OUT3_PORT |= _BV(OUT3);
		} else {
			OUT3_PORT &= ~_BV(OUT3);
		}

		if (system_clock%100==0) {
			if (0==(status=TC_performRead())) {
							TC_getInternalTemp(&deg, &milideg);
							LCD_PutStr("IN: ", 100,5,0,BLACK,WHITE);
							LCD_PutDecimalSigned(deg, 100, 35, 0, GREEN,WHITE);
							LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);
							LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);

							TC_getTCTemp(&deg, &milideg);
							LCD_PutStr("PV: ", 110, 5, 0, BLACK, WHITE);
							LCD_PutDecimalSigned(deg, 110, 35, 0, RED,WHITE);
							LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
							LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
							pv=deg*10+(milideg/10);

							//USART_TransmitDecimal(milideg);
							//USART_Puts_P(VERSION);
							//USART_Put('\n');
						} else {
							LCD_PutStr("TC ERR:", 110,5,0,RED,WHITE);
							if (!(0==(TC_READOC&status))) {
								LCD_PutStr("Open!", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
							}
							if (!(0==(TC_READSCG&status))) {
								LCD_PutStr("Short-!", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
							}
							if (!(0==(TC_READSCV&status))) {
								LCD_PutStr("Short+!", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
							}
						}

			}
		if (50==(system_clock%100)) {
					//process PID controller
					output = PID_Process(pv);
					LCD_PutStr_P(STR_PV, 90,5,0,BLUE,WHITE);
					LCD_PutDecimalSigned(controller.PV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
					LCD_PutStr(",SV:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
					LCD_PutDecimalSigned(controller.SV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
					LCD_PutStr(",E:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
					LCD_PutDecimalSigned(controller.e, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
					LCD_PutStr(",OUT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
					LCD_PutDecimalSigned(output, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
					LCD_PutStr(",INT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
					LCD_PutDecimalSigned(controller.integral, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
					LCD_PutStr(",DVDT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
					LCD_PutDecimalSigned(controller.derivative, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);

					LCD_PutStr("  ", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);

				}


		switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2));
		if (switch_status != prev_switch_status) {
			_delay_ms(10);
			switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2));

			if (bit_is_clear(switch_status, SW1) && bit_is_set(prev_switch_status, SW1)) {
				USART_Put('U');
				MenuProcess(KEY_UP);
#ifdef BEEPER
				BUZZ_PORT |= _BV(BUZZ);
				_delay_ms(25);
				//LCD_PutStr("AA", 100,0,0,BLACK,WHITE);
				BUZZ_PORT &= ~_BV(BUZZ);
#endif
			}
			if (bit_is_clear(switch_status, SW2) && bit_is_set(prev_switch_status, SW2)) {
				USART_Put('D');
				MenuProcess(KEY_DOWN);
#ifdef BEEPER
				BUZZ_PORT |= _BV(BUZZ);
				_delay_ms(25);
				BUZZ_PORT &= ~_BV(BUZZ);
#endif
			}
			if (bit_is_clear(switch_status, SW3) && bit_is_set(prev_switch_status, SW3)) {
				USART_Put('L');
				MenuProcess(KEY_LEFT);
#ifdef BEEPER
				BUZZ_PORT |= _BV(BUZZ);
				_delay_ms(25);
				BUZZ_PORT &= ~_BV(BUZZ);
#endif
			}
			if (bit_is_clear(switch_status, SW4)&& bit_is_set(prev_switch_status, SW4)) {
				USART_Put('R');
				MenuProcess(KEY_RIGHT);
#ifdef BEEPER
				BUZZ_PORT |= _BV(BUZZ);
				_delay_ms(25);
				BUZZ_PORT &= ~_BV(BUZZ);
#endif
			}
			USART_StartSending();

		}
		prev_switch_status = switch_status;
	}
}				/* main() */
