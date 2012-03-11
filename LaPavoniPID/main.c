#include "includes.h"

/// data i czas kompilacji
const char VERSION[] __attribute__ ((progmem)) = __DATE__ " " __TIME__;

#define ADCCS0 cbi(ADC_CSPORT,ADC_CS);
#define ADCCS1 sbi(ADC_CSPORT,ADC_CS);
#define CLK0 cbi(LCD_SPIPORT,LCD_SCK);
#define CLK1 sbi(LCD_SPIPORT,LCD_SCK);

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

	//Buzzer
	BUZZ_DDR |= _BV(BUZZ);
	//BUZZ_PORT |= _BV(BUZZ);

	//SS as output
	SPI_PORT |= _BV(SPI_MISO); //pullup
	ADC_CSDDR |= _BV(ADC_CS);
	ADC_CSPORT |= _BV(ADC_CS); //High - inactive
	SPI_DDR |= _BV(SPI_MOSI) | _BV(SPI_SCK);

	USART_Init(38400);
	//enable interrupts
	sei();

	//LCD
	LCD_Init();
	LCD_Test();

	USART_Puts_P(VERSION);

	uint8_t j=0, c=0;

	volatile uint8_t status;
	//GLOWNA PETLA ************************************************************
	uint8_t bajt;
	for (;;) {
    	//LCD_Rectangle(10,50,16,8, YELLOW);
    	if (0!=buf_getcount(&USART_buffer_RX)) {
    		bajt = buf_getbyte(&USART_buffer_RX);
    		USART_Put(bajt);

    		if ('A'==bajt) {
    			SPCR |= _BV(SPE) | _BV(MSTR)| _BV(SPR0) | _BV(SPR1); // Enable Hardware SPI
    			ADCCS0
    			_delay_us(1);
    			for (j=0; j<4; j++) {
    				SPDR = 0xff; // send data
    				while(!(SPSR & (1<<SPIF)))// wait until send complete
    					;
    				USART_Put(SPDR);
    			}
    			SPCR &= ~_BV(SPE);
    			ADCCS1
       		}
    		if ('Z'==bajt) {
//-----------
    			CLK0
    			ADCCS0
    			_delay_ms(1);
    		  for (j=0; j<16; j++) {
    			CLK1
    			_delay_ms(1);
    			if (SPI_PIN & _BV(SPI_MISO))
    				{ USART_Put('1'); }
    			else
    				{ USART_Put('0'); }
    			CLK0
    			if (SPI_PIN & _BV(SPI_MISO))
    			    				{ USART_Put('_'); }
    			    			else
    			    				{ USART_Put('.'); }
    			_delay_ms(1);
    		    }
    		  ADCCS1

//**-----------

    		}
    		USART_StartSending();
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
	}



}				/* main() */
