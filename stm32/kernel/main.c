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
#include "main.h"
#define LED_TOGGLE_DELAY         100

RTC_HandleTypeDef rtc_handle;
UART_HandleTypeDef lpuart_handle;
static __IO uint32_t TimingDelay;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_RTC_Init(void);
static void Error_Handler(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* define the logging signature */
    static const char *FuncName="main()";
    
    HAL_Init();

    /* Configure LED1, and LED2 */
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);

    SystemClock_Config();
    HAL_MspInit();
    HAL_RCCEx_WakeUpStopCLKConfig(RCC_STOP_WAKEUPCLOCK_MSI);

    /* Initialize all configured peripherals */
    MX_LPUART1_UART_Init();
    MX_RTC_Init();

    /* Infinite loop */
    while (1)
    {
        MX_GPIO_Init();

        /* Re-enable wakeup source */
        /* ## Setting the Wake up time ############################################*/
        /* RTC Wakeup Interrupt Generation: 
           the wake-up counter is set to its maximum value to yield the longest
           stop time to let the current reach its lowest operating point.
           The maximum value is 0xFFFF, corresponding to about 33 sec. when 
           RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16

            Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
            Wakeup Time = Wakeup Time Base * WakeUpCounter 
                = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
                ==> WakeUpCounter = Wakeup Time / Wakeup Time Base
  
            To configure the wake up timer to 60s the WakeUpCounter is set to 0xFFFF:
            Wakeup Time Base = 16 /(~32.000KHz) = ~0.5 ms
            Wakeup Time = 0.5 ms  * WakeUpCounter
            Therefore, with wake-up counter =  0xFFFF  = 65,535 
            Wakeup Time =  0,5 ms *  65,535 = 32,7675 s ~ 33 sec.
        */
        HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0x0FFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
  
        /* Enter STOP 2 mode */
        HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
    
        /* ... STOP2 mode ... */

        /* Re-configure the system clock to 120 MHz based on MSI, enable and
            select PLL as system clock source (PLL is disabled in STOP mode) */
        SYSCLKConfig_STOP();
    
        /* Re-configure and turn on LED1  */
        BSP_LED_Init(LED1); 
        BSP_LED_On(LED1);
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    /* define the logging signature */
    static const char *FuncName="SystemClock_Config()";

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }
    
    /** Configure LSE Drive Capability
    */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);


    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
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

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 209700;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_7B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
    if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};
    RTC_TamperTypeDef sTamper = {0};

    /* Configure RTC */
    rtc_handle.Instance = RTC;
    
    /* Set the RTC time base to 1s */
    /* Configure RTC prescaler and RTC data registers as follow:
       - Hour Format = Format 24
       - Asynch Prediv = Value according to source clock
       - Synch Prediv = Value according to source clock
       - OutPut = Output Disable
       - OutPutPolarity = High Polarity
       - OutPutType = Open Drain
    */
    
    rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc_handle.Init.AsynchPrediv = 127;
    rtc_handle.Init.SynchPrediv = 255;
    rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtc_handle.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    rtc_handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc_handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    if(HAL_RTC_Init(&rtc_handle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(); 
    }

    /** Initialize RTC and set the Time and Date
    */
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 0x0;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }

    /** Enable the Alarm A
    */
    sAlarm.AlarmTime.Hours = 0x0;
    sAlarm.AlarmTime.Minutes = 0x0;
    sAlarm.AlarmTime.Seconds = 0x0;
    sAlarm.AlarmTime.SubSeconds = 0x0;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 0x1;
    sAlarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }

    /** Enable the RTC Tamper 1
    */
    sTamper.Tamper = RTC_TAMPER_1;
    sTamper.Trigger = RTC_TAMPERTRIGGER_RISINGEDGE;
    sTamper.NoErase = RTC_TAMPER_ERASE_BACKUP_ENABLE;
    sTamper.MaskFlag = RTC_TAMPERMASK_FLAG_DISABLE;
    sTamper.Filter = RTC_TAMPERFILTER_DISABLE;
    sTamper.SamplingFrequency = RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768;
    sTamper.PrechargeDuration = RTC_TAMPERPRECHARGEDURATION_1RTCCLK;
    sTamper.TamperPullUp = RTC_TAMPER_PULLUP_ENABLE;
    sTamper.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_ENABLE;
    if (HAL_RTCEx_SetTamper(&hrtc, &sTamper) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();        
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_All;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure); 
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure); 
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);    
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);

    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();  
    __HAL_RCC_GPIOG_CLK_DISABLE();  
    __HAL_RCC_GPIOH_CLK_DISABLE();
    __HAL_RCC_GPIOI_CLK_DISABLE();
  
    /* Disable all used wakeup source */
    HAL_RTCEx_DeactivateWakeUpTimer(&rtc_handle);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    BSP_LED_On(LED2);
    __disable_irq();
    while (1)
    {
        /* Infitine Loop */
    }
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
    while (1)
    {
        /* Infinite Loop */
    }
}
#endif /* USE_FULL_ASSERT */
