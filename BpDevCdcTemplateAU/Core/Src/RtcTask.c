/*
 * RtcTask.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "RtcTask.h"


/**
* @brief Function implementing the RtcTask thread.
* @param argument: Not used
* @retval None
*/
void RtcTaskHandler(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(500);
  }
}
