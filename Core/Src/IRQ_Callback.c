/*
 * IRQ_Callback.c
 *
 *  Created on: Dec 4, 2024
 *      Author: Dell
 */

#include "stm32f7xx_hal.h"

extern volatile uint8_t Flag_tim4, Flag_tim7, Flag_btn, Flag_tim2,Flag_tim5,  screen_pile;
extern volatile uint32_t interrupt_count;
extern float tick_count;
void TouchScreen();
void detect_pluie();
void Get_Wind_Speed();
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(htim);
    Flag_tim4 = (htim->Instance == TIM4);
    Flag_tim7 = (htim->Instance == TIM7);
    Flag_tim2 = (htim->Instance == TIM2);
    //////// timer générale de temps d'acquisitions ici 5s
    Flag_tim5 = (htim->Instance == TIM5);
	if (Flag_tim5)
	{
		if(screen_pile == 0)show_sensors();
		else show_rain();
	}

    if(htim->Instance == TIM3) TouchScreen();
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin )
{
	UNUSED(GPIO_Pin) ;
	Flag_btn = 1;
	if (GPIO_Pin==GPIO_PIN_15)
	    {
		float tick_actuel = HAL_GetTick();
	      if ( (tick_actuel-0)>1000){ // si il y a au moins 1 s de delai entre les deux inetrruptions
	    	interrupt_count++; // Incrémenter le compteur d'interruptions
	        }

	    }
	HAL_ResumeTick();
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        // Incrémente le compteur de ticks à chaque capture
        tick_count++;


    }
}
