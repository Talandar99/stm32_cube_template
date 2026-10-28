#include "freq.h"

static TIM_HandleTypeDef *hfreq_tim;

extern TIM_HandleTypeDef htim4;

void MX_TIM4_Init_New(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 95;              // 1 MHz timer (zalecane)
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 0xFFFF;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  /* =========================
     SLAVE MODE (PWM INPUT)
     ========================= */
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
  sSlaveConfig.TriggerFilter = 0;

  if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* =========================
     CHANNEL 1 → PERIOD
     ========================= */
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;

  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* =========================
     CHANNEL 2 → DUTY
     ========================= */
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;

  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* =========================
     MASTER (nieużywany)
     ========================= */
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    FREQ_TIM_IC_CaptureCallback(htim);
}



/* Dane pomiarowe */
static volatile uint32_t period = 0;
static volatile uint32_t high_time = 0;

static volatile float frequency = 0.0f;
static volatile float duty = 0.0f;

/* Zegar TIM4 – dopasuj do swojej konfiguracji */
#define FREQ_TIM_CLOCK 96000000UL


// IRQ Handler
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(hfreq_tim);
}

void FREQ_Init(TIM_HandleTypeDef *htim)
{
    hfreq_tim = htim;

    MX_TIM4_Init_New();

    HAL_TIM_Base_Start(htim);

    // IRQ Enable
    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void FREQ_Start(void)
{
    /* PWM Input Mode:
       Start obu kanałów */
    HAL_TIM_IC_Start_IT(hfreq_tim, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(hfreq_tim, TIM_CHANNEL_2);
}

float FREQ_GetFrequency(void)
{
    return frequency;
}

float FREQ_GetDuty(void)
{
    return duty;
}

/* Callback z HAL */
void FREQ_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != hfreq_tim->Instance) return;

    /* CH1 → okres */
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        period = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        if (period != 0)
        {
            frequency = (float)FREQ_TIM_CLOCK /
                        ((float)(htim->Init.Prescaler + 1) * (float)period);
        }
    }

    /* CH2 → czas stanu wysokiego */
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        high_time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

        if (period != 0)
        {
            duty = ((float)high_time / (float)period) * 100.0f;
        }
    }
}
