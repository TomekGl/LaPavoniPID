/*
 * twi.h
 *
 *  Created on: 12-03-2012
 *      Author: tomek
 */

#ifndef TWI_H_
#define TWI_H_

#define I2C_WRITE   0
#define I2C_READ    1

int8_t TWI_Recv (uint8_t address, uint8_t len, uint8_t *dataptr);
int8_t TWI_Send (uint8_t address, uint8_t len, uint8_t *dataptr);
#endif /* TWI_H_ */
