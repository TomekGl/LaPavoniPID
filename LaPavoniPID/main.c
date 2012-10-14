#include "includes.h"

#define FLAG_1S 1
#define FLAG_100MS 2
#define FLAG_10MS 4

//enable beeper
#define BEEPER

/// time of build
const char TXT_VERSION[] __attribute__ ((progmem)) = "Build:" __DATE__ " " __TIME__;
const char TXT_Hello[] __attribute__ ((progmem)) = "Coffee PID controller v0.2\n tomaszgluch.pl 2012\n";
const char TXT_SerialInit[] __attribute__ ((progmem)) = "USART initialization...";
const char TXT_PumpTimer[] __attribute__ ((progmem)) = "PUMP TIMER: ";
const char TXT_PV[]  __attribute__ ((progmem)) = "PV:";
const char TXT_OK[]  __attribute__ ((progmem)) = "OK.";
const char TXT_TCError[] __attribute__ ((progmem)) = "TC ERR:";
const char TXT_TCErrorOpen[] __attribute__ ((progmem)) = "Open!";
const char TXT_TCErrorShortPlus[] __attribute__ ((progmem)) = "Short+!";
const char TXT_TCErrorShortMinus[] __attribute__ ((progmem)) = "Short-!";
volatile uint8_t flag;


//Moving average window size
#define AVG_WIN_SIZE 8


/// System clock control routine
ISR(TIMER0_OVF_vect)
{
	// load
	TCNT0 = timer0;
	system_clock++;

	if (system_clock%100 == 0) {
		flag = _BV(FLAG_1S) | _BV(FLAG_100MS);
		return;
	}
	if (system_clock%10 == 0) {
		flag = _BV(FLAG_100MS);
		return;
	}
	return;
}

/// Timer 2 support - AC output 3
ISR(TIMER2_OVF_vect) {
	TCNT2 = 255-30;
	pwm++;
	if (0 == output) {
		OUT3_PORT |= _BV(OUT3);//disable
		//BUZZ_PORT &= ~_BV(BUZZ);
	} else {
		if (pwm <= output)  {
			OUT3_PORT &= ~_BV(OUT3);
			//BUZZ_PORT |= _BV(BUZZ);
		} else {
			OUT3_PORT |= _BV(OUT3);
			//BUZZ_PORT &= ~_BV(BUZZ);
		}
	}
	return;
}

/// External interrupt - AC input 1
ISR(INT1_vect)
{
	if (tmp_in != 255) {
	tmp_in++;
	}

	in_flag = 4;
	return;
}

void __attribute__ ((naked)) main(void) {
	int16_t averaging_window[AVG_WIN_SIZE];
	int16_t average;
	uint8_t window_index = 0;
	int32_t avg_tmp_sum;
	uint8_t status,prevstatus;
	uint8_t bajt;
	int16_t deg;
	uint16_t milideg;
	uint8_t switch_status = 0xff, prev_switch_status = 0xff;
	uint8_t repeat, repeated_flag;
	int16_t pv=100; //, output=0;



	timer0=178;
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


	// timer 0 - zegar systemowy 8e6 / 1024 / 79 ~= 100Hz     7372800/
	TCCR0 |= _BV(CS00) | _BV(CS02);
	TIMSK |= _BV(TOIE0);
	TCNT0 = timer0; //255-72;

	// timer 2 - output  T=~ 1s
	TCCR2 |= _BV(CS20) | _BV(CS21) | _BV(CS22);
	TIMSK |= _BV(TOIE2);
	TCNT2 = 255-29; //255-72;

	//enable interrupts
	sei();

	//LCD
	LCD_Init();
	LCD_PutStr_P(TXT_Hello, 112, 5, 0, BLACK, WHITE);
	LCD_PutStr_P(TXT_VERSION, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLACK, WHITE);

	//USART
	USART_Init((unsigned int)115200);

	//Thermocouple
	TC_init();

	//RTC
	//	DS1307_Init();

	USART_Puts_P(TXT_Hello);
	USART_Puts_P(TXT_VERSION);

	//Initialize runtime variables
	PID_Init();


	for (uint8_t i=0; i<20; i++) {
		_delay_ms(100);
	}
	LCD_Blank(0);
	menu_Init();
	MenuProcess(0);

	BUZZ_ON;
	_delay_ms(100);
	BUZZ_OFF;

	wdt_enable(WDTO_2S);

	//GLOWNA PETLA ************************************************************

	while (1) {
		//process received data on serial port
		if (0!=buf_getcount((Tcircle_buffer *)&USART_buffer_RX)) {
			bajt = buf_getbyte((Tcircle_buffer *)&USART_buffer_RX);
			//LCD_PutChar(bajt,0,LCD_AUTOINCREMENT,0,BLACK,WHITE);
			USART_Put(bajt);
			USART_StartSending();
			//if ('A'==bajt) {
		}


		/*	if (tmp_buzz) {
			BUZZ_PORT |= _BV(BUZZ);
		} else {
			BUZZ_PORT &= ~_BV(BUZZ);
		}
		 */
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
		/*
		if (tmp_out3) {
			OUT3_PORT |= _BV(OUT3);
		} else {
			OUT3_PORT &= ~_BV(OUT3);
		}
		 */
		if (flag & _BV(FLAG_1S)) {
			flag &= ~_BV(FLAG_1S);

			// if no error from termocouple converter
			if (0==(status=TC_performRead())) {
				if (0 != prevstatus) {
					LCD_Rectangle(110,0,8,132,WHITE);
					BUZZ_ON;
					_delay_ms(10);
					BUZZ_OFF;
				}

				TC_getTCTemp(&deg, &milideg);
				pv=deg*10+(milideg/10);

				/*  Display process value */
				LCD_PutStr_P(TXT_PV, 112, 5, 1, BLACK, WHITE);
				LCD_PutDecimalSigned(deg, 112, 35, 1, RED,WHITE);
				LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, RED,WHITE);
				LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, RED,WHITE);
				LCD_PutStr(" oC ", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, RED,WHITE);

				LCD_PutStr_P(TXT_PumpTimer, 98,5,1,BLACK,WHITE);
				LCD_PutDecimal(pump_timer/10, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT,1, GREEN,WHITE);
				LCD_PutChar('s', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, 0, WHITE);
				LCD_PutChar(' ', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, 0, WHITE);

//				TC_getInternalTemp(&deg, &milideg);
/*				LCD_PutStr("IN: ", 102,5,0,BLACK,WHITE);
				LCD_PutDecimalSigned(deg, 102, 35, 0, GREEN,WHITE);
				LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);
				LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, GREEN,WHITE);
*/

				//USART_TransmitDecimal(milideg);
				//USART_Puts_P(VERSION);
				//USART_Put('\n');

				if (controller_param.k_r>0) {
				//process PID controller
				output = (uint8_t)(PID_Process(pv));
				} else if (-1 == controller_param.k_r) {
					output = (uint8_t)controller.y;
				} else {
					output = 0;
				}

				LCD_PutStr("PV:", 87,5,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.PV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",SP:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller_param.SV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",E:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.e, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
//				LCD_PutStr(",OUT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,RED,WHITE);
				//LCD_PutDecimalSigned(output, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
				LCD_PutStr(",INT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.integral, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",DVDT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.derivative, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(" ", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
			//correct TC read END
			} else {
				// some error while TC reading has occured
				output = 0;
				if (0 == prevstatus) { //only on transition to erroneous state
					//clear controller section on LCD
					LCD_Rectangle(96, 0, (132-96), 132, WHITE);
					BUZZ_ON;
					_delay_ms(100);
					BUZZ_OFF;
				}

				LCD_PutStr_P(TXT_TCError, 110,5,0,RED,WHITE);
				if (!(0==(TC_READOC&status))) {
					LCD_PutStr_P(TXT_TCErrorOpen, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
				}
				if (!(0==(TC_READSCG&status))) {
					LCD_PutStr_P(TXT_TCErrorShortMinus, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
				}
				if (!(0==(TC_READSCV&status))) {
					LCD_PutStr_P(TXT_TCErrorShortPlus, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED, WHITE);
				}
			}
			prevstatus = status;

		/*	USART_Put('\n');
			USART_TransmitDecimal(system_clock);
			USART_Put(',');
			USART_TransmitDecimalSigned(controller.PV);
			USART_Put(',');
			USART_TransmitDecimalSigned(controller.y);
			USART_Put(',');
			USART_TransmitDecimalSigned(output);
			USART_Put(',');
			USART_TransmitDecimalSigned(controller.integral);
			USART_Put(',');
			USART_TransmitDecimalSigned(controller.derivative);
*/
		} // end of flagged 1S section

		if (flag & _BV(FLAG_100MS)) {
			flag &= ~_BV(FLAG_100MS);

			// reset timer automatically after switching pump off
			if (0 != in_flag) {
				in_flag--;
				pump_timer++;
				pump_timer_reset_timeout = PUMP_TIMER_RESET_TIMEOUT;
			} else {
				if (0 != pump_timer_reset_timeout) {
					pump_timer_reset_timeout--;
				} else {
					pump_timer = 0;
				}
			}

			LCD_PutDecimal(system_clock, 0,0,0,BLACK,WHITE);
			LCD_PutChar(' ', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, 0, WHITE);
			//LCD_PutDecimal(output, 0,100,0,BLACK,WHITE);
			//LCD_PutChar(' ', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, 0, WHITE);

			//MOVING AVERAGE
			status = TC_performRead();
			TC_getTCTemp(&deg, &milideg);
			USART_Put('\r');USART_Put('\n');
			USART_TransmitDecimal(system_clock);
			USART_Put(',');
			USART_TransmitDecimalSigned(deg*100+milideg);
			USART_Put(',');
			USART_TransmitDecimalSigned(output);
			USART_Put(',');
			USART_TransmitDecimal(status);
		//	USART_Puts(",0,0,0");

/*			averaging_window[window_index++] = deg*10 + milideg/10;
			if (AVG_WIN_SIZE-1 > window_index) window_index = 0;
			avg_tmp_sum = 0;
			for (uint8_t i=0; i<AVG_WIN_SIZE; i++) {
				avg_tmp_sum += averaging_window[i];
			}
			pv = avg_tmp_sum / AVG_WIN_SIZE;
*/

			//BUTTONS
			switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2));
			if (switch_status != (_BV(SW1)| _BV(SW3) | _BV(SW4) | _BV(SW2))) { //some button is pressed
				if (switch_status != prev_switch_status) { // key pressed
					_delay_ms(10);
					switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2)); //TODO

				} else { //end if status changed
					repeat++;
					if (repeat<10) goto SKIP;
					repeated_flag = REPEATED_FLAG;  //button pressed for 1s
					if (repeat>30) {
						repeated_flag = REPEATED_FLAG | REPEATED_2X_FLAG; //button pressed for 3s
						repeat--; //do not increment to avoid overflow
					}
				}
				if (bit_is_clear(switch_status, SW1)) { // && bit_is_set(prev_switch_status, SW1)) {
					//USART_Put('U');
					MenuProcess(KEY_UP|repeated_flag);
#ifdef BEEPER
					BUZZ_PORT |= _BV(BUZZ);
					_delay_ms(25);
					//LCD_PutStr("AA", 100,0,0,BLACK,WHITE);
					BUZZ_PORT &= ~_BV(BUZZ);
#endif
				}
				if (bit_is_clear(switch_status, SW2)) { // && bit_is_set(prev_switch_status, SW2)) {
					//USART_Put('D');
					MenuProcess(KEY_DOWN|repeated_flag);
#ifdef BEEPER
					BUZZ_PORT |= _BV(BUZZ);
					_delay_ms(25);
					BUZZ_PORT &= ~_BV(BUZZ);
#endif
				}
				if (bit_is_clear(switch_status, SW3) && bit_is_set(prev_switch_status, SW3)) {
					//USART_Put('L');
					MenuProcess(KEY_LEFT);
#ifdef BEEPER
					BUZZ_PORT |= _BV(BUZZ);
					_delay_ms(25);
					BUZZ_PORT &= ~_BV(BUZZ);
#endif
				}
				if (bit_is_clear(switch_status, SW4)&& bit_is_set(prev_switch_status, SW4)) {
					//USART_Put('R');
					MenuProcess(KEY_RIGHT);
#ifdef BEEPER
					BUZZ_PORT |= _BV(BUZZ);
					_delay_ms(25);
					BUZZ_PORT &= ~_BV(BUZZ);
#endif
				}

			} //end if pressed
			else {
				repeat = 0;
				repeated_flag = 0;
			}
			SKIP:
			prev_switch_status = switch_status;

		}
		//shutdown
		wdt_reset();
	} //main loop
}				/* main() */
