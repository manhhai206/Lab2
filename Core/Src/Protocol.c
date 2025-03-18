/*
 * Protocol.c
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#include "Protocol.h"
#include <string.h>
#include "Driver.h"

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

void pack_packet(packet_t *packet, uint8_t *payload, uint16_t payload_length) {
    packet->header = HEADER_BYTE;
    packet->timestamp = get_current_timestamp();
    packet->payload_size = payload_length;

    for (uint16_t i = 0; i < payload_length; i++) {
        packet->payload[i] = payload[i];
    }

    packet->checksum = calculate_crc16((uint8_t*)packet, sizeof(packet_t) - sizeof(packet->checksum));
}

void send_packet(packet_t *packet)
{
    uart_send((uint8_t*)&packet->header, sizeof(packet->header));

    uart_send((uint8_t*)&packet->timestamp, sizeof(packet->timestamp));

    uart_send((uint8_t*)&packet->payload_size, sizeof(packet->payload_size));

    uart_send(packet->payload, packet->payload_size);

    uart_send((uint8_t*)&packet->checksum, sizeof(packet->checksum));
}




