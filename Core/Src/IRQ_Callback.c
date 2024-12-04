/*
 * IRQ_Callback.c
 *
 *  Created on: Dec 4, 2024
 *      Author: Dell
 */

#include "stm32f7xx_hal.h"

extern volatile uint8_t Flag_tim4, Flag_tim7;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(htim);

    Flag_tim4 = (htim->Instance == TIM4);
    Flag_tim7 = (htim->Instance == TIM7);
}
