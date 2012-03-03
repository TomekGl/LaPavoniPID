#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "includes.h"


//inline uint8_t *buf_getptr(Tcircle_buffer *buffer)
//{
//      return &(buffer.data[0]); 
//}

Tcircle_buffer *buf_init(Tcircle_buffer * buffer)
{
    buffer->write_ptr = 0;
    buffer->read_ptr = 0;
    buffer->count = 0;
    buffer->size = BUF_SIZE;
    return buffer;
}


uint8_t buf_getbyte(Tcircle_buffer * buffer)
{
    uint8_t byte;
    if (buffer->count > 0) {
	byte = buffer->data[buffer->read_ptr];
	buffer->read_ptr++;
	if (buffer->read_ptr > BUF_SIZE - 1)
	    buffer->read_ptr = 0;
	buffer->count--;
	return byte;
    } else
	return 0;
}

inline uint8_t buf_getcount(Tcircle_buffer * buffer)
{
    return buffer->count;
}

uint8_t buf_putbyte(Tcircle_buffer * buffer, uint8_t byte)
{
    if (buffer->count < BUF_SIZE) {
	buffer->data[buffer->write_ptr] = byte;
	buffer->write_ptr++;
	buffer->count++;
	if (buffer->write_ptr > BUF_SIZE - 1)
	    buffer->write_ptr = 0;
	return 0;
    } else
	return 1;
}


void USART_Init(unsigned int baud)
{
    /* Set baud rate */
    UBRRH = (unsigned char) (baud >> 8);
    UBRRL = (unsigned char) baud;
    /* Enable receiver and transmitter */

    UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
    /* Set frame format: 8data, 1stop bit */
    UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
    UCSRA = (1 << U2X);
}


/** Przerwanie odbiorcze USART 0 */
ISR(USART_RXC_vect)
{
    uint8_t status;

    status = UCSRA;
    if ((status & _BV(RXC)) == 0) {	/*czy aby napewno jest co odbierac */
	return;
    }
//    if (status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))
//    {
//      USART_Transmit('E');
//      USART_Transmit('R');
//      USART_Transmit('R');
//    }
    USART_arrival_time = system_clock;
    buf_putbyte(&USART_buffer_RX, UDR);
    return;

}


void USART_Puts(const char *s)
{
    register char c;

    while ((c = *s++)) {
	USART_Transmit(c);
    }
}

void USART_TransmitDecimal(uint32_t data)
{
    char str[11];

    /* convert interger into string */
    ultoa(data, str, 10);

    USART_Puts(str);
}

void USART_TransmitSignedDecimal(int32_t data)
{
    char str[11];

    /* convert interger into string */
    ltoa(data, str, 10);

    USART_Puts(str);


}


void USART_TransmitBinary(unsigned char data)
{
    /* Wait for empty transmit buffer */
    uint8_t i;
    for (i = 0; i < 8; i++) {
	while (!(UCSRA & (1 << UDRE)));
	if (data & (128 >> i))
	    UDR = '1';
	else
	    UDR = '0';

    }
}

void USART_Transmit(unsigned char data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSRA & (1 << UDRE)));
    /* Put data into buffer, sends the data */
    UDR = data;
}
