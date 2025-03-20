/*
 * Protocol.h
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdint.h>
#include <stdlib.h>

/** @brief Byte đầu tiên của tiêu đề gói tin. */
static const uint8_t HEADER_BYTE1 = 0xDE;


/** @brief Byte thứ hai của tiêu đề gói tin. */
static const uint8_t HEADER_BYTE2 = 0xAB;


/** @brief Kích thước phần overhead của gói tin, bao gồm header, timestamp và checksum. */
#define PACKET_OVERHEAD (sizeof(uint16_t) * 3)


/** @brief Kích thước tối đa của payload trong gói tin. */
#define MAX_PAYLOAD_SIZE 1024


#pragma pack(push, 1)
typedef struct {
    uint16_t header;                             /**< Tiêu đề của gói tin. */
    uint16_t timestamp;                          /**< Thời gian đánh dấu của gói tin. */
    uint16_t payload_size;                       /**< Kích thước của dữ liệu payload. */
    uint8_t  payload[MAX_PAYLOAD_SIZE];          /**< Dữ liệu thực tế của gói tin. */
    uint16_t checksum;                           /**< Giá trị kiểm tra CRC16 của gói tin. */
} packet_t;
#pragma pack(pop)


/**
 * @brief Tính toán giá trị CRC16 cho một mảng dữ liệu.
 * Hàm này sử dụng thuật toán CRC16 để tính toán giá trị kiểm tra cho một mảng dữ liệu đầu vào.
 * @param[in]: data   Con trỏ đến mảng dữ liệu cần tính CRC.
 * @param[in]: length Độ dài của mảng dữ liệu.
 * @return  Giá trị CRC16 của dữ liệu.
 */
uint16_t calculate_crc16(uint8_t *data, uint16_t length);


/**
 * @brief Đóng gói dữ liệu vào cấu trúc gói tin.
 * Hàm này sẽ điền các thông tin cần thiết vào cấu trúc gói tin,
 * 			bao gồm tiêu đề, thời gian, kích thước payload và checksum.
 * @param[in]: packet   Con trỏ đến cấu trúc gói tin sẽ được điền dữ liệu.
 * @param[in]:  payload  Con trỏ đến dữ liệu payload cần đóng gói.
 * @param[in]:  payload_length Độ dài của dữ liệu payload.
 */
void pack_packet(packet_t *packet, uint8_t *payload, uint16_t payload_length);


/**
 * @brief Gửi gói tin qua giao thức truyền thông.
 * Hàm này chịu trách nhiệm gửi gói tin đã được đóng gói
 * 			qua giao thức truyền thông được định nghĩa.
 * @param[in] packet Con trỏ đến gói tin cần gửi.
 */
void send_packet(packet_t *packet);

#endif /* INC_PROTOCOL_H_ */

