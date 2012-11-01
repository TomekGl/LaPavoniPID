/*! \file max31855.h
    \brief MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter driver header file

    This module provides support for MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter. It uses hardware SPI to make readings.
*/

/*


 * max31855.h
 *
 *  Created on: 11-03-2012
 *      Author: tomek
 */

#ifndef MAX31855_H_
#define MAX31855_H_
/** \defgroup max31855 MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter driver
 @{ */

/// Last reading status
typedef enum {
	/// reading correct
	TC_READOK = 0,
	/// reading incorrect - thermocouple connector open
	TC_READOC = 1,
	/// reading incorrect - thermocouple short to ground
	TC_READSCG = 2,
	/// reading incorrect - thermocouple short to VCC
	TC_READSCV = 4
} TTC_read_status;

/// Converter RAW data structure
union MAX31855Data {
	uint32_t integer;
	int16_t word[2];
	uint8_t byte[4];
};


/**
 * Initialize communication with converter connected via SPI
 */
void TC_init();

/**
 * Acquire data from thermocouple converter
 * @return TTC_read_status structure with RAW values and error flags
 */
TTC_read_status TC_performRead();

/**
 * Convert previously acquired data of TC temperature to Celsius degrees
 * @param deg Integer part of value
 * @param milideg Fractional part of value
 */
void TC_getTCTemp(int16_t *deg, uint16_t *milideg);

/**
 * Convert previously acquired data of cold junction to temperature in Celsius degrees
 * @param deg Integer part of value
 * @param milideg Fractional part of value
 */
void TC_getInternalTemp(int16_t *deg, uint16_t *milideg);

/** @} */


#endif /* MAX31855_H_ */
