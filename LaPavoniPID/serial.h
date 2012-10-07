#ifndef SERIAL_H_
#define SERIAL_H_
/** \defgroup serial Moduł obsługi portu szeregowego
 @{ */

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<PE)
#define DATA_OVERRUN (1<<DOR)

struct TUSART_errors_tag {
	uint8_t framing;
	uint8_t parity;
	uint8_t overrun;
} ;
typedef struct TUSART_errors_tag TUSART_errors;

/// errors counters
extern volatile TUSART_errors USART_errors;

/// circular buffer for received data
extern volatile Tcircle_buffer USART_buffer_RX;

/// circular buffer for transmited data
extern volatile Tcircle_buffer USART_buffer_TX;


///transmiter state
typedef enum {
	USART_STATE_IDLE, //nadajnik wolny
	USART_STATE_INTHEMIDDLE, //w trakcie nadawania
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

/** Wysłanie bajtu jako liczby binarnej w ASCII*/
void USART_TransmitBinary( unsigned char data );

/** Wysłanie liczby ze znakiem w ASCII*/
void USART_TransmitDecimalSigned(int32_t data);

/** @} */

/*! \file serial.h
    \brief Serial port support header file
*/
#endif /*SERIAL_H_*/
