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
	//TC_init();

	sei();
	//DS1307_start();
	//DS1307_Reset();
	_delay_ms(100);
	DS1307_Init();

	//enable interrupts


	//LCD
	LCD_Init();



	LCD_Test();

	USART_Puts_P(VERSION);

	volatile uint8_t status;

	//GLOWNA PETLA ************************************************************
	uint8_t bajt;
	int16_t deg;
	uint16_t milideg;
	char screen_buf_1[17] = "################";
	//strcpy(&screen_buf_1, "###############");

	for (;;) {
		if (0==(system_clock%100)) {
			//USART_Puts("TIK\r\n");
			DS1307_Read();
			prepare_timeline(&screen_buf_1);
			LCD_PutStr(&screen_buf_1, 30,10,0,BLACK,GREEN);
			USART_Puts(&screen_buf_1);
			USART_Put('_');
			USART_TransmitDecimal(system_clock);
			USART_Puts("\n");

		}
		if (system_clock==1007) {
			_delay_ms(10);
			for(uint8_t i=0; i<sizeof(Tsystime); i++) {
				USART_Put(systime.chars[i]);
			}
			USART_StartSending();
		}
		DS1307_Process();
		//********************

		//clear background
		//DS1307_read((Tsystime *)&systime);
		//prepare_timeline(screen_buf_1);


		if (0!=buf_getcount(&USART_buffer_RX)) {
			bajt = buf_getbyte(&USART_buffer_RX);
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

		_delay_ms(10);
	}
}				/* main() */
