/*
 * IRQ_Callback.c
 *
 *  Created on: Dec 4, 2024
 *      Author: Dell
 */

#include "stm32f7xx_hal.h"

extern volatile uint8_t Flag_tim4, Flag_tim7, Flag_btn, Flag_tim2;
void TouchScreen();

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(htim);

    Flag_tim4 = (htim->Instance == TIM4);
    Flag_tim7 = (htim->Instance == TIM7);
    Flag_tim2 = (htim->Instance == TIM2);
    if(htim->Instance == TIM3) TouchScreen();;
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin )
{
	UNUSED(GPIO_Pin) ;
	Flag_btn = 1;

	HAL_ResumeTick();
}
