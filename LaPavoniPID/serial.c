#include "includes.h"

/** \defgroup serial Moduł obsługi portu szeregowego
 @{ */

//inline uint8_t *buf_getptr(Tcircle_buffer *buffer)
//{
//	return &(buffer.data[0]);
//}

/// liczniki błędów
volatile TUSART_errors USART_errors;

/// bufor kołowy odbiorczy
volatile Tcircle_buffer USART_buffer_RX;
/// bufor kołowy nadawczy
volatile Tcircle_buffer USART_buffer_TX;
/// czas nadejścia ostatniego bajtu
volatile uint32_t USART_arrival_time;
/// czas nadejścia poprzedniego bajtu
volatile uint32_t USART_prev_arrival_time;

/// stan nadawania z buforaserial
volatile TUSART_state USART_state;


void USART_Init(unsigned int baud) {
	buf_init((Tcircle_buffer *)&USART_buffer_TX);
	buf_init((Tcircle_buffer *)&USART_buffer_RX);
	//USART_arrival_time = 0;
	//USART_prev_arrival_time = 0;

	USART_state = USART_STATE_IDLE;

	/* Set baud rate */
	UBRRL = 3;//(unsigned char)(F_CPU / (baud * 16L) - 1);
	UBRRH = 0;//(unsigned char)((F_CPU / (baud * 16L) - 1) >> 8);

	/* Enable receiver and transmitter, with interrupts */
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE) | (1 << TXCIE);

	/* Set frame format: 8data, 1stop bit, no parity -- this is default*/
	//UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

	//UCSRA = (1 << U2X);
}

/** Przerwanie odbiorcze USART */
ISR(USART_RXC_vect)
{
	volatile uint8_t status;

	//while (!(UCSRA & _BV(RXC)))
	//	;
	status = UCSRA;

	if (status & (FRAMING_ERROR|PARITY_ERROR|DATA_OVERRUN)) {
		if (status & (FRAMING_ERROR)) {
			USART_errors.framing++;
		}
		if (status & (PARITY_ERROR)) {
			USART_errors.parity++;
		}
		if (status & (DATA_OVERRUN)) {
			USART_errors.overrun++;
		}
		status = UDR; //discard data
		return;
	}

	//USART_prev_arrival_time = USART_arrival_time;
	//USART_arrival_time = system_clock;
	//if (buf_isfree((Tcircle_buffer *)&USART_buffer_RX))
	buf_putbyte((Tcircle_buffer *)&USART_buffer_RX, UDR);
	return;

}

void USART_StartSending() {

	if (USART_STATE_INTHEMIDDLE!=USART_state)
	{
		USART_TX_Byte();
	}
	//pozostałe bajty z bufora będą wysłane automatycznie
}

void USART_TX_Byte() {
		//sprawdzenie czy w buforze nadawczym są dane
		if (buf_getcount((Tcircle_buffer *)&USART_buffer_TX)) {
			/* Wait for empty transmit buffer */
			while (!(UCSRA & (1 << UDRE)))
				;

			//wysłanie pierwszego znaku
			USART_state = USART_STATE_INTHEMIDDLE;
			UDR = (buf_getbyte((Tcircle_buffer *)&USART_buffer_TX));
		} else
			USART_state = USART_STATE_IDLE;
	//pozostałe bajty z bufora będą wysłane automatycznie
}


/** Przerwanie od zakończenia nadawania */
ISR(USART_TXC_vect)
{ ///TODO USART_UDRE_vect
	//wysłanie następnego znaku jeżeli bufor zawiera dane
	USART_TX_Byte();
}



void USART_Puts(const char *s) {
	register char c;

	while ((c = *s++)) {
		buf_putbyte((Tcircle_buffer *)&USART_buffer_TX, c);
	}
	if (USART_STATE_INTHEMIDDLE!=USART_state)
	{
		USART_TX_Byte();
	}
}

void USART_Puts_P(const char *s) {
	register char c;

	while ((c = pgm_read_byte(s++))) {
		buf_putbyte((Tcircle_buffer *)&USART_buffer_TX, c);
	}
	if (USART_STATE_INTHEMIDDLE!=USART_state)
	{
		USART_TX_Byte();
	}
}

void USART_Put(char ch) {
	buf_putbyte((Tcircle_buffer *)&USART_buffer_TX, ch);
}

void USART_TransmitDecimal(uint32_t data) {
	char str[11];

	/* convert unsigned integer into string */
	ultoa(data, str, 10);

	USART_Puts(str);
}

void USART_TransmitDecimalSigned(int32_t data) {
	char str[11];

	/* convert signed integer into string */
	ltoa(data, str, 10);

	USART_Puts(str);
}


void USART_TransmitBinary(unsigned char data) {
	//ZAPIS BINARNY 0/1 Z _POMINIĘCIEM BUFORÓW_ !!!
	/* Wait for empty transmit buffer */
	uint8_t i;
	for (i = 0; i < 8; i++) {
		while (!(UCSRA & (1 << UDRE)))
			;

		if (data & (128 >> i))
			UDR = '1';
		else
			UDR = '0';

	}
}

void USART_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while (!(UCSRA & (1 << UDRE)))
		;
	/* Put data into buffer, sends the data */
	//PORTD |= _BV(RS485TXEN);
	UDR = data;
}


/** @} */

/*! \file serial.c
 \brief Moduł obsługi portu szeregowego
 */

