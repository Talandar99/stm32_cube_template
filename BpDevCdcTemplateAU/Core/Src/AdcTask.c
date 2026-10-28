/*
 * AdcTask.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "AdcTask.h"
#include "log_msg.h"

/**
* @brief Function implementing the AdcTask thread.
* @param argument: Not used
* @retval None
*/
void AdcTaskHandler(void *argument)
{
  // Infinite loop
  for(;;)
  {
    osDelay(1000);
  }

}
