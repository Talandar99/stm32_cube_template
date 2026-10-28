/*
 * I2c1Task.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "LedTask.h"
#include "I2c1Task.h"


/**
* @brief Function implementing the I2c1Task thread.
* @param argument: Not used
* @retval None
*/

void I2c1TaskHandler(void *argument)
{
  // Infinite loop
  for(;;)
  {
    osDelay(500);
  }
}



