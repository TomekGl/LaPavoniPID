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

	//enable interrupts
	sei();
	//LCD
	LCD_Init();
	//USART
	USART_Init(38400);
	//Thermocouple
	TC_init();
	//RTC
	DS1307_Init();

	USART_Puts_P(VERSION);

	uint8_t status;
	uint8_t bajt;
	int16_t deg;
	uint16_t milideg;
	uint8_t switch_status = 0xff, prev_switch_status = 0xff;
	char screen_buf_1[17] = "################";
	int16_t pv=0, output=0;
	menu_Init();
	PID_Init();
	//GLOWNA PETLA ************************************************************


	for (;;) {


		if (0==(system_clock%100)) {
			DS1307_Read();
			prepare_timeline((char *)&screen_buf_1);
			LCD_PutStr((char *)&screen_buf_1, 122,5,0,BLACK,WHITE);

		}
		if (15==(system_clock%100)) {
			DS1307_Read();
			prepare_timeline((char *)&screen_buf_1);
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

				USART_TransmitDecimal(milideg);
				USART_Put('\n');
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
			LCD_PutStr("PV:", 90,5,0,BLUE,WHITE);
			LCD_PutDecimalSigned(controller.PV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
			LCD_PutStr(",SV:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
			LCD_PutDecimalSigned(controller.SV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
			LCD_PutStr(",E:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
			LCD_PutDecimalSigned(controller.e, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
		}

		switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2));

		if (switch_status < prev_switch_status) {
			BUZZ_PORT |= _BV(BUZZ);
			_delay_ms(100);
			MenuProcess();
			BUZZ_PORT &= ~_BV(BUZZ);
		}
		prev_switch_status = switch_status;


		if (system_clock==1007) {
			_delay_ms(10);
			for(uint8_t i=0; i<sizeof(Tsystime); i++) {
				USART_Put(systime.chars[i]);
			}
			USART_StartSending();
		}

		//clear background


		if (0!=buf_getcount((Tcircle_buffer *)&USART_buffer_RX)) {
			bajt = buf_getbyte((Tcircle_buffer *)&USART_buffer_RX);
			USART_Put(bajt);
			USART_StartSending();


			switch ( bajt)
			{
			// Send a Generall Call
			case ('a'):
		    		//rst
					break;



			// Send a Address Call, sending a command and data to the Slave
			case ('B'):
		        				  //start
					break;
			case ('S'):
					   //square
					break;
			// Send a Address Call, sending a request, followed by a resceive
			case ('c'):
		    		  //read
					break;

			}
		}

		//_delay_ms(1);
	}
}				/* main() */
