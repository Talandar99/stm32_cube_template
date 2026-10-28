/*
 * DacTask.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "DacTask.h"
#include "log_msg.h"
#include "tim.h"
//#include "DS18B20.h"
#include "freq.h"


// PWM Timer
extern TIM_HandleTypeDef htim3;
/*
void TIM3_SetFrequency(uint32_t frequency)
{
    uint32_t timer_clk = 96000000; // 84 MHz (TIM3 clock)
    uint32_t psc = htim3.Init.Prescaler + 1;

    uint32_t arr;

    if (frequency == 0) return;

    arr = (timer_clk / (psc * frequency)) - 1;

    if (arr > 0xFFFF) arr = 0xFFFF; // zabezpieczenie

    __HAL_TIM_SET_AUTORELOAD(&htim3, arr);

    // Zachowanie tego samego wypełnienia (%)
    uint32_t pulse = (arr + 1) / 2; // np. 50%
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse);
}
*/
void TIM3_SetFrequency(uint32_t frequency)
{
    uint32_t timer_clk = 96000000;
    uint32_t psc = htim3.Init.Prescaler + 1;

    if (frequency == 0) return;

    uint32_t arr = (timer_clk / (psc * frequency)) - 1;

    if (arr > 0xFFFF) arr = 0xFFFF;

    __HAL_TIM_SET_AUTORELOAD(&htim3, arr);
}
// -------------------
// duty cycle w [%]
// -------------------
/*
void setDutyCycle(uint8_t percent)
{
    if(percent > 100) percent = 100; // ograniczenie max do 100%
    uint32_t ccr = ((htim3.Init.Period + 1) * percent) / 100;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
}
*/
void setDutyCycle(uint8_t percent) {
    if (percent > 100) percent = 100;

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim3);
    uint32_t ccr = ((arr + 1) * percent) / 100;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
}
/**
* @brief Function implementing the DacTask thread.
* @param argument: Not used
* @retval None
*/
void DacTaskHandler(void *argument)
{
  //MX_TIM2_Init();

  // Setup PWM on CH1
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  uint8_t duty = 0;

  FREQ_Init(&htim4);
  FREQ_Start();

  //setDutyCycle(80);

  uint32_t freq = 20000;
  // Infinite loop
  for(;;)
  {
	duty = (duty + 1);
	if(duty > 99) duty = 1;

	uint32_t fr = (uint32_t)FREQ_GetFrequency();
	uint32_t dt = (uint32_t)FREQ_GetDuty();

	LogMsg_Inf("freq: %d Hz, duty: %d", fr, dt);

	setDutyCycle(duty);

	TIM3_SetFrequency(freq);

    osDelay(500);
    //osDelay(2000);
    //float temp = DS18B20_GetTemp();
    //int temp = (int)DS18B20_GetTemp();
    //LogMsg_Inf("Temp: %d.%d", (sw_ver >> 8), (sw_ver & 0xFF));
    //LogMsg_Inf("Temp: %d C", temp);

  }

}
