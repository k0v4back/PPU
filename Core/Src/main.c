/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "stdio.h"
#include "main.h"
#include "lcd.h"
#include "main_loop.h"


UART_HandleTypeDef huart1;
UART_HandleTypeDef hlpuart1;
I2C_HandleTypeDef hi2c2;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim6;


void SystemClock_Config(void);
static void GPIO_Init(void);
static void USB_UART_Init(void);
static void LPUART_RS485(void);
//static void ADC_Init(void);
void TIM6_Init(void);

uint16_t data;
int main(void)
{
	
  HAL_Init();
  SystemClock_Config();
  GPIO_Init();
	USB_UART_Init();
	LPUART_RS485();
	TIM6_Init();
	//ADC_Init();
	LCD_Init();
	LCD_Clear();	
	//HAL_TIM_Base_Start_IT(&htim6);

	
  while (1)
  {
			loop();
  }
	
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


static void GPIO_Init(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
	
	//тактирование портов
  __HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStruct;
		
	/* USART1 for USB 
		GPIO Configuration
     PA9     ------> USB_UART_TX_Pin
     PA10     ------> USB_UART_RX_Pin
  */
    GPIO_InitStruct.Pin = USB_UART_TX_Pin  |USB_UART_RX_Pin  ;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(USB_UART_Port, &GPIO_InitStruct);
			
		/*USART2 for RS485
		GPIO Configuration
     PB10     ------> LPUART_RS485_TX_Pin 
     PB11     ------> LPUART_RS485_RX_Pin
		 PB1      ------> LPUART_RS485_DE_Pin (Hardware Flow Control)
    */
		GPIO_InitStruct.Pin = LPUART_RS485_RX_Pin | LPUART_RS485_TX_Pin | LPUART_RS485_DE_Pin ;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_LPUART1;
    HAL_GPIO_Init(LPUART_RS485_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(LPUART_RS485_Port,LPUART_RS485_DE_Pin,GPIO_PIN_SET); 
		
		/*
		ADC1 GPIO Configuration
		PA0    ------>ADC1_IN0_vrX_Pin 
		PA1    ------>ADC1_IN1_vrY_Pin 
		*/
		GPIO_InitStruct.Pin = ADC1_IN0_vrX_Pin|ADC1_IN0_vrY_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(ADC1_Port, &GPIO_InitStruct);
		
		/*
		LCD1602 GPIO Config(RS,D4,D5,D6,D7,BL,EN) and (LCD_BL_Pin,LCD_Enable)
		*/
		GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(LCD_Data_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(LCD_Data_Port,LCD_RS_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = LCD_BL_Pin|LCD_Enable_Pin|MODE_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(LCD_BL_EN_Port, &GPIO_InitStruct);
		
		HAL_GPIO_WritePin(LCD_BL_EN_Port,LCD_Enable_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_BL_EN_Port,LCD_BL_Pin,GPIO_PIN_SET); // backlight ON

}

static void USB_UART_Init(void)
{
  __HAL_RCC_USART1_CLK_ENABLE();

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
	
		HAL_NVIC_SetPriority(USART1_IRQn,0,0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
}

static void LPUART_RS485(void)
{
	__HAL_RCC_LPUART1_CLK_ENABLE();
	
	hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 4800;
	hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  //hlpuart1.Init.Mode = UART_MODE_TX_RX;
	hlpuart1.Init.Mode  = UART_MODE_TX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
}


/*static void ADC_Init(void)
{
		__HAL_RCC_ADC1_CLK_ENABLE();
			// For  Vdda ( 2.4 to 3.6 V ) FREQ MAXIMUM = 36 MHz
		hadc1.Instance = ADC1;  
		hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4; //21 MHz
		hadc1.Init.Resolution = ADC_RESOLUTION_8B;
		hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  hadc1.Init.ScanConvMode = ENABLE;
		hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
		hadc1.Init.ContinuousConvMode = DISABLE;
		hadc1.Init.NbrOfConversion = 2;
		hadc1.Init.DiscontinuousConvMode = DISABLE;
		//hadc1.Init.NbrOfDiscConversion = DISABLE;
	  //hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
	 //hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
		
	if( HAL_ADC_Init(&hadc1) != HAL_OK)
	{
	 Error_Handler();
	}
	
	//Configure Channel(injected) parameters
	AdcInjectedChannel.InjectedChannel = ADC_CHANNEL_1;
	AdcInjectedChannel.InjectedRank = ADC_INJECTED_RANK_1;
	AdcInjectedChannel.InjectedSamplingTime = ADC_SAMPLETIME_15CYCLES;
	AdcInjectedChannel.InjectedNbrOfConversion = 2;
	AdcInjectedChannel.InjectedOffset = 0;
	AdcInjectedChannel.AutoInjectedConv = DISABLE;
	AdcInjectedChannel.InjectedDiscontinuousConvMode = DISABLE;
	AdcInjectedChannel.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
	HAL_ADCEx_InjectedConfigChannel(&hadc1,&AdcInjectedChannel);
	
	AdcInjectedChannel.InjectedChannel = ADC_CHANNEL_2;
	AdcInjectedChannel.InjectedRank = ADC_INJECTED_RANK_2;
	HAL_ADCEx_InjectedConfigChannel(&hadc1,&AdcInjectedChannel);
	
	//HAL_NVIC_SetPriority(ADC_IRQn,0,0);
	//HAL_NVIC_EnableIRQ(ADC_IRQn);
	
	
}*/




void TIM6_Init() 
{
	__HAL_RCC_TIM6_CLK_ENABLE();
	
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 7999;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 2999; 
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim6);
	
	//HAL_NVIC_EnableIRQ(TIM6_IRQn);
	//HAL_NVIC_SetPriority(TIM6_IRQn,0,0);
	
}


void Error_Handler(void)
{
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
