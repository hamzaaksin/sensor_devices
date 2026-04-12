/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mpu9250.h"
#include "median_filter.h"
#include "circular_buffer.h"
#include "stdio.h"
#include "string.h"   

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c1_rx;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */
uint8_t TX_Buffer[2];
uint8_t data[14];
float accel_data[3];
float gyro_data[3];
volatile uint8_t i2c_ready_flag = 1;
MedianFilter_t f_ax, f_ay, f_az;
MedianFilter_t f_gx, f_gy, f_gz;
struct buf_handle_t ax_stats_buf = { 0 };
char msg[150];
struct buf_handle_t buf_ax, buf_ay, buf_az;
struct buf_handle_t buf_gx, buf_gy, buf_gz;
volatile uint8_t timer_1hz_flag = 0;
uint32_t last_report_time = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART5_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_I2C1_Init();
	MX_UART5_Init();
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */

	uint8_t wake_up_data = 0x00;

	if (HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1, 1, &wake_up_data, 1,
			100) != HAL_OK) {

		Error_Handler();
	}
	HAL_Delay(100);
	HAL_TIM_Base_Start_IT(&htim1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		if (timer_1hz_flag == 1) {
			timer_1hz_flag = 0; 

			HAL_UART_Transmit(&huart5,
					(uint8_t*) "\r\n--- [1Hz SENSOR REPORT] ---\r\n", 31, 100);

			Send_Sensor_Stats(&buf_ax, "ACC X");
			Send_Sensor_Stats(&buf_ay, "ACC Y");
			Send_Sensor_Stats(&buf_az, "ACC Z");

			HAL_UART_Transmit(&huart5,
					(uint8_t*) "----------------------------\r\n", 30, 100);

			Send_Sensor_Stats(&buf_gx, "GYRO X");
			Send_Sensor_Stats(&buf_gy, "GYRO Y");
			Send_Sensor_Stats(&buf_gz, "GYRO Z");

			HAL_UART_Transmit(&huart5, (uint8_t*) "--- [END OF REPORT] ---\r\n",
					25, 100);
		}

		if (i2c_ready_flag == 1) {
			i2c_ready_flag = 0;
			HAL_I2C_Mem_Read_DMA(&hi2c1, MPU6050_ADDR, TheMemAdd,
			I2C_MEMADD_SIZE_8BIT, data, 14);

		}
		HAL_Delay(10);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

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
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 15999;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 999;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief UART5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART5_Init(void) {

	/* USER CODE BEGIN UART5_Init 0 */

	/* USER CODE END UART5_Init 0 */

	/* USER CODE BEGIN UART5_Init 1 */

	/* USER CODE END UART5_Init 1 */
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 115200;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart5) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN UART5_Init 2 */

	/* USER CODE END UART5_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	/* DMA1_Stream6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {

		data_parsing(data, accel_data, gyro_data);

		buffer_add_value(&buf_ax, filter_that(accel_data[0], &f_ax));
		buffer_add_value(&buf_ay, filter_that(accel_data[1], &f_ay));
		buffer_add_value(&buf_az, filter_that(accel_data[2], &f_az));

		buffer_add_value(&buf_gx, filter_that(gyro_data[0], &f_gx));
		buffer_add_value(&buf_gy, filter_that(gyro_data[1], &f_gy));
		buffer_add_value(&buf_gz, filter_that(gyro_data[2], &f_gz));

		int len = sprintf(msg,
				"A: X:%.2f Y:%.2f Z:%.2f | G: X:%.2f Y:%.2f Z:%.2f\r\n",
				accel_data[0], accel_data[1], accel_data[2], gyro_data[0],
				gyro_data[1], gyro_data[2]);

		i2c_ready_flag = 1;
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM1) {
		timer_1hz_flag = 1;
	}
}
void Send_Sensor_Stats(struct buf_handle_t *p_handle, char *label) {
	if (p_handle->count < 2) {
		return;
	}

	float min = p_handle->buffer[0];
	float max = p_handle->buffer[0];
	float sum = 0.0f;
	float temp_sort[CIRC_BUF_SIZE];

	for (uint16_t i = 0; i < p_handle->count; i++) {
		float val = p_handle->buffer[i];
		if (val < min)
			min = val;
		if (val > max)
			max = val;
		sum += val;
		temp_sort[i] = val; 
	}
	float mean = sum / p_handle->count;

	float sum_sq_diff = 0.0f;
	for (uint16_t i = 0; i < p_handle->count; i++) {
		sum_sq_diff += pow(p_handle->buffer[i] - mean, 2);
	}
	float std_dev = sqrt(sum_sq_diff / p_handle->count);

	for (int i = 0; i < p_handle->count - 1; i++) {
		for (int j = 0; j < p_handle->count - i - 1; j++) {
			if (temp_sort[j] > temp_sort[j + 1]) {
				float temp = temp_sort[j];
				temp_sort[j] = temp_sort[j + 1];
				temp_sort[j + 1] = temp;
			}
		}
	}
	float median = temp_sort[p_handle->count / 2];

	char msg[128];
	int len = sprintf(msg, "[%s] Max:%.2f Min:%.2f Med:%.2f StdDev:%.4f\r\n",
			label, max, min, median, std_dev);

	HAL_UART_Transmit(&huart5, (uint8_t*) msg, len, 100);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
