/*
 * max31855.h
 *
 *  Created on: 11-03-2012
 *      Author: tomek
 */

#ifndef MAX31855_H_
#define MAX31855_H_

#define TC_READOK 0
#define TC_READSCV 4
#define TC_READSCG 2
#define TC_READOC 1

void TC_init();

uint8_t TC_performRead();

void TC_getTCTemp(int16_t *deg, uint16_t *milideg);

void TC_getInternalTemp(int16_t *deg, uint16_t *milideg);

#endif /* MAX31855_H_ */
