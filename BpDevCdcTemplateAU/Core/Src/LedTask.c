/*
 * LedTask.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "LedTask.h"
#include "log_msg.h"


/**
* @brief Function implementing the LedTask thread.
* @param argument: Not used
* @retval None
*/
void LedTaskHandler(void *argument)
{
  // Infinite loop
  for(;;) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(333);
  }
}
