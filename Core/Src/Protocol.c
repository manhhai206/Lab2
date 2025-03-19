/*
 * Protocol.c
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#include "Protocol.h"
#include "Driver.h"
#include "Utils.h"
#include <string.h>

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







