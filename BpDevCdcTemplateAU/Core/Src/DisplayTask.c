/*
 * DisplayTask.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "DisplayTask.h"
#include "log_msg.h"

/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
void DisplayTaskHandler(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(40);
  }
}
