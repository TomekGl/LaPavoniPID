#include "includes.h"

/** \defgroup serial USART support module
 @{ */

//inline uint8_t *buf_getptr(Tcircle_buffer *buffer)
//{
//	return &(buffer.data[0]);
//}

/// errors counters
volatile TUSART_errors USART_errors;

/// RX circular buffer
volatile Tcircle_buffer USART_buffer_RX;
/// TX circular buffer
volatile Tcircle_buffer USART_buffer_TX;
/// receive time of last byte
volatile uint32_t USART_arrival_time;
/// receive time of previous byte
volatile uint32_t USART_prev_arrival_time;

/// receiver state
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

/** USART Receive Interrupt*/
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
	//next and another remaining bytes will be sent automatically
}

void USART_TX_Byte() {
	    // Check transmit buffer for data to be sent
		if (buf_getcount((Tcircle_buffer *)&USART_buffer_TX)) {
			/* Wait for empty transmit buffer */
			while (!(UCSRA & (1 << UDRE)))
				;

			//sending of first character
			USART_state = USART_STATE_INTHEMIDDLE;
			UDR = (buf_getbyte((Tcircle_buffer *)&USART_buffer_TX));
		} else
			USART_state = USART_STATE_IDLE;
	//next and another remaining bytes will be sent automatically
}


/** Transmition completed interrupt */
ISR(USART_TXC_vect)
{ ///TODO USART_UDRE_vect
	//send next byte if exist
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
	///This function do not use TX buffers !!!
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

/** Send double as decimal string representation */
void USART_TransmitDouble(double data) {
	char str[12];

	/* convert signed integer into string */
	dtostre(data, str, 4, 0);

	USART_Puts(str);
	}

/** @} */

/*! \file serial.c
 \brief Serial port support
 */

