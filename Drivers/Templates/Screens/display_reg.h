/*
 * display_reg.h
 *
 *  Created on: Dec 28, 2024
 *      Author: Dell
 */

#ifndef TEMPLATES_SCREENS_DISPLAY_REG_H_
#define TEMPLATES_SCREENS_DISPLAY_REG_H_

#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

#include "humidity.h"
#include "pression.h"



void show_sensors();
void setDrawText(uint16_t x, uint16_t y, const char *value);
void base_screen(char *title1, char *title2, char *title3);
void ephemere_screen();
void sensors_screen();
void raie_screen();
void DrawBlock(uint16_t x, uint16_t y, const char *title);
void error(uint8_t *message);
void TouchScreen();

#endif /* TEMPLATES_SCREENS_DISPLAY_REG_H_ */
