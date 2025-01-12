/*
 * IRQ_Callback.c
 *
 *  Created on: Dec 4, 2024
 *      Author: Dell
 *  Ce fichier gère toutes les routines d'interruptions
 */

#include "stm32f7xx_hal.h"

extern volatile uint8_t Flag_tim4, Flag_tim7, Flag_btn, Flag_tim2,Flag_tim5,  page;
extern volatile uint32_t interrupt_count;
extern float tick_count;
//Fonctions définis ailleurs et utilisées
void TouchScreen();
void detect_pluie();
void Get_Wind_Speed();

//Fonction qui gère les IRQ des timers
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(htim);
    Flag_tim4 = (htim->Instance == TIM4);
    Flag_tim7 = (htim->Instance == TIM7);
    Flag_tim2 = (htim->Instance == TIM2);
    //////// timer générale de temps d'acquisitions ici 5s
    Flag_tim5 = (htim->Instance == TIM5);

    if(htim->Instance == TIM3) TouchScreen(); //appellé le touchscreen chque 20ms
}

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin )
{
	/*Calcul le nombre d'interruptions  pour avoir la quantité de pluie
	 *
	 */
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
