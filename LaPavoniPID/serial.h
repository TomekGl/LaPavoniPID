#ifndef SERIAL_H_
#define SERIAL_H_
/**
 @file serial.h

 @defgroup serial AVR USART support module
 @code #include <serial.h> @endcode

 @brief AVR USART support module

 This library provides routines to interrupt-driven USART

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 06-03-2009

*/

/**@{*/

/// Framing error flag
#define FRAMING_ERROR (1<<FE)
/// Parity error flag
#define PARITY_ERROR (1<<PE)
/// Data overrun flag
#define DATA_OVERRUN (1<<DOR)

/// USART errors structure
struct TUSART_errors_tag {
	uint8_t framing; ///< framing errors counter
	uint8_t parity; ///< parity errors counter
	uint8_t overrun; ///< overrun errors counter
} ;
/// USART errors structure type definition
typedef struct TUSART_errors_tag TUSART_errors;

/// errors counters
extern volatile TUSART_errors USART_errors;

/// circular buffer for received data
extern volatile Tcircle_buffer USART_buffer_RX;

/// circular buffer for transmited data
extern volatile Tcircle_buffer USART_buffer_TX;


///transmiter state
typedef enum {
	USART_STATE_IDLE, //transmitter idle
	USART_STATE_INTHEMIDDLE, //data are being sent
	USART_STATE_FINISHED //zakonczono
} TUSART_state;


/** predefined common USART speeds */
//typedef enum TSpeed_tag TSpeed
//enum TSpeed_tag { S2400, S4800, S9600, S19200, S38400, S57600, S76800, S115200 }
#define GETUBRR(rate) ((F_CPU/(8*rate))-1)


/** Initialize USART and set baud speed */
void USART_Init(unsigned int baud );

/** Start sending data from transmit buffer*/
void USART_StartSending();

/** Send following byte from buffer*/
void USART_TX_Byte();


/** Send byte via USART */
void USART_Transmit( unsigned char data );

//unsigned char USART_Receive( void );

/** Send null-terminated string via USART */
void USART_Puts(const char *s);

/** Send single char via USART */
void USART_Put(char ch);

/** Send null-terminated string from flash via USART */
void USART_Puts_P(const char *s);

/** Send unsigned integer as a string via USART */
void USART_TransmitDecimal( uint32_t data );

/** Send byte value as binary string representation */
void USART_TransmitBinary( unsigned char data );

/** Send signed integer as decimal string representation */
void USART_TransmitDecimalSigned(int32_t data);

/** Send double as decimal string representation */
void USART_TransmitDouble(double data);

/** @} */

#endif /*SERIAL_H_*/
