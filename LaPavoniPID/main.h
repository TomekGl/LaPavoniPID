#ifndef MAIN_H_
#define MAIN_H_


#define TICKS_IN_SECOND 100

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

#define false 0
#define true 1

#define DS2482 0x30

#define OW_DRST 0xf0
#define OW_WCFG 0Xd2
#define OW_SRP 0xe1
#define OW_RS 0xb4
#define OW_WB 0xa5
#define OW_RB 0x96
#define OW_SB 0x87
#define OW_WT 0x78

#define OW_1WB 0

/// czas wygaszania podswietlenia [sek]
#define LCD_TIMEOUT 2

#endif				/*MAIN_H_ */
