/*
 * DS18B20.c
 *
 *  Created on: Jan 17, 2026
 *      Author: bitstork
 */
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

// ===== USER CONFIG =====
#define DS18B20_PORT GPIOB
#define DS18B20_PIN  GPIO_PIN_3
// =======================

TIM_HandleTypeDef htim2;

// Microsecond delay using TIM2
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

void DQ_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

void DQ_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

uint8_t DS18B20_Reset(void)
{
    uint8_t presence;
    DQ_Output();
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(480);
    DQ_Input();
    delay_us(70);
    presence = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
    delay_us(410);
    return !presence;
}

void DS18B20_WriteBit(uint8_t bit)
{
    DQ_Output();
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    if (bit)
    {
        delay_us(5);
        DQ_Input();
        delay_us(55);
    }
    else
    {
        delay_us(60);
        DQ_Input();
    }
}

uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit;
    DQ_Output();
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(2);
    DQ_Input();
    delay_us(8);
    bit = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
    delay_us(50);
    return bit;
}

void DS18B20_WriteByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(data & 0x01);
        data >>= 1;
    }
}

uint8_t DS18B20_ReadByte(void)
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        data >>= 1;
        if (DS18B20_ReadBit()) data |= 0x80;
    }
    return data;
}

float DS18B20_GetTemp(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp;

    DS18B20_Reset();
    DS18B20_WriteByte(0xCC); // Skip ROM
    DS18B20_WriteByte(0x44); // Convert T

    osDelay(750);

    DS18B20_Reset();
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE); // Read Scratchpad

    temp_l = DS18B20_ReadByte();
    temp_h = DS18B20_ReadByte();

    temp = (temp_h << 8) | temp_l;
    return (float)temp / 16.0f;
}

// ===== INIT TIM2 FOR 1MHz =====
void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = (SystemCoreClock / 1000000) - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFF;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2);
    HAL_TIM_Base_Start(&htim2);
}


