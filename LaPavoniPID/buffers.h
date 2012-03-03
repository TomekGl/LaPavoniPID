/*
 * buffers.h
 *
 *  Created on: 2009-03-06
 *      Author: tomek
 */

#ifndef BUFFERS_H_
#define BUFFERS_H_

/// wielkość bufora nadawczego i odbiorczego
#define BUF_SIZE 48

///definicja struktury bufora kołowego
struct Tcircle_buffer_tag
{
	///wskaznik do zapisu
	uint8_t write_ptr;
	///wskaznik do odczytu
	uint8_t read_ptr;
	///rozmiar
	uint8_t size;
	///liczba zapisanych bajtów
	uint8_t count;

	///bufor z danymi
	uint8_t data[BUF_SIZE];

};
typedef struct Tcircle_buffer_tag Tcircle_buffer;


/** Funkcje inicjalizacji buforów kołowych*/
Tcircle_buffer *buf_init(Tcircle_buffer *buffer);
/** Pobranie bajtu z bufora */
uint8_t buf_getbyte(Tcircle_buffer *buffer);
/** Wysłanie bajtu do bufora */
uint8_t buf_putbyte(Tcircle_buffer *buffer, uint8_t byte);
/** Pobranie liczby bajtów w buforze */
//inline
uint8_t buf_getcount(Tcircle_buffer *buffer);

/** Zwraca true jeżeli w buforze jest jeszcze miejsce **/
uint8_t buf_isfree(Tcircle_buffer *buffer);



#endif /* BUFFERS_H_ */
