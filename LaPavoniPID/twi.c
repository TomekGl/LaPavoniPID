/*
 * twi.c
 *
 *  Created on: 12-03-2012
 *      Author: tomek
 */
#ifndef TWI_C_
#define TWI_C_

#include "includes.h"


void TWI_Init() {
	//set TWI bitrate
	TWBR = (F_CPU / 100000UL - 16) / 2;
}

uint8_t TWI_Start() {
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */

	while ((TWCR & _BV(TWINT)) == 0)
		; /* wait for transmission */
	switch (TW_STATUS) {
		case TW_REP_START:		/* OK, but should not happen */
		case TW_START:
			return 1;
		case TW_MT_ARB_LOST:
			USART_Puts("ArbLost!\n");
			return 0;
		default:
			USART_Puts("NotStartr!\n");
			return 0;		/* error: not in start condition */
				/* NB: do /not/ send stop condition */
	}
}

void TWI_Stop() {
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
}

uint8_t TWI_Write(uint8_t data) {
	  TWDR = data;
	  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	  return TW_STATUS;
}

uint8_t TWI_ReadN(uint8_t len, uint8_t *buf) {
	uint8_t twcr,twst;
	for (twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) /* Note [13] */;
			len > 0;
			len--)
	{
		if (len == 1)
			twcr = _BV(TWINT) | _BV(TWEN); /* send NAK this time */
		TWCR = twcr;		/* clear int to start transmission */
		while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
		switch ((twst = TW_STATUS))
		{
		case TW_MR_DATA_NACK:
			len = 0;		/* force end of loop */
			/* FALLTHROUGH */
		case TW_MR_DATA_ACK:
			*buf++ = TWDR;
			return 0;
		default:
			return 0;
		}
	}
	return 0;
}

#define MAX_ITER 10
int8_t TWI_Send (uint8_t address, uint8_t len, uint8_t *dataptr) {
	uint8_t twst=0, n=0, rv=0;

	restart:
	if (n++ >= MAX_ITER) {
		USART_Puts("Max_iter reached!\n");
		return -1;
	}

	begin:

	_delay_ms(10);

	if (TWI_Start()==0) {goto restart; }
	USART_Puts(" Addr\n");
	if (TWI_Write(DS1307|TW_WRITE)!=TW_MT_SLA_ACK) { goto restart; }
	USART_Puts(" MemAdr\n");
	if (TWI_Write(address)!=TW_MT_SLA_ACK) { goto restart; }
	USART_Puts(" Value:\n");
	if (TWI_Write(*dataptr)!=TW_MT_DATA_ACK) { goto error; } else {
		rv++;
	}

	quit:
	TWI_Stop();

	error:
	USART_Puts(" Written:");
	USART_TransmitDecimal(rv);
	return rv;
}

int8_t TWI_Recv (uint8_t address, uint8_t len, uint8_t *dataptr) {
	uint8_t twst=0, n=0, rv=0;

	restart:
	if (n++ >= MAX_ITER) {
		USART_Puts("Max_iter reached!\n");
		return -1;
	}

	begin:

	_delay_ms(10);

	if (TWI_Start()==0) {goto restart; }
	USART_Puts(" Addr\n");
	if (TWI_Write(DS1307|TW_WRITE)!=TW_MT_SLA_ACK) { goto restart; }
	USART_Puts(" MemAdr\n");
	if (TWI_Write(address)!=TW_MT_SLA_ACK) { goto restart; }
	if (TWI_Start()==0) {goto restart; } //rep start
	USART_Puts(" Read Value:\n");
	rv = TWI_ReadN(len, dataptr);

	quit:
	TWI_Stop();

	error:
	USART_Puts(" Read:");
	USART_TransmitDecimal(rv);
	return rv;
}


#endif /* TWI_C_ */
