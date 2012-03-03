/*
 * buffers.c
 *
 *  Created on: 2009-03-06
 *      Author: tomek
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdlib.h>

#include "includes.h"


Tcircle_buffer *buf_init(Tcircle_buffer *buffer)
{
	buffer->write_ptr =	0;
	buffer->read_ptr = 0;
	buffer->count = 0;
	buffer->size = BUF_SIZE;
	return buffer;
}

uint8_t buf_getbyte(Tcircle_buffer *buffer)
{
	uint8_t byte;
	if (buffer->count > 0) {
		cli();
		byte = buffer->data[buffer->read_ptr];
		buffer->read_ptr++;
		if(buffer->read_ptr >	(BUF_SIZE-1))
			buffer->read_ptr =	0;
		buffer->count--;
		sei();
		return byte;
	}
	else
		return 0;
}

//inline
uint8_t buf_getcount(Tcircle_buffer *buffer)
{
	return buffer->count ;
}

uint8_t buf_isfree(Tcircle_buffer *buffer)
{
	if (buffer->count < BUF_SIZE)
			return true;
		else
			return false;
}

uint8_t buf_putbyte(Tcircle_buffer *buffer, uint8_t byte)
{
	if (buffer->count < BUF_SIZE) {
	cli();
	buffer->data[buffer->write_ptr] = byte;
	buffer->write_ptr++;
	buffer->count++;
	if(buffer->write_ptr >	(BUF_SIZE-1))
		buffer->write_ptr =	0;
	sei();
	return true;
	}
	else
	return false;
}
