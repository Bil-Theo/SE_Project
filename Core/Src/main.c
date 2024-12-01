
/**
  **********
  * @file           : main.c
  * @brief          : Main program body
  **********
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********
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

#include "hts221_reg.h"
#include "pression.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//**********definition du bus I2C1*******

#define SENSOR_BUS hi2c1


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//*****************************
static int16_t data_raw_humidity;
static int16_t data_raw_temperature;
static float humidity_perc;
static float temperature_degC;
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
//*****************************


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)



//**************************
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void platform_delay(uint32_t ms);
typedef struct {
  float x0;
  float y0;
  float x1;
  float y1;
} lin_t;

float linear_interpolation(lin_t *lin, int16_t x)
{
  return ((lin->y1 - lin->y0) * x + ((lin->x1 * lin->y0) -
                                     (lin->x0 * lin->y1)))
         / (lin->x1 - lin->x0);
}
//***************************



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
//********************************
	stmdev_ctx_t dev_ctx;
	  dev_ctx.write_reg = platform_write;
	  dev_ctx.read_reg = platform_read;
	  dev_ctx.mdelay = platform_delay;
	  dev_ctx.handle = &SENSOR_BUS;
	  //***************************



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
  /* USER CODE BEGIN 2 */

  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);



  // 2. Configuration de l'écran
      BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR); // Initialisation par défaut du layer 0
      BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER); // Sélection du layer actif
      BSP_LCD_Clear(LCD_COLOR_WHITE); // Effacer l'écran avec une couleur blanche

      // 3. Configuration des paramètres d'affichage
      BSP_LCD_SetFont(&Font24); // Définir la police (Font24 est une police par défaut)
      BSP_LCD_SetTextColor(LCD_COLOR_CYAN); // Couleur du texte en noir
      BSP_LCD_SetBackColor(LCD_COLOR_WHITE); // Couleur de fond en blanc

      // 4. Affichage du texte
      BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Hello Project", CENTER_MODE);

  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */




    //Temperature
      /* Check device ID */
      //**************************
        whoamI = 0;
        hts221_device_id_get(&dev_ctx, &whoamI);

        if ( whoamI != HTS221_ID ) printf("Device not found!!\r\n");
         // while (1); /*manage here device not found */
        /* Read humidity calibration coefficient */
          lin_t lin_hum;
          hts221_hum_adc_point_0_get(&dev_ctx, &lin_hum.x0);
          hts221_hum_rh_point_0_get(&dev_ctx, &lin_hum.y0);
          hts221_hum_adc_point_1_get(&dev_ctx, &lin_hum.x1);
          hts221_hum_rh_point_1_get(&dev_ctx, &lin_hum.y1);
          /* Read temperature calibration coefficient */
          lin_t lin_temp;
          hts221_temp_adc_point_0_get(&dev_ctx, &lin_temp.x0);
          hts221_temp_deg_point_0_get(&dev_ctx, &lin_temp.y0);
          hts221_temp_adc_point_1_get(&dev_ctx, &lin_temp.x1);
          hts221_temp_deg_point_1_get(&dev_ctx, &lin_temp.y1);
          /* Enable Block Data Update */
          hts221_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
          /* Set Output Data Rate */
          hts221_data_rate_set(&dev_ctx, HTS221_ODR_1Hz);
          /* Device power on */
          hts221_power_on_set(&dev_ctx, PROPERTY_ENABLE);
          //***************************





  HAL_GPIO_WritePin(user_led_GPIO_Port, user_led_Pin, GPIO_PIN_RESET);

  static float t;
  while (1)
  {

	  /* Read output only if new value is available */
	     hts221_reg_t reg;
	     hts221_status_get(&dev_ctx, &reg.status_reg);


	      t = lps22hh_read_data_polling();
	      snprintf((char *)tx_buffer, sizeof(tx_buffer), "pressure [hPa]:%6.2f\r\n", t);

	     //*****************************
	     if (reg.status_reg.h_da) {
	       /* Read humidity data */
	       memset(&data_raw_humidity, 0x00, sizeof(int16_t));
	       hts221_humidity_raw_get(&dev_ctx, &data_raw_humidity);
	       humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity);

	       if (humidity_perc < 0) {
	         humidity_perc = 0;
	       }

	       if (humidity_perc > 100) {
	         humidity_perc = 100;
	       }
	       printf("******\r\n");
	       printf((char *)tx_buffer);
	       snprintf((char *)tx_buffer, sizeof(tx_buffer), "Humidity [%%]:%3.2f\r\n", humidity_perc);
	       printf((char *)tx_buffer);
	     }

	     if (reg.status_reg.t_da) {
	       /* Read temperature data */
	       memset(&data_raw_temperature, 0x00, sizeof(int16_t));
	       hts221_temperature_raw_get(&dev_ctx, &data_raw_temperature);
	       temperature_degC = linear_interpolation(&lin_temp,
	                                               data_raw_temperature);
	       snprintf((char *)tx_buffer, sizeof(tx_buffer), "Temperature [degC]:%6.2f\r\n",
	               temperature_degC );
	       printf((char *) tx_buffer);
	     }
	     //**************************
	     HAL_GPIO_TogglePin(user_led_GPIO_Port,user_led_Pin);

	   	  HAL_Delay(250);
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

//Temperature
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
	 /* Write multiple command */
	  reg |= 0x80;
	  HAL_I2C_Mem_Write(handle, HTS221_I2C_ADDRESS, reg,
	                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
	  return 0;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
	reg |= 0x80;
	  HAL_I2C_Mem_Read(handle, HTS221_I2C_ADDRESS, reg,
	                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	  return 0;

}

static void platform_delay(uint32_t ms)
{
  HAL_Delay(ms);

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
