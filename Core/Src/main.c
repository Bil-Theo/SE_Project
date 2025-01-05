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
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "sdmmc.h"
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
#include "display_reg.h"

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
volatile uint16_t cmpt = 0, screen_pile = 0;

uint8_t sd_buffer[_MAX_SS]; /* Tampon de lecture/écriture pour SD */
const char *sensor_data = "Température: 25.3°C, Humidité: 40%\n";

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

void stop_Mode();
void start_again_timer(TIM_HandleTypeDef htim);

void SD_Init(void);
void SD_WriteData(const char *filename, const char *data);


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
  MX_DMA_Init();
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
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
///////////////////////////////////////





  HAL_Init();
  BSP_TS_Init(480, 272);
  ephemere_screen();
  HAL_Delay(5000);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  SD_Init();

  if(start_sensor_hts221()== -1) printf("Device for sensor hts221 not found!");
  if(start_sensor_lps22hh() == -1) printf("Device for sensor lps22hh not found!");

    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    sensors_screen();



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
	    SD_WriteData("sensors_data.csv", sensor_data);
  {// timer pour lattente avant mise en veille
	  //Teste projet et avancement
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
  RCC_OscInitStruct.PLL.PLLQ = 8;
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


void start_again_timer(TIM_HandleTypeDef htim){
	 htim.Instance->CNT = 0; // Réinitialise le
}





void stop_Mode(){
	HAL_GPIO_WritePin(user_led_GPIO_Port, user_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(blue_led_GPIO_Port, blue_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(red_led_GPIO_Port, red_led_Pin, GPIO_PIN_RESET);
}

void SD_Init(void) {
    if (f_mount(&SDFatFS, (TCHAR const *)SDPath, 0) != FR_OK) {
        printf("Erreur: Montage de la carte SD échoué.\n");
        Error_Handler();
    }
}

// Fonction pour écrire des données sur la carte SD
  void SD_WriteData(const char *filename, const char *data) {
      UINT byteswritten;
      FRESULT res;
      // fonction de debeugage
//      FRESULT res = f_open(&SDFile, "sensors_data.csv", FA_CREATE_ALWAYS | FA_WRITE);
//      if (res != FR_OK) {
//          switch (res) {
//              case FR_NO_FILESYSTEM:
//                  printf("Erreur: Pas de système de fichiers sur la carte SD.\n");
//                  break;
//              case FR_DISK_ERR:
//                  printf("Erreur: Problème matériel avec la carte SD.\n");
//                  break;
//              case FR_NOT_READY:
//                  printf("Erreur: La carte SD n'est pas prête.\n");
//                  break;
//              case FR_WRITE_PROTECTED:
//                  printf("Erreur: La carte SD est protégée en écriture.\n");
//                  break;
//              default:
//                  printf("Erreur FATFS: %d\n", res);
//                  break;
//          }
//          Error_Handler();
//      }


      // Ouvrir ou créer le fichier
      if (f_open(&SDFile, filename, FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
          printf("Erreur: Impossible d'ouvrir ou de créer le fichier.\n");
          Error_Handler();
      } else {
          // Écrire dans le fichier
          res = f_write(&SDFile, data, strlen(data), &byteswritten);
          if (res != FR_OK || byteswritten == 0) {
              printf("Erreur: Écriture échouée.\n");
              Error_Handler();
          } else {
              printf("Données écrites: %s\n", data);
          }

          // Fermer le fichier
          f_close(&SDFile);
      }
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
