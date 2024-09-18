
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32l4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if(huart->Instance==LPUART1)
    {
        /** Initializes the peripherals clock
        */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
        PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_LPUART1_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**LPUART1 GPIO Configuration
        PC0     ------> LPUART1_RX
        PC1     ------> LPUART1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance==LPUART1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_LPUART1_CLK_DISABLE();

        /**LPUART1 GPIO Configuration
        PC0     ------> LPUART1_RX
        PC1     ------> LPUART1_TX
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1);
    }
}

/**
* @brief RTC MSP Initialization
* This function configures the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if(hrtc->Instance==RTC)
    {
    /** Initializes the peripherals clock
    */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_RTC_ENABLE();
        /* RTC interrupt Init */
        HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
    }
}

/**
* @brief RTC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
    if(hrtc->Instance==RTC)
    {
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();

        /* RTC interrupt DeInit */
        HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
    }
}
