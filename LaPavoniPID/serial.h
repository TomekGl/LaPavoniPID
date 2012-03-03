#ifndef SERIAL_H_
#define SERIAL_H_


#define BUF_SIZE 8

struct Tcircle_buffer_tag {
    ///wskaznik do zapisu
    uint8_t write_ptr;
    ///wskaznik do odczytu
    uint8_t read_ptr;
    ///rozmiar
    uint8_t size;
    ///liczba zapisanych bajt�w
    uint8_t count;

    ///bufor z danymi
    uint8_t data[BUF_SIZE];

};
typedef struct Tcircle_buffer_tag Tcircle_buffer;

//inline uint8_t *buf_getptr(Tcircle_buffer *buffer);
Tcircle_buffer *buf_init(Tcircle_buffer * buffer);
uint8_t buf_getbyte(Tcircle_buffer * buffer);
uint8_t buf_putbyte(Tcircle_buffer * buffer, uint8_t byte);
uint8_t buf_getcount(Tcircle_buffer * buffer);

void USART_Init(unsigned int baud);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void USART_Puts(const char *s);
void USART_TransmitDecimal(uint32_t data);
void USART_TransmitBinary(unsigned char data);


#endif				/*SERIAL_H_ */
