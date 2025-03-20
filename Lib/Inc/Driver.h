/*
 * Driver.h
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#ifndef INC_DRIVER_H_
#define INC_DRIVER_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Gửi dữ liệu qua giao tiếp UART.
 * Hàm này chịu trách nhiệm gửi một mảng dữ liệu qua giao tiếp UART.
 * @param[in] data Con trỏ đến mảng dữ liệu cần gửi.
 * @param[in] size Kích thước của mảng dữ liệu (số byte).
 */
void Driver_UART_Send(const uint8_t* data, size_t size);

#endif /* INC_DRIVER_H_ */



