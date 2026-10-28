/*
 * Usart1Task.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "Usart1Task.h"
#include "log_msg.h"

/**
* @brief Function implementing the Usart1Task thread.
* @param argument: Not used
* @retval None
*/

void Usart1TaskHandler(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(200);
  }
}

