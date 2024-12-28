/*
 * humidity.h
 *
 *  Created on: Dec 4, 2024
 *      Author: Dell
 */

#ifndef TEMPLATES_HUMIDITY_H_
#define TEMPLATES_HUMIDITY_H_

#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//Ecran include
//
#include "stm32f7xx_hal.h"
#include "hts221_reg.h"


#include <stdio.h>
#include <string.h>

#define SENSOR_BUS hi2c1

typedef struct {
  float x0;
  float y0;
  float x1;
  float y1;
} lin_t;

typedef struct{
	float hum;
	float temp;
}hum_temp_t;

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void platform_delay(uint32_t ms);


static float linear_interpolation(lin_t *lin, int16_t x);

uint8_t start_sensor_hts221();

void get_grandeur_values_sensor_hts221();

#endif /* TEMPLATES_HUMIDITY_H_ */
