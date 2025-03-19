/*
 * Utils.c
 *
 *  Created on: Mar 19, 2025
 *      Author: MACH TRONG HAI
 */


#include "Utils.h"
#include "stm32f4xx_hal.h"

uint16_t Driver_GetTimeMs(void)
{
    return (uint16_t)HAL_GetTick();
}
