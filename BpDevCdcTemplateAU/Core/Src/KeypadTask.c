/*
 * KeypadTask.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "KeypadTask.h"
#include "log_msg.h"

/**
  * @brief  Function implementing the KeypadTask thread.
  * @param  argument: Not used
  * @retval None
  */
void KeypadTaskHandler(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(25);
  }

}
