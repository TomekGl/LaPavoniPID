/*
 * plot.c
 *
 *  Created on: 04-03-2014
 *      Author: tomek
 */
#include "includes.h"

uint8_t x=0;
#define PLOT_X 0
#define PLOT_Y 16
#define PLOT_H 64
#define PLOT_W 128
uint8_t PlotInterval = 5;
uint8_t TemperatureHistory[PLOT_W];
uint8_t SetPointHistory[PLOT_W];
uint8_t FlowHistory[PLOT_W];

void PlotInit() {
	LCD_Rectangle(MENU_X_POS-16-8*MENU_ROWS,0,64,132,WHITE);
	x = 23;
}

#define PLOT_HIRES

#ifdef PLOT_HIRES
///map range 85-101 into 0-(PLOT_H-1)
inline uint8_t ScaleTemp(double temp) {
#define TEMP_MIN 85.0
#define TEMP_MAX 85.0+16.0
	if (temp < TEMP_MIN || temp>=TEMP_MAX)
		return 0; //NaN
	return (uint8_t)((temp - TEMP_MIN)*4.0);
}
#else
///map range 80-144 into 0-(PLOT_H-1)
inline uint8_t ScaleTemp(double temp) {
#define TEMP_MIN 80.0
#define TEMP_MAX 144.0
	if (temp < TEMP_MIN || temp>=TEMP_MAX)
		return 0; //NaN
	return (uint8_t)(temp - TEMP_MIN);
}
#endif

void PlotRefresh() {
	TemperatureHistory[x] = ScaleTemp(Temperature);
	FlowHistory[x] = (uint8_t)(controller.flow*10.0);
	SetPointHistory[x] = ScaleTemp(controller_param.SV);
	LCD_Rectangle(PLOT_Y, x, FlowHistory[x], 1, BLACK);
	LCD_Rectangle(PLOT_Y+SetPointHistory[x], x, 1, 2, GRAY);
	LCD_Rectangle(PLOT_Y+TemperatureHistory[x], x, 2, 1, RED);

	//horizontal rulers
#ifdef PLOT_HIRES
	LCD_Rectangle(PLOT_Y+ ScaleTemp(90.0), x, 1, 2, BLUE);
	LCD_Rectangle(PLOT_Y+ ScaleTemp(100.0), x, 1, 2, BLUE);
	//LCD_Rectangle(PLOT_Y+ ScaleTemp(120.0), x, 1, 2, BLUE);
#else
	LCD_Rectangle(PLOT_Y+ ScaleTemp(100.0), x, 1, 2, BLUE);
	LCD_Rectangle(PLOT_Y+ ScaleTemp(80.0), x, 1, 2, BLUE);
	LCD_Rectangle(PLOT_Y+ ScaleTemp(120.0), x, 1, 2, BLUE);
#endif

	x++;
	if (x==PLOT_W) x=0;
	LCD_Rectangle(PLOT_Y, x, PLOT_H, 1, (PlotInterval<5)?YELLOW:WHITE);
	LCD_Rectangle(PLOT_Y, x+1, PLOT_H, 1, BLACK);
	if (x==3*8) {
#ifdef PLOT_HIRES
		LCD_PutStr_P(PSTR("100"), PLOT_Y-4+ScaleTemp(100.0), 0, 0, WHITE, BLUE);
		LCD_PutStr_P(PSTR("90"), PLOT_Y-4+ScaleTemp(90.0), 0, 0, WHITE, BLUE);
#else
		LCD_PutStr_P(PSTR("120"), PLOT_Y-4+ScaleTemp(120.0), 0, 0, WHITE, BLUE);
		LCD_PutStr_P(PSTR("100"), PLOT_Y-4+ScaleTemp(100.0), 0, 0, WHITE, BLUE);
		LCD_PutStr_P(PSTR("80"), PLOT_Y-4+ScaleTemp(80.0), 0, 0, WHITE, BLUE);
#endif
	}

}
