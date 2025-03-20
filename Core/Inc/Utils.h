/*
 * utils.h
 *
 *  Created on: Mar 19, 2025
 *      Author: MACH TRONG HAI
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>

/**
 * @brief Lấy thời gian hệ thống hiện tại tính bằng mili giây.
 * Hàm này trả về thời gian hiện tại của hệ thống tính bằng mili giây kể từ  khi hệ thống khởi động.
 * @return Thời gian hiện tại tính bằng mili giây.
 */
uint32_t Driver_GetTimeMs(void);

#endif /* INC_UTILS_H_ */


