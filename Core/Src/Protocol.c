/*
 * Protocol.c
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#include "Protocol.h"
#include <string.h>
#include "Driver.h"
#include "Utils.h"


/**
 * @brief Tính toán giá trị CRC16 cho một mảng dữ liệu.
 * Hàm này sử dụng thuật toán CRC16 để tính toán giá trị kiểm tra
 * 				cho một mảng dữ liệu đầu vào.
 * @param[in] data   Con trỏ đến mảng dữ liệu cần tính CRC.
 * @param[in] length Độ dài của mảng dữ liệu.
 * @return    Giá trị CRC16 của dữ liệu.
 */
uint16_t calculate_crc16(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}


/**
 * @brief Đóng gói dữ liệu vào cấu trúc gói tin.
 * Hàm này sẽ điền các thông tin cần thiết vào cấu trúc gói tin,
 * 		bao gồm tiêu đề, thời gian,kích thước payload và checksum.
 * @param[in] packet         Con trỏ đến cấu trúc gói tin sẽ được điền dữ liệu.
 * @param[in]  payload        Con trỏ đến dữ liệu payload cần đóng gói.
 * @param[in]  payload_length Độ dài của dữ liệu payload.
 */
void pack_packet(packet_t *packet, uint8_t *payload, uint16_t payload_length)
{

    if (packet == NULL || payload == NULL || payload_length > MAX_PAYLOAD_SIZE) {
        return;
    }

    packet->header = ((uint16_t)HEADER_BYTE2 << 8) | HEADER_BYTE1;

    packet->timestamp = (uint16_t)Driver_GetTimeMs();

    packet->payload_size = payload_length;

    memcpy(packet->payload, payload, payload_length);

    uint16_t crc_length = sizeof(packet->header) + sizeof(packet->timestamp) + sizeof(packet->payload_size) + payload_length;

    packet->checksum = calculate_crc16((uint8_t*)packet, crc_length);
}

/**
 * @brief Gửi gói tin qua giao thức truyền thông.
 *
 * Hàm này chịu trách nhiệm gửi gói tin đã được đóng gói qua giao thức truyền thông được định nghĩa.
 *
 * @param[in] packet Con trỏ đến gói tin cần gửi.
 */
void send_packet(packet_t *packet)
{
    if (packet == NULL) {
        return;
    }

    uint16_t overhead_size = sizeof(packet->header) +
                             sizeof(packet->timestamp) +
                             sizeof(packet->payload_size);

    Driver_UART_Send((uint8_t*)packet, overhead_size);

    Driver_UART_Send(packet->payload, packet->payload_size);

    Driver_UART_Send((uint8_t*)&(packet->checksum), sizeof(packet->checksum));
}








