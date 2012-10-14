#ifndef MAIN_H_
#define MAIN_H_

#define TICKS_IN_SECOND 100

#define false 0
#define true 1

#define BUZZ_ON BUZZ_PORT |= _BV(BUZZ)
#define BUZZ_OFF BUZZ_PORT &= ~_BV(BUZZ)

/// czas wygaszania podswietlenia [sek]
#define LCD_TIMEOUT 2

/// pump timer is set to 0 after n ticks
#define PUMP_TIMER_RESET_TIMEOUT 50

#endif				/*MAIN_H_ */
