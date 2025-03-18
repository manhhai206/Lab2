/*
 * Driver.h
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */
#ifndef INC_DRIVER_H_
#define INC_DRIVER_H_

#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

uint16_t get_current_timestamp();
void uart_send(uint8_t* data, uint16_t size);

#endif /* INC_DRIVER_H_ */

