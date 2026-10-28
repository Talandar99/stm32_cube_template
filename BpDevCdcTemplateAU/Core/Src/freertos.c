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
#include "cmsis_os.h"
#include "LedTask.h"
#include "AdcTask.h"
#include "DisplayTask.h"
#include "KeypadTask.h"
#include "I2c1Task.h"
#include "RtcTask.h"
#include "Usart1Task.h"
#include "DacTask.h"
#include "CliTask.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticTimer_t osStaticTimerDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;
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

/* USER CODE END Variables */
/* Definitions for KeypadTask */
osThreadId_t KeypadTaskHandle;
uint32_t KeypadTaskBuffer[ 1024 ];
osStaticThreadDef_t KeypadTaskControlBlock;
const osThreadAttr_t KeypadTask_attributes = {
  .name = "KeypadTask",
  .cb_mem = &KeypadTaskControlBlock,
  .cb_size = sizeof(KeypadTaskControlBlock),
  .stack_mem = &KeypadTaskBuffer[0],
  .stack_size = sizeof(KeypadTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LedTask */
osThreadId_t LedTaskHandle;
uint32_t LedTaskBuffer[ 1024 ];
osStaticThreadDef_t LedTaskControlBlock;
const osThreadAttr_t LedTask_attributes = {
  .name = "LedTask",
  .cb_mem = &LedTaskControlBlock,
  .cb_size = sizeof(LedTaskControlBlock),
  .stack_mem = &LedTaskBuffer[0],
  .stack_size = sizeof(LedTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for AdcTask */
osThreadId_t AdcTaskHandle;
uint32_t AdcTaskBuffer[ 1024 ];
osStaticThreadDef_t AdcTaskControlBlock;
const osThreadAttr_t AdcTask_attributes = {
  .name = "AdcTask",
  .cb_mem = &AdcTaskControlBlock,
  .cb_size = sizeof(AdcTaskControlBlock),
  .stack_mem = &AdcTaskBuffer[0],
  .stack_size = sizeof(AdcTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
uint32_t DisplayTaskBuffer[ 1024 ];
osStaticThreadDef_t DisplayTaskControlBlock;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .cb_mem = &DisplayTaskControlBlock,
  .cb_size = sizeof(DisplayTaskControlBlock),
  .stack_mem = &DisplayTaskBuffer[0],
  .stack_size = sizeof(DisplayTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for I2c1Task */
osThreadId_t I2c1TaskHandle;
uint32_t I2c1TaskBuffer[ 1024 ];
osStaticThreadDef_t I2c1TaskControlBlock;
const osThreadAttr_t I2c1Task_attributes = {
  .name = "I2c1Task",
  .cb_mem = &I2c1TaskControlBlock,
  .cb_size = sizeof(I2c1TaskControlBlock),
  .stack_mem = &I2c1TaskBuffer[0],
  .stack_size = sizeof(I2c1TaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for RtcTask */
osThreadId_t RtcTaskHandle;
uint32_t RtcTaskBuffer[ 1024 ];
osStaticThreadDef_t RtcTaskControlBlock;
const osThreadAttr_t RtcTask_attributes = {
  .name = "RtcTask",
  .cb_mem = &RtcTaskControlBlock,
  .cb_size = sizeof(RtcTaskControlBlock),
  .stack_mem = &RtcTaskBuffer[0],
  .stack_size = sizeof(RtcTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Usart1Task */
osThreadId_t Usart1TaskHandle;
uint32_t Usart1TaskBuffer[ 1024 ];
osStaticThreadDef_t Usart1TaskControlBlock;
const osThreadAttr_t Usart1Task_attributes = {
  .name = "Usart1Task",
  .cb_mem = &Usart1TaskControlBlock,
  .cb_size = sizeof(Usart1TaskControlBlock),
  .stack_mem = &Usart1TaskBuffer[0],
  .stack_size = sizeof(Usart1TaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DacTask */
osThreadId_t DacTaskHandle;
uint32_t DacTaskBuffer[ 1024 ];
osStaticThreadDef_t DacTaskControlBlock;
const osThreadAttr_t DacTask_attributes = {
  .name = "DacTask",
  .cb_mem = &DacTaskControlBlock,
  .cb_size = sizeof(DacTaskControlBlock),
  .stack_mem = &DacTaskBuffer[0],
  .stack_size = sizeof(DacTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for CliTask */
osThreadId_t CliTaskHandle;
uint32_t CliTaskBuffer[ 1024 ];
osStaticThreadDef_t CliTaskControlBlock;
const osThreadAttr_t CliTask_attributes = {
  .name = "CliTask",
  .cb_mem = &CliTaskControlBlock,
  .cb_size = sizeof(CliTaskControlBlock),
  .stack_mem = &CliTaskBuffer[0],
  .stack_size = sizeof(CliTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for AdcCtrlQueue */
osMessageQueueId_t AdcCtrlQueueHandle;
uint8_t AdcCtrlQBuffer[ 8 * sizeof( uint32_t ) ];
osStaticMessageQDef_t AdcCtrlQControlBlock;
const osMessageQueueAttr_t AdcCtrlQueue_attributes = {
  .name = "AdcCtrlQueue",
  .cb_mem = &AdcCtrlQControlBlock,
  .cb_size = sizeof(AdcCtrlQControlBlock),
  .mq_mem = &AdcCtrlQBuffer,
  .mq_size = sizeof(AdcCtrlQBuffer)
};
/* Definitions for AdcResultQueue */
osMessageQueueId_t AdcResultQueueHandle;
uint8_t AdcResultQBuffer[ 8 * sizeof( uint32_t ) ];
osStaticMessageQDef_t AdcResultQControlBlock;
const osMessageQueueAttr_t AdcResultQueue_attributes = {
  .name = "AdcResultQueue",
  .cb_mem = &AdcResultQControlBlock,
  .cb_size = sizeof(AdcResultQControlBlock),
  .mq_mem = &AdcResultQBuffer,
  .mq_size = sizeof(AdcResultQBuffer)
};
/* Definitions for PwmCtrlCh1Queue */
osMessageQueueId_t PwmCtrlCh1QueueHandle;
uint8_t PwmCtrlCh1QBuffer[ 8 * sizeof( uint32_t ) ];
osStaticMessageQDef_t PwmCtrlCh1QControlBlock;
const osMessageQueueAttr_t PwmCtrlCh1Queue_attributes = {
  .name = "PwmCtrlCh1Queue",
  .cb_mem = &PwmCtrlCh1QControlBlock,
  .cb_size = sizeof(PwmCtrlCh1QControlBlock),
  .mq_mem = &PwmCtrlCh1QBuffer,
  .mq_size = sizeof(PwmCtrlCh1QBuffer)
};
/* Definitions for PwmCtrlCh2Queue */
osMessageQueueId_t PwmCtrlCh2QueueHandle;
uint8_t PwmCtrlCh2QBuffer[ 8 * sizeof( uint32_t ) ];
osStaticMessageQDef_t PwmCtrlCh2QControlBlock;
const osMessageQueueAttr_t PwmCtrlCh2Queue_attributes = {
  .name = "PwmCtrlCh2Queue",
  .cb_mem = &PwmCtrlCh2QControlBlock,
  .cb_size = sizeof(PwmCtrlCh2QControlBlock),
  .mq_mem = &PwmCtrlCh2QBuffer,
  .mq_size = sizeof(PwmCtrlCh2QBuffer)
};
/* Definitions for EncoderQueue */
osMessageQueueId_t EncoderQueueHandle;
uint8_t EncoderQBuffer[ 16 * sizeof( int32_t ) ];
osStaticMessageQDef_t EncoderQControlBlock;
const osMessageQueueAttr_t EncoderQueue_attributes = {
  .name = "EncoderQueue",
  .cb_mem = &EncoderQControlBlock,
  .cb_size = sizeof(EncoderQControlBlock),
  .mq_mem = &EncoderQBuffer,
  .mq_size = sizeof(EncoderQBuffer)
};
/* Definitions for TestTimer */
osTimerId_t TestTimerHandle;
osStaticTimerDef_t TestTimerControlBlock;
const osTimerAttr_t TestTimer_attributes = {
  .name = "TestTimer",
  .cb_mem = &TestTimerControlBlock,
  .cb_size = sizeof(TestTimerControlBlock),
};
/* Definitions for TestMutex */
osMutexId_t TestMutexHandle;
osStaticMutexDef_t TestMutexControlBlock;
const osMutexAttr_t TestMutex_attributes = {
  .name = "TestMutex",
  .cb_mem = &TestMutexControlBlock,
  .cb_size = sizeof(TestMutexControlBlock),
};
/* Definitions for TestBinarySem */
osSemaphoreId_t TestBinarySemHandle;
osStaticSemaphoreDef_t TestBinarySemControlBlock;
const osSemaphoreAttr_t TestBinarySem_attributes = {
  .name = "TestBinarySem",
  .cb_mem = &TestBinarySemControlBlock,
  .cb_size = sizeof(TestBinarySemControlBlock),
};
/* Definitions for keyEvent */
osEventFlagsId_t keyEventHandle;
osStaticEventGroupDef_t keyEventControlBlock;
const osEventFlagsAttr_t keyEvent_attributes = {
  .name = "keyEvent",
  .cb_mem = &keyEventControlBlock,
  .cb_size = sizeof(keyEventControlBlock),
};
/* Definitions for RtcAlarm1Event */
osEventFlagsId_t RtcAlarm1EventHandle;
osStaticEventGroupDef_t RtcAlarm1EventControlBlock;
const osEventFlagsAttr_t RtcAlarm1Event_attributes = {
  .name = "RtcAlarm1Event",
  .cb_mem = &RtcAlarm1EventControlBlock,
  .cb_size = sizeof(RtcAlarm1EventControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TestTimerCallback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of TestMutex */
  TestMutexHandle = osMutexNew(&TestMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of TestBinarySem */
  TestBinarySemHandle = osSemaphoreNew(1, 1, &TestBinarySem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of TestTimer */
  TestTimerHandle = osTimerNew(TestTimerCallback, osTimerPeriodic, NULL, &TestTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of AdcCtrlQueue */
  AdcCtrlQueueHandle = osMessageQueueNew (8, sizeof(uint32_t), &AdcCtrlQueue_attributes);

  /* creation of AdcResultQueue */
  AdcResultQueueHandle = osMessageQueueNew (8, sizeof(uint32_t), &AdcResultQueue_attributes);

  /* creation of PwmCtrlCh1Queue */
  PwmCtrlCh1QueueHandle = osMessageQueueNew (8, sizeof(uint32_t), &PwmCtrlCh1Queue_attributes);

  /* creation of PwmCtrlCh2Queue */
  PwmCtrlCh2QueueHandle = osMessageQueueNew (8, sizeof(uint32_t), &PwmCtrlCh2Queue_attributes);

  /* creation of EncoderQueue */
  EncoderQueueHandle = osMessageQueueNew (16, sizeof(int32_t), &EncoderQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of KeypadTask */
  KeypadTaskHandle = osThreadNew(KeypadTaskHandler, NULL, &KeypadTask_attributes);

  /* creation of LedTask */
  LedTaskHandle = osThreadNew(LedTaskHandler, NULL, &LedTask_attributes);

  /* creation of AdcTask */
  AdcTaskHandle = osThreadNew(AdcTaskHandler, NULL, &AdcTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(DisplayTaskHandler, NULL, &DisplayTask_attributes);

  /* creation of I2c1Task */
  I2c1TaskHandle = osThreadNew(I2c1TaskHandler, NULL, &I2c1Task_attributes);

  /* creation of RtcTask */
  RtcTaskHandle = osThreadNew(RtcTaskHandler, NULL, &RtcTask_attributes);

  /* creation of Usart1Task */
  Usart1TaskHandle = osThreadNew(Usart1TaskHandler, NULL, &Usart1Task_attributes);

  /* creation of DacTask */
  DacTaskHandle = osThreadNew(DacTaskHandler, NULL, &DacTask_attributes);

  /* creation of CliTask */
  CliTaskHandle = osThreadNew(CliTaskHandler, NULL, &CliTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of keyEvent */
  keyEventHandle = osEventFlagsNew(&keyEvent_attributes);

  /* creation of RtcAlarm1Event */
  RtcAlarm1EventHandle = osEventFlagsNew(&RtcAlarm1Event_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* TestTimerCallback function */
void TestTimerCallback(void *argument)
{
  /* USER CODE BEGIN TestTimerCallback */

  /* USER CODE END TestTimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

