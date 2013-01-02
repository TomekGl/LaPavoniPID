/**
 @file main.c
 @defgroup main Espresso PID controller
 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 03-01-2012
*/
/**@{*/
#include "includes.h"

/// timer flag 1s mask
#define FLAG_1S 1
/// timer flag 0.1s mask
#define FLAG_100MS 2
/// timer flag 0.01s mask
#define FLAG_10MS 4

///enable beeper
#define BEEPER
/// default buzzer timeout
#define BUZZER_TIME 15

/// time of build
const char TXT_VERSION[] __attribute__ ((progmem)) = "Build:" __DATE__ " " __TIME__;
const char TXT_Hello[] __attribute__ ((progmem)) = "Coffee PID controller v1.0\n\rtomaszgluch.pl 2012\r\n";
const char TXT_SerialInit[] __attribute__ ((progmem)) = "USART initialization...";
const char TXT_PumpTimer[] __attribute__ ((progmem)) = "PUMP TIMER: ";
const char TXT_PV[]  __attribute__ ((progmem)) = "PV:";
const char TXT_OK[]  __attribute__ ((progmem)) = "OK.";
const char TXT_TCError[] __attribute__ ((progmem)) = "TC ERR:";
const char TXT_TCErrorOpen[] __attribute__ ((progmem)) = "Open!";
const char TXT_TCErrorShortPlus[] __attribute__ ((progmem)) = "Short+!";
const char TXT_TCErrorShortMinus[] __attribute__ ((progmem)) = "Short-!";

volatile uint8_t flag;

volatile uint8_t buzzer_timeout;

/// System clock control routine
ISR(TIMER0_OVF_vect)
{
	// load
	TCNT0 = timer0;
	system_clock++;

#ifdef BEEPER
	if (buzzer_timeout>0) {
		if (buzzer_timeout == 1) {
			BUZZ_PORT &= ~_BV(BUZZ);
		}
		buzzer_timeout--;
	}
#endif

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

/// Timer 1 (16bit) support - AC output 3
ISR(TIMER1_OVF_vect) {
	TCNT1 = 65535-30; //FIXME

	pwm++;
	if (0 == output) {
		OUT3_PORT |= _BV(OUT3);//disable
	} else {
		if (pwm <= output)  {
			OUT3_PORT &= ~_BV(OUT3);
		} else {
			OUT3_PORT |= _BV(OUT3);
		}
	}

	return;
}

/// External interrupt - AC input 1
ISR(INT1_vect)
{
	//debug
	if (tmp_in != 255) {
		tmp_in++;
	}

	in_flag = 4; //timeout to avoid flapping when input is AC 50Hz, decremented every 0.1s
	return;
}

/// Non-blocking buzzer support
void BuzzerStart(uint8_t time) {
#ifdef BEEPER
	if (controller_param.buzzer_enabled == 0) return;
	BUZZ_PORT |= _BV(BUZZ);
	buzzer_timeout = time;
#endif
	return;
}


void __attribute__ ((naked)) main(void) {
	// TC ADC read status
	uint8_t status,prevstatus;
	int16_t deg;
	uint16_t milideg;


	uint8_t bajt;
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


	// timer 0 - system ticks generation      8e6 / 1024 / 79 ~= 100Hz     7372800/
	TCCR0 |= _BV(CS00) | _BV(CS02);
	TIMSK |= _BV(TOIE0);
	TCNT0 = timer0; //255-72;

	// timer 1 - output  T=~ 1s
	TCCR1B = _BV(CS10)| _BV(CS12)  /*| _BV(CS21)*/ ;
	TIMSK |= _BV(TOIE1);
	TCNT1 = 65535-29; //255-72;

	// PWM for LCD backlight
	TCCR2 |= /*_BV(CS20)|*/ /* _BV(CS21) |*/ _BV(CS22) //Prescaler /64
	        | _BV(WGM21) | _BV(WGM20) //Fast PWM
	        | _BV(COM21); //Clear OC2 on compare match, set OC2 at BOTTOM,
	OCR2 = 160; //tentative brightness setting

	//enable interrupts
	sei();

	//LCD
	//SPSR |= _BV(SPI2X);

	LCD_Init();
	LCD_PutStr_P(TXT_Hello, 112, 5, 0, BLACK, WHITE);
	LCD_PutStr_P(TXT_VERSION, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLACK, WHITE);

	//USART
	USART_Init((unsigned int)115200);

	//Thermocouple
	TC_Init();

	//RTC
	//DS1307_Init();

	USART_Puts_P(TXT_Hello);
	USART_Puts_P(TXT_VERSION);

	//Initialize runtime variables
	PID_Init();


	for (uint8_t i=0; i<20; i++) {
		_delay_ms(100);
	}
	LCD_Blank();
	Menu_Init();
	Menu_Process(0);

	BuzzerStart(50);

	wdt_enable(WDTO_2S);

	/* ****** MAIN LOOP ******* */

	while (1) {
		//process data received on serial port
		if (0!=buf_getcount((Tcircle_buffer *)&USART_buffer_RX)) {
			bajt = buf_getbyte((Tcircle_buffer *)&USART_buffer_RX);
			USART_Put(bajt);
			USART_StartSending();
			if ('a'==bajt) {
				USART_TransmitDecimal(TCNT1);
			}
			if ('s'==bajt) {
				USART_TransmitDecimal(TCNT2);
			}
			if ('d'==bajt) {
				USART_TransmitDecimal(OSCCAL); //181 was OK in my uC
			}
		}

		// control outputs
		if (tmp_out1) {
			OUT1_PORT &= ~_BV(OUT1);
		} else {
			OUT1_PORT |= _BV(OUT1);
		}
		if (tmp_out2) {
			OUT2_PORT &= ~_BV(OUT2);
		} else {
			OUT2_PORT |= _BV(OUT2);
		}

		/* ***** Every-1s tasks are here ***** */
		if (flag & _BV(FLAG_1S)) {
			flag &= ~_BV(FLAG_1S);

			// no error from termocouple converter
			if (0 == status) {
				/*  Display process value */
				LCD_PutStr_P(TXT_PV, 112, 5, 1, BLACK, WHITE);
				LCD_PutDecimalSigned(deg, 112, 35, 1, RED,WHITE);
				LCD_PutChar('.', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, RED,WHITE);
				LCD_PutDecimal(milideg, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, RED,WHITE);
				LCD_PutStr(" oC ", LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, RED,WHITE);

				/*  Display pump timer */
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
					output = (uint8_t)(PID_Process(floatpv));
					//output = (uint8_t)(PID_Process_3(floatpv));
				} else if (-1 == controller_param.k_r) {
					output = (uint8_t)controller.y;
				} else {
					output = 0;
				}
/* integer
				LCD_PutStr("PV:", 89,5,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.PV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",SP:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller_param.SV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",E:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.e, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				//LCD_PutStr(",OUT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,RED,WHITE);
				//LCD_PutDecimalSigned(output, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
				LCD_PutStr(",INT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.integral, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",DVDT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDecimalSigned(controller.derivative, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(" ", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
*/
/* floating point */
				LCD_PutStr("PV:", 89,5,0,BLUE,WHITE);
				LCD_PutDouble(controller.PV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
//				LCD_PutStr(",SP:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
//				LCD_PutDecimalSigned(controller_param.SV, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(",E:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDouble(controller.e, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				//LCD_PutStr(",OUT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,RED,WHITE);
				//LCD_PutDecimalSigned(output, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, RED,WHITE);
				LCD_PutStr(",INT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				LCD_PutDouble(controller.integral, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
//				LCD_PutStr(",DVDT:", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
//				LCD_PutDecimalSigned(controller.derivative, LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, BLUE,WHITE);
				LCD_PutStr(" ", LCD_AUTOINCREMENT,LCD_AUTOINCREMENT,0,BLUE,WHITE);
				//correct TC read END
			}
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
		} // end of every-1s section

		/* ***** Every 0.1s tasks here ****** */
		if (flag & _BV(FLAG_100MS)) {
			flag &= ~_BV(FLAG_100MS); //reset flag

			/* read TC data */
			if (0==(status=TC_PerformRead())) {
				if (0 != prevstatus) {
					LCD_Rectangle(110,0,8,132,WHITE);
					BuzzerStart(10);
				}
				TC_GetTCTemp(&deg, &milideg);
				//pv=deg*10+(milideg/10);

				/* low-pass filter using exponential moving average */
				if (0 == floatpv) {
					floatpv = deg+milideg/100.0;
				} else {
					floatpv = deg+milideg/100.0 + (controller_param.alpha * (floatpv-(deg+milideg/100.0)));
				}

				USART_TransmitDecimal(system_clock);
				USART_Put(',');
				USART_TransmitDouble(floatpv);
				USART_Put(',');
				USART_TransmitDouble(controller_param.k_r);
				USART_Put(',');
				USART_TransmitDouble(controller_param.T_i);
				USART_Put(',');
				USART_TransmitDouble(controller_param.T_d);
				USART_Put(',');
				USART_TransmitDouble(controller.proportional);
				USART_Put(',');
				USART_TransmitDouble(controller.integral);
				USART_Put(',');
				USART_TransmitDouble(controller.derivative);
				USART_Put(',');
				USART_Put(tmp_out1?'1':'0');
				USART_Put(',');
				USART_Put(tmp_out2?'1':'0');
				USART_Put(',');
				USART_Put(bit_is_clear(OUT3_PORT,OUT3)?'1':'0');
				USART_Put(',');
				USART_TransmitDecimal(in_flag);
				USART_Put(',');
				USART_TransmitDecimal(output);
				USART_Put('\r');
				USART_Put('\n');


			} else {
				// some error while TC reading has occured
				output = 0;
				if (0 == prevstatus) { //only on transition to erroneous state
					//clear controller section on LCD
					LCD_Rectangle(96, 0, (132-96), 132, WHITE);
					BuzzerStart(100);
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


			// input is on
			if (0 != in_flag) {
				in_flag--;
				pump_timer++;
				pump_timer_reset_timeout = controller_param.preinfusion_valve_off_delay; // PUMP_TIMER_RESET_TIMEOUT;
				tmp_out2 = 1; //enable valve during whole extraction process
				if (pump_timer == controller_param.preinfusion_time)
					BuzzerStart(15);
				if (pump_timer > controller_param.preinfusion_time) {
					//pre-infusion finished, full steam ahead!
					tmp_out1 = 1;
				} else {
					//preinfussion PWM
					tmp_out1 = ((pump_timer%10) > controller_param.preinfusion_duty_cycle)?0:1; //duty cycle must be in <0;10>
				}
			} else {
				tmp_out1 = 0; //disable pump
				// reset timer automatically after few seconds after switching pump switch off
				if (0 != pump_timer_reset_timeout) {
					pump_timer_reset_timeout--;
					// pump_timer_reset_timeout zeroed
					if (0 == pump_timer_reset_timeout) {
						pump_timer = 0;
						tmp_out2 = 0; //release valve
						BuzzerStart(50);
					}
				}
			}

			//Display system clock in lower left corner:
			//LCD_PutDecimal(system_clock, 0, 0, 0, BLACK, WHITE);
			//LCD_PutChar(' ', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 0, 0, WHITE);

			// status bar - inputs, outputs
			LCD_PutChar('1', 0, 96, 1, WHITE, tmp_out1?RED:GREEN);
			LCD_PutChar('2', 0, 105, 1, WHITE, tmp_out2?RED:GREEN);
			LCD_PutChar('3', 0, 114, 1, WHITE, bit_is_clear(OUT3_PORT,OUT3)?RED:GREEN);
			//LCD_PutChar('I', 0, 123, 1, WHITE, bit_is_clear(IN1_PIN,IN1)?RED:GREEN);
			LCD_PutChar('I', 0, 123, 1, WHITE, (in_flag)?RED:GREEN);

			LCD_Rectangle(0,64+16, 16,16, WHITE); //clear last 2 chars
			LCD_PutDecimal((output*100)/255, 0, 64, 1, BLACK, WHITE);
			LCD_PutChar('%', LCD_AUTOINCREMENT, LCD_AUTOINCREMENT, 1, BLACK, WHITE);


			//BUTTONS
			switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2));
			if (switch_status != (_BV(SW1)| _BV(SW3) | _BV(SW4) | _BV(SW2))) { //some button is pressed
				if (switch_status != prev_switch_status) { // key pressed
					_delay_ms(10); //debouncing - slightly blocking :)
					switch_status = ( SW1_PIN & (_BV(SW1)|_BV(SW3)|_BV(SW4))) | ( SW2_PIN & _BV(SW2)); //TODO

				} else { //end if key status changed
					repeat++;
					if (repeat<10) goto SKIP;
					repeated_flag = REPEATED_FLAG;  //button pressed for 1s
					if (repeat>30) {
						repeated_flag = REPEATED_FLAG | REPEATED_2X_FLAG; //button pressed for 3s
						repeat--; //do not increment to avoid overflow
					}
				}
				/* button ->  task mapping */
				if (bit_is_clear(switch_status, SW1)) { // && bit_is_set(prev_switch_status, SW1)) {
					Menu_Process(KEY_UP|repeated_flag);
					BuzzerStart(BUZZER_TIME);
				}
				if (bit_is_clear(switch_status, SW2)) { // && bit_is_set(prev_switch_status, SW2)) {
					Menu_Process(KEY_DOWN|repeated_flag);
					BuzzerStart(BUZZER_TIME);
				}
				if (bit_is_clear(switch_status, SW3) && bit_is_set(prev_switch_status, SW3)) {
					Menu_Process(KEY_LEFT);
					BuzzerStart(BUZZER_TIME);
				}
				if (bit_is_clear(switch_status, SW4)&& bit_is_set(prev_switch_status, SW4)) {
					Menu_Process(KEY_RIGHT);
					BuzzerStart(BUZZER_TIME);
				}

			} //buttons pressed - end
			else {
				repeat = 0;
				repeated_flag = 0;
			}
SKIP:
			prev_switch_status = switch_status;

			OCR2 = controller_param.lcd_brightness;
		} // end every 0.1s section

		//refresh watchdog timer
		wdt_reset();
	} //main loop
}	/* ******** main() */
