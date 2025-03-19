/*
 * Utils.c
 *
 *  Created on: Mar 19, 2025
 *      Author: MACH TRONG HAI
 */


#include "Utils.h"

#include "stm32f4xx_hal.h"

uint32_t Driver_GetTimeMs(void)
{
    return HAL_GetTick();
}
