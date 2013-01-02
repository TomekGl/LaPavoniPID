#ifndef BUFFERS_H_
#define BUFFERS_H_

/**
 @file buffers.h

 @defgroup buffers Circular buffers library
 @code #include <buffers.h> @endcode

 @brief Circular buffers library

 This library provides routines to operate on constant size circullar buffers.
 @note  Size of buffer is constant, set at compile time.

 @author Tomasz Głuch contact+avr@tomaszgluch.pl http://tomaszgluch.pl/
 @date 06-03-2009
 @par Usage Example
  The following code shows typical usage of this library

 @code

 #include <buffers.h>

 // RX circular buffer
 volatile Tcircle_buffer USART_buffer_RX;
 // TX circular buffer
 volatile Tcircle_buffer USART_buffer_RX;

 int main(void)
 {
   	buf_init((Tcircle_buffer *)&USART_buffer_RX);
   	buf_init((Tcircle_buffer *)&USART_buffer_TX);

    //copy data from ones to another
   	while (buf_getcount((Tcircle_buffer *)&USART_buffer_RX)) {
   	  buf_putbyte((Tcircle_buffer *)&USART_buffer_TX,
   	   	   	   	   buf_getbyte((Tcircle_buffer *)&USART_buffer_RX);
   	}

 }
 @endcode

*/

/**@{*/


/// buffer size in bytes,
#define BUF_SIZE 80

///circullar buffer structure definition
struct Tcircle_buffer_tag
{
	///write pointer
	uint8_t write_ptr;
	///read pointer
	uint8_t read_ptr;
	///size
	uint8_t size;
	///number of bytes kept in buffer
	uint8_t count;

	///place for data
	uint8_t data[BUF_SIZE];

};

///circullar buffer structure type definition
typedef struct Tcircle_buffer_tag Tcircle_buffer;


//!  Initializes structure
/**
      @param buffer pointer to buffer structure
      @return Buffer structure
*/
Tcircle_buffer *buf_init(Tcircle_buffer *buffer);


//!   Return next byte from buffer
/**	  Before reading check if there is any data to read with buf_getcount(). Calling function on empty buffer returns 0.
      @param buffer pointer to buffer structure
      @return Read byte
*/
uint8_t buf_getbyte(Tcircle_buffer *buffer);


//! Put byte into buffer
/**
      @param buffer pointer to buffer structure
      @param byte byte to be written
      @return 1 on success, 0 on fail
*/
uint8_t buf_putbyte(Tcircle_buffer *buffer, uint8_t byte);

//! Return number of bytes kept in buffer
/**
      @param buffer pointer to buffer structure
      @return Number of bytes kept in buffer
 */
uint8_t buf_getcount(Tcircle_buffer *buffer);

//! Returns false if buffer is full
/**
      @param buffer pointer to buffer structure
      @return 1 if not full, 0 if full
 */
uint8_t buf_isfree(Tcircle_buffer *buffer);

/**@}*/
#endif /* BUFFERS_H_ */
