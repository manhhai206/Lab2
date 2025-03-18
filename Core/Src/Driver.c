/*
 * Driver.c
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */


#include "Driver.h"

uint16_t get_current_timestamp()
{
    return (uint16_t)HAL_GetTick();
}

void uart_send(uint8_t* data, uint16_t size)
{
    HAL_UART_Transmit(&huart2, data, size, 100);
}
