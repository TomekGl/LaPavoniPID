#ifndef DS1307_H_
#define DS1307_H_

/// I2C address
#define DS1307 0xd0

///offsety
typedef enum {
	DS1307_SEC,
	DS1307_MIN,
	DS1307_HOUR,
	DS1307_DAYOFWEEK,
	DS1307_DATE,
	DS1307_MONTH,
	DS1307_YEAR,
} Tds1307_offset;

///offsety
//const char DAYSOFWEEK[][]__attribute__ ((progmem)) = { "Pn", "Wt", "Sr", "Cz", "Pt", "So", "Ni" };


struct Tsystime_tag_1 { //BCD
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t date;
	uint8_t month;
	uint8_t year;
};

union Tsystime_tag {
	struct Tsystime_tag_1 systime;
	uint8_t chars[7];
};

//typedef struct Tsystime_tag_1 Tsystime_1;
typedef union Tsystime_tag Tsystime;

uint8_t DS1307_bcd2int(uint8_t);
uint8_t DS1307_int2bcd(uint8_t);


void DS1307_Init(void);
void DS1307_Reset(void);
void DS1307_Read(void);
void DS1307_Process(void);
//start clock if halted, set parameters
void DS1307_start(void);

//read time
void DS1307_read(Tsystime *time);

uint8_t DS1307_get(Tds1307_offset offset);
void DS1307_set(Tds1307_offset offset, uint8_t val);
void prepare_timeline(char * str);

#endif				/* DS1307_H_ */
