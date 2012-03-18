#include "includes.h"
#define LINE_WIDTH 16

/*
#define TWI_GEN_CALL         0x00  // The General Call address is 0

// Sample TWI transmission states, used in the main application.
#define SEND_DATA             0x01
#define REQUEST_DATA          0x02
#define READ_DATA_FROM_BUFFER 0x03*/

const char DAYSOFWEEK[][3]  = { "Pn", "Wt", "Sr", "Cz", "Pt", "So", "Ni" };
uint8_t messageBuf[8]; //sizeof(Tsystime)];
//uint8_t myCounter=0;
//uint8_t TWI_operation = 0;
/*uint8_t TWI_targetSlaveAddress   = 0xd0>>1;*/

/*
uint8_t TWI_Act_On_Failure_In_Last_Transmission ( uint8_t TWIerrorMsg )
{
                    // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
                    // and take appropriate actions.
                    // Se header file for a list of possible failures messages.

                    // Here is a simple sample, where if received a NACK on the slave address,
                    // then a retransmission will be initiated.

  if ( (TWIerrorMsg == TWI_MTX_ADR_NACK) | (TWIerrorMsg == TWI_MRX_ADR_NACK) )
    TWI_Start_Transceiver();
  //USART_Puts("TWI_Error:");
  //USART_TransmitDecimal(TWIerrorMsg);
  //USART_Puts("\n");
  return TWIerrorMsg;
}*/

/*void DS1307_Init() {
	TWI_statusReg = 1;
	TWI_Master_Initialise();
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
	messageBuf[1] = 0x0;             // Offset
	messageBuf[2] = 0x0;             // Seconds in BCD
	messageBuf[3] = 0x23;             // Seconds in BCD
	messageBuf[4] = 0x03;             // Seconds in BCD
	messageBuf[5] = 0x31;             // Seconds in BCD
	messageBuf[6] = 0x01;
	messageBuf[7] = 0x99;
	TWI_Start_Transceiver_With_Data( messageBuf, 3 );

}*/

/*
void DS1307_Reset() {
	messageBuf[0] = TWI_GEN_CALL;     // The first byte must always consit of General Call code or the TWI slave address.
	messageBuf[1] = 0xAA;             // The command or data to be included in the general call.
	TWI_Start_Transceiver_With_Data( messageBuf, 2 );
}
*/
/*
void DS1307_Read() {
	// Send the request-for-data command to the Slave
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
	messageBuf[1] = 0;             // The first byte is used for commands.
	TWI_Start_Transceiver_With_Data( messageBuf, 2 );

	TWI_operation = REQUEST_DATA;         // To release resources to other operations while waiting for the TWI to complete,
			// we set a operation mode and continue this command sequence in a "followup"
			// section further down in the code.

			// Get status from Transceiver and put it on PORTB
	USART_TransmitDecimal(TWI_Get_State_Info());
}
*/

/*
void DS1307_SquareEnable() {
	//square out, 1Hz
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
	messageBuf[1] = 0x07;             // The first byte is used for commands.
	messageBuf[2] = 0x10;                        // The second byte is used for the data.
	TWI_Start_Transceiver_With_Data( messageBuf, 3 );
}
*/

/*
void DS1307_Process() {
	if ( ! TWI_Transceiver_Busy() )
	{
		// Check if the last operation was successful
		if ( TWI_statusReg )
		{
			if ( TWI_operation ) // Section for follow-up operations.
			{
				// Determine what action to take now
				if (TWI_operation == REQUEST_DATA)
				{ // Request/collect the data from the Slave
				//	USART_Puts(" RQ ");
					messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
					messageBuf[1] = 0;
					TWI_Start_Transceiver_With_Data( messageBuf, 1 );
					TWI_operation = READ_DATA_FROM_BUFFER; // Set next operation
				}
				else if (TWI_operation == READ_DATA_FROM_BUFFER)
				{ // Get the received data from the transceiver buffer
				//	USART_Puts(" RD ");
					TWI_Get_Data_From_Transceiver( messageBuf, 7);
				//	USART_Put('.');
					for(uint8_t i=1; i<sizeof(Tsystime); i++) {
						systime.chars[i-1]=messageBuf[i];
				//		USART_TransmitDecimal( messageBuf[i]);        // Store data on PORTB.
				//		USART_Put('.');
					}
				//	USART_StartSending();
					TWI_operation = FALSE;        // Set next operation
				}
			}
		}
		else // Got an error during the last transmission
		{
			// Use TWI status information to detemine cause of failure and take appropriate actions.
			TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );
		}
	}
}
*/

void DS1307_Init(void)
{
	uint8_t ret;
	ret = i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode
	if (ret) {
		i2c_stop();
		return;
	} else {
		i2c_write(0x00);	//adres sekund
		i2c_rep_start(DS1307 + I2C_READ);
		ret = i2c_readNak();
		i2c_stop();
	}

	if ((ret & 128)) {
		//enable clock
		i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode
		i2c_write(0);
		i2c_stop();

		//enable square out
		i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode
		i2c_write(0x07);		//adres
		i2c_write(0x10);		//control reg
		i2c_stop();
		USART_Puts("DS1307 set ");

	}
	return;
}

void DS1307_Read()
{
	uint8_t i;

	i2c_start(DS1307 + I2C_WRITE);
	i2c_write(0x00);		//adres sekund
	i2c_rep_start(DS1307 + I2C_READ);

	for (i = 0; i < 6; )
	{
		systime.chars[i++] = i2c_readAck();
	}
	systime.chars[i] = i2c_readNak();
	i2c_stop();
}

void DS1307_set(Tds1307_offset offset, uint8_t val)
{
	switch(offset) {
	case DS1307_SEC:
	case DS1307_MIN:
		val=val%60;
		break;
	case DS1307_HOUR:
		val=val%24;
		break;
	case DS1307_DAYOFWEEK:
		val=val%7;
		break;
	case DS1307_DATE:
		val=val%31;
		break;
	case DS1307_MONTH:
		val=val%12;
		break;
	default:
		break;
	}

	i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode

	i2c_write(offset);	//adres
	i2c_write(DS1307_int2bcd(val));	//wartosc
	i2c_stop();

}

uint8_t DS1307_get(Tds1307_offset offset)
{
	uint8_t bcd=0;
	i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode

	i2c_write(offset);		//adres sekund
	i2c_rep_start(DS1307 + I2C_READ);

	bcd = i2c_readNak();
	i2c_stop();

	return bcd;
}


uint8_t DS1307_bcd2int(uint8_t bcd)
{
	return ((bcd>>4)*10 + (bcd & 0x0f));
}

uint8_t DS1307_int2bcd(uint8_t val)
{
	return (((val/10)<<4) + (val%10));
}


void prepare_timeline(char * str) {
	// "__:__:__ __/__/__ ";


	uint8_t i;

	//static const uint8_t idx[] = { 6, 3, 0, 19, 9, 12, 17 };
	static const uint8_t idx[] = { 6, 3, 0, 19, 9, 12, 17 };
	//uint8_t str[] = "\0\0:\0\0:\0\0 \0"; //X\0\0/\0\0/20\0\0 \0";

	memset(str, ' ', LINE_WIDTH);

	//SS HH MM
	for (i = 0; i < 3; i++) {
		str[idx[i]] = ((systime.chars[i] >> 4) + '0');
		str[idx[i] + 1] = ((systime.chars[i] & 0xf) + '0');
	}
	str[2]=':';
	str[5]=(systime.chars[0]%2)?':':'.';

	//strcpy(&(str[6]), DAYSOFWEEK[systime.systime.dayofweek]);
	//str[8]=' ';
	str[8]=0;

	return;

}

