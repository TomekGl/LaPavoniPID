#include "includes.h"
#define LINE_WIDTH 16

const char DAYSOFWEEK[][3]  = { "Pn", "Wt", "Sr", "Cz", "Pt", "So", "Ni" };


void DS1307_start(void) {
	return ;
}
/*
void DS1307_start(void)
{
    uint8_t ret;
    ret = i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode
    if (ret) {
    	i2c_stop();
    	//return;
    } else {
    	i2c_write(0x00);	//adres sekund
    	i2c_rep_start(DS1307 + I2C_READ);
    	ret = i2c_readNak();
    	i2c_stop();
    }

    //if ((ret & 128)) {
    	//enable clock
    	i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode
	    i2c_write(0);
	    i2c_write(ret&127);
	    i2c_stop();
	    
	    //enable square out
	    i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode
	    i2c_write(0x07);		//adres
	    i2c_write(0x10);		//control reg
	    i2c_stop();
	    USART_Puts("DS1307 set ");

    //}
    return;
}


void ds1307_read(Tsystime *time)
{
		uint8_t i;
		
	    i2c_start(DS1307 + I2C_WRITE);
	    i2c_write(0x00);		//adres sekund
	    i2c_rep_start(DS1307 + I2C_READ);
	    
	    for (i = 0; i < 7; )
	    	{ 
	    	  time->chars[i++] = i2c_readAck(); 
	    	}
	    time->chars[i] = i2c_readNak();
	    i2c_stop();
}

void ds1307_set(Tds1307_offset offset, uint8_t val)
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
    i2c_write(ds1307_int2bcd(val));	//wartosc
    i2c_stop();

}

uint8_t ds1307_get(Tds1307_offset offset)
{
	uint8_t bcd=0;
    i2c_start(DS1307 + I2C_WRITE);	// set device address and write mode

    i2c_write(offset);		//adres sekund
    i2c_rep_start(DS1307 + I2C_READ);
    
    bcd = i2c_readNak();
    i2c_stop();
	
	return bcd;
}


uint8_t ds1307_bcd2int(uint8_t bcd)
{
	return ((bcd>>4)*10 + (bcd & 0x0f));
}

uint8_t ds1307_int2bcd(uint8_t val)
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
        for (i = 1; i < 3; i++) {
                str[idx[i]] = ((systime.chars[i] >> 4) + '0');
                str[idx[i] + 1] = ((systime.chars[i] & 0xf) + '0');
        }
        str[2]=(systime.chars[0]%2)?':':'.';

        strcpy(&(str[6]), DAYSOFWEEK[systime.systime.dayofweek]);
        str[8]=' ';
           str[16]=0;

           return;

       }
*/
