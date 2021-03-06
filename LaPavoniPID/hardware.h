#ifndef HARDWARE_H_
#define HARDWARE_H_

#ifndef F_CPU
#define F_CPU 7372800UL
#endif

#define BUZZ_PORT PORTD
#define BUZZ PD5
#define BUZZ_DDR DDRD
#define BUZZ_PIN PIND

#define SQW_PIN PIND
#define SQW PD3
#define SQW_DDR DDRD
#define SQW_PORT PORTD

#define SW1_PIN PINC
#define SW1 PC3
#define SW1_DDR DDRC
#define SW1_PORT PORTC

#define SW2_PIN PIND
#define SW2 PD6
#define SW2_DDR DDRD
#define SW2_PORT PORTD

#define SW3_PIN PINC
#define SW3 PC2
#define SW3_DDR DDRC
#define SW3_PORT PORTC

#define SW4_PIN PINC
#define SW4 PC4
#define SW4_DDR DDRC
#define SW4_PORT PORTC

#define IN1_PIN PIND
#define IN1 PD3
#define IN1_DDR DDRD
#define IN1_PORT PORTD

#define OUT1 PC7
#define OUT1_DDR DDRC
#define OUT1_PORT PORTC

#define OUT2 PC6
#define OUT2_DDR DDRC
#define OUT2_PORT PORTC

#define OUT3 PC5
#define OUT3_DDR DDRC
#define OUT3_PORT PORTC

#define ADC_CS PB4
#define ADC_CSPORT PORTB
#define ADC_CSDDR DDRB

#define SPI_SCK PB7
#define SPI_MISO PB6
#define SPI_MOSI PB5
#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_PIN PINB

#define I2C_SDA PC1
#define I2C_SCK PC0
#define I2C_PIN PINC
#define I2C_DDR DDRC
#define I2C_PORT PORTC

#define FM1_PIN PINB
#define FM1 PB2
#define FM1_DDR DDRB
#define FM1_PORT PORTB

#define FM2_PIN PINB
#define FM2 PB3
#define FM2_DDR DDRB
#define FM2_PORT PORTB


#endif /*HARDWARE_H_*/
