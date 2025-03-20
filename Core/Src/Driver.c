/*
 * Driver.c
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */


#include "Driver.h"
#include "stm32f4xx_hal.h"


extern UART_HandleTypeDef huart2;

/**
 * @brief Gửi dữ liệu qua giao tiếp UART.
 *
 * Hàm này chịu trách nhiệm gửi một mảng dữ liệu qua giao tiếp UART.
 *
 * @param[in] data Con trỏ đến mảng dữ liệu cần gửi.
 * @param[in] size Kích thước của mảng dữ liệu (số byte).
 */
void Driver_UART_Send(const uint8_t* data, size_t size)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)data, size, HAL_MAX_DELAY);
}

