/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
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
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
//
#include "stm32f7xx_hal.h"


#include <stdio.h>
#include <string.h>

#include "humidity.h"
#include "pression.h"
#include "next.h"
#include "meteo_reg.h"
#include "fond.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BLOCK_WIDTH 200
#define BLOCK_HEIGHT 100
#define BLOCK_PADDING 20

#define BUTTON_WIDTH_fl 100
#define BUTTON_HEIGHT_fl 50

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

extern volatile float pressure_hPa;
extern volatile hum_temp_t grandeur;
volatile uint8_t Flag_tim4 = 0, Flag_tim7 = 0, Flag_btn = 0, Flag_tim2 = 0;
static uint8_t tx_buffer[1000];
static uint16_t cmpt = 0, screen_pile = 0;

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
void DrawBlock(uint16_t x, uint16_t y, const char *title);
void setDrawText(uint16_t x, uint16_t y, const char *value);
void TouchScreen();
void show_sensors();
void stop_Mode();
void start_again_timer(TIM_HandleTypeDef htim);
void ephemere_screen();
void base_screen(char *title1, char *title2, char *title3);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM7_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  BSP_TS_Init(800, 400);
  ephemere_screen();
  HAL_Delay(5000);
  BSP_LCD_Clear(LCD_COLOR_WHITE);


  if(start_sensor_hts221()== -1) printf("Device for sensor hts221 not found!");
  if(start_sensor_lps22hh() == -1) printf("Device for sensor lps22hh not found!");

    HAL_Init();
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
  	base_screen("Temperature", "Humidité", "Pression");
	BSP_LCD_DrawBitmap(380, 180, (uint8_t*)btn_next_bmp);
	show_sensors();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_TIM_Base_Init(&htim4) ;
  HAL_TIM_Base_Init(&htim3) ;
  HAL_TIM_Base_Init(&htim2) ;

  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim2);



  HAL_GPIO_WritePin(user_led_GPIO_Port, user_led_Pin, GPIO_PIN_RESET);
int i =0;
  while (1)
  {// timer pour lattente avant mise en veille
	  if(Flag_tim2 == 1){
		  BSP_LCD_DisplayOff();
		  /*
		  HAL_SuspendTick();
		  HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
		  SystemClock_Config();
		  HAL_ResumeTick();*/

		  Flag_tim2 =  0;
	  }
	  else if(Flag_tim4 == 1){
		   if(screen_pile == 0) show_sensors();
		   Flag_tim4 = 0;
	  }
	  else if(Flag_tim7 == 1){
		  HAL_GPIO_TogglePin(user_led_GPIO_Port, user_led_Pin);
		  Flag_tim7 = 0;
		  i++;
		  printf("je m'alume %d \r\n", i);
	  }
	  else if(Flag_btn == 1){
		  start_again_timer(htim2);
		  BSP_LCD_DisplayOn();
		  Flag_btn = 0;
	  }
	  else{
		  HAL_SuspendTick();
		  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
		  HAL_ResumeTick();
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void TouchScreen(){
	TS_StateTypeDef ts = {0};                  // Zero-initialize the current state to ensure no uninitialized data
	    static TS_StateTypeDef prev_state = {0};
	    static uint8_t is_touching = 0;           // Track if a touch is currently active

	    BSP_TS_GetState(&ts);

	    if (ts.touchDetected) {
	        // If a touch is detected and no touch was previously active
	    	start_again_timer(htim2);
	        if (!is_touching) {
	            is_touching = 1;  // Mark as touching
	            if ((ts.touchX[0] >= 380 && ts.touchX[0] <= 380 + 60)
	            		&& (ts.touchY[0] >= 180 && ts.touchY[0]<= 180 + 50)) {
	                cmpt++;
	                //printf("Touche ecran ici %d \r\n", cmpt);
	                base_screen("Pluie", "Vent", "Transfert");
	            	BSP_LCD_DrawBitmap(380, 180, (uint8_t*)btn_next_bmp);
	            	if(screen_pile == 0){
	            		base_screen("Pluie", "Vent", "Transfert");
	            	    BSP_LCD_DrawBitmap(380, 180, (uint8_t*)btn_next_bmp);

	            	    screen_pile = 1;
	            	}
	            	else{
	            		base_screen("Temperature", "Humidite", "Pression");
	            	    BSP_LCD_DrawBitmap(380, 180, (uint8_t*)btn_next_bmp);
	            	    show_sensors();
	            	    screen_pile = 0;
	            	}

	            }
	        }
	        // Update previous state
	        prev_state.touchX[0] = ts.touchX[0];
	        prev_state.touchY[0] = ts.touchY[0];
	    } else {
	        // If no touch is detected, reset the touch state
	        is_touching = 0;
	    }
}

void start_again_timer(TIM_HandleTypeDef htim){
	 htim.Instance->CNT = 0; // Réinitialise le
}

void DrawBlock(uint16_t x, uint16_t y, const char *title) {
    // Dessiner le rectangle
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawRect(x, y, BLOCK_WIDTH, BLOCK_HEIGHT);

    // Remplir le fond (optionnel, ici blanc)
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(x + 1, y + 1, BLOCK_WIDTH - 2, BLOCK_HEIGHT - 2);

    // Afficher le titre en gras
    //BSP_LCD_DrawBitmap(0, 0, (uint8_t*)fond_bmp);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(x + 10, y + 10, (uint8_t *)title, LEFT_MODE);


}

void setDrawText(uint16_t x, uint16_t y, const char *value){
	  // Afficher la valeur
	    BSP_LCD_SetFont(&Font16);
	    BSP_LCD_DisplayStringAt(x + 10, y + 40, (uint8_t *)value, LEFT_MODE);
}

void base_screen(char *title1, char *title2, char *title3){
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  // Dessiner les trois blocs
	DrawBlock(20, 20, title1);
	DrawBlock(20 + BLOCK_WIDTH + BLOCK_PADDING, 20, title2);
	DrawBlock(20, 20 + BLOCK_HEIGHT + BLOCK_PADDING, title3);
}

void stop_Mode(){
	HAL_GPIO_WritePin(user_led_GPIO_Port, user_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(blue_led_GPIO_Port, blue_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(red_led_GPIO_Port, red_led_Pin, GPIO_PIN_RESET);
}

void show_sensors(){
	  get_values_pressure_sensor_lps22hh();
	  get_grandeur_values_sensor_hts221();

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f[degC]",
			   grandeur.temp, '\xB0');
	   setDrawText(20, 20, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "%3.2f[%c]", grandeur.hum, 37);
	   setDrawText(20 + BLOCK_WIDTH + BLOCK_PADDING, 20, (char *)tx_buffer);

	   snprintf((char *)tx_buffer, sizeof(tx_buffer),"%6.2f[hPa]", pressure_hPa);
	   setDrawText(20, 20 + BLOCK_HEIGHT + BLOCK_PADDING, (char *)tx_buffer);
}


void ephemere_screen(){
	 BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
	BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
	BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_DrawBitmap(130, 60, (uint8_t*)meteo_bmp);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
