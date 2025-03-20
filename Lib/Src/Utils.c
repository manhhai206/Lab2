/*
 * Utils.c
 *
 *  Created on: Mar 19, 2025
 *      Author: MACH TRONG HAI
 */


#include "Utils.h"

#include "stm32f4xx_hal.h"


/**
 * @brief Lấy thời gian hệ thống hiện tại tính bằng mili giây.
 * Hàm này trả về thời gian hiện tại của hệ thống tính bằng mili giây kể từ  khi hệ thống khởi động.
 * @return Thời gian hiện tại tính bằng mili giây.
 */
uint32_t Driver_GetTimeMs(void)
{
    return HAL_GetTick();
}
