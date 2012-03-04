#ifndef SERIAL_H_
#define SERIAL_H_
/** \defgroup serial Moduł obsługi portu szeregowego
 @{ */

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<PE)
#define DATA_OVERRUN (1<<DOR)

/// bufor kołowy odbiorczy
extern volatile Tcircle_buffer USART_buffer_RX;
/// bufor kołowy nadawczy
extern volatile Tcircle_buffer USART_buffer_TX;


///typ wyliczeniowy stanu nadawania
typedef enum {
	USART_STATE_IDLE, //nadajnik wolny
	USART_STATE_INTHEMIDDLE, //w trakcie nadawania
	USART_STATE_FINISHED //zakonczono
} TUSART_state;


/** dostępne prędkości portu */
//typedef enum TSpeed_tag TSpeed
//enum TSpeed_tag { S2400, S4800, S9600, S19200, S38400, S57600, S76800, S115200 }
#define GETUBRR(rate) ((F_CPU/(8*rate))-1)


/** Inicjalizacja USART z zadaną szybkością */
void USART_Init( unsigned int baud );

/** Rozpoczęcie wysyłania z bufora */
void USART_StartSending();

/** Nadanie kolejnego znaku z bufora */
void USART_TX_Byte();


/** Wysłanie znaku przez USART*/
void USART_Transmit( unsigned char data );

//unsigned char USART_Receive( void );

/** Wysłanie ciągu znaków przez USART*/
void USART_Puts(const char *s);

/** Wysłanie pojedynczego znaku przez USART */
void USART_Put(char ch);

/** Wysłanie ciągu znaków z pamięci programu przez USART*/
void USART_Puts_P(const char *s);

/** Wysłanie liczby w ASCII*/
void USART_TransmitDecimal( uint32_t data );

/** Wysłanie bajtu jako liczby binarnej w ASCII*/
void USART_TransmitBinary( unsigned char data );


/** @} */

/*! \file serial.h
    \brief Plik nagłówkowy dla modułu obsługi portu szeregowego
*/
#endif /*SERIAL_H_*/
