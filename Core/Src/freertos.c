/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "obd2.h"
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
/* USER CODE BEGIN Variables */

osThreadId_t testTaskHandle;
const osThreadAttr_t testTask_attributes = {
  .name = "testTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void OBD2_TestTask(void *argument);
void USART_TestTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  //OBD2_TestTaskHandle = osThreadNew(OBD2_TestTask, NULL, &testTask_attributes);

  testTaskHandle = osThreadNew(OBD2_TestTask, NULL, &testTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

// Source - https://stackoverflow.com/a/3208376
// Posted by William Whyte, modified by community. See post 'Timeline' for change history
// Retrieved 2026-08-30, License - CC BY-SA 4.0

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
((byte) & 0x80 ? '1' : '0'), \
((byte) & 0x40 ? '1' : '0'), \
((byte) & 0x20 ? '1' : '0'), \
((byte) & 0x10 ? '1' : '0'), \
((byte) & 0x08 ? '1' : '0'), \
((byte) & 0x04 ? '1' : '0'), \
((byte) & 0x02 ? '1' : '0'), \
((byte) & 0x01 ? '1' : '0')

void OBD2_TestTask(void *argument)
{
  while(1)
  {
    printf("OBD2_TestTask cycle...\n");
    uint16_t OBD2_update_bits = OBD2_Poll_ECU_Data();
    if(OBD2_update_bits != 0)
    {
      printf("OBD2 update bits: "BYTE_TO_BINARY_PATTERN""BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(OBD2_update_bits));
    }

    printf("Gear: %d\n", (int) pECU_Data->actual_gear);
    printf("Speed: %d km/h\n", (int) pECU_Data->speed);
    printf("Fuel level: %d %%\n", (int) pECU_Data->fuel_level);
    printf("Coolant temp: %d °C\n", (int) pECU_Data->coolant_temperature);
    printf("Engine rpm: %d rpm\n", (int) pECU_Data->engine_rpm);
    printf("Boost pressure: %f kPa\n", pECU_Data->boost_pressure);
    printf("Fuel rate: %f L/h\n", pECU_Data->fuel_rate);

    osDelay(250);
  }
}

void USART_TestTask(void *argument)
{
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitTypeDef init = {0};
  init.Pin = GPIO_PIN_3;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &init);

  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
  while(1)
  {
    printf("Hello world\n");
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_3);
    osDelay(500);
  }
}

/* USER CODE END Application */

