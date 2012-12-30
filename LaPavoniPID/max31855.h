#ifndef MAX31855_H_
#define MAX31855_H_
/**
 @file max31855.h

 @defgroup max31855 MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter driver

 @code #include <max31855.h> @endcode

 @brief MAX31855 Cold-Junction Compensated Thermocouple-to-Digital Converter driver

 This library provides routines to comunicate with MAX31855 via hardware SPI

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 11-03-2012
*/

/**@{*/


/// Last reading status
typedef enum {
	TC_READOK = 0, ///< reading correct
	TC_READOC = 1, ///< reading incorrect - thermocouple connector open
	TC_READSCG = 2, ///< reading incorrect - thermocouple short to ground
	TC_READSCV = 4 ///< reading incorrect - thermocouple short to VCC
} TTC_read_status;

/// RAW data received from chip
union MAX31855Data {
	uint32_t integer; ///< as integer
	int16_t word[2]; ///< as 2 words
	uint8_t byte[4]; ///< as 4 bytes
};


/**
 * Initialize communication with converter connected via SPI
 */
void TC_init();

/*
 * Debug, print raw data on serial port
 */
/* void TC_debug(); */


/**
 * Acquire data from thermocouple converter
 * @return TTC_read_status structure with RAW values and error flags
 */
TTC_read_status TC_performRead();

/**
 * Convert previously acquired data of TC temperature to Celsius degrees
 * @param *deg Pointer to integer part of value
 * @param *milideg Pointer to fractional part of value
 */
void TC_getTCTemp(int16_t *deg, uint16_t *milideg);

/**
 * Convert previously acquired data of cold junction to temperature in Celsius degrees
 * @param *deg Pointer to integer part of value
 * @param *milideg Pointer to fractional part of value
 */
void TC_getInternalTemp(int16_t *deg, uint16_t *milideg);

/** @} */


#endif /* MAX31855_H_ */
