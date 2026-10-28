#ifndef FREQ_H
#define FREQ_H

#include "stm32f4xx_hal.h"

void FREQ_Init(TIM_HandleTypeDef *htim);
void FREQ_Start(void);

float FREQ_GetFrequency(void);
float FREQ_GetDuty(void);

void FREQ_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

#endif
