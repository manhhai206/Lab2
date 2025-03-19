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

static const uint8_t HEADER_BYTE1 = 0xDE;
static const uint8_t HEADER_BYTE2 = 0xAB;
#define PACKET_OVERHEAD (sizeof(uint16_t) * 3)
#define MAX_PAYLOAD_SIZE 1024

#pragma pack(push, 1)
typedef struct {
    uint16_t header;
    uint16_t timestamp;
    uint16_t payload_size;
    uint8_t  payload[MAX_PAYLOAD_SIZE];
    uint16_t checksum;
} packet_t;
#pragma pack(pop)


uint16_t calculate_crc16(uint8_t *data, uint16_t length);
void pack_packet(packet_t *packet, uint8_t *payload, uint16_t payload_length);
void send_packet(packet_t *packet);

#endif /* INC_PROTOCOL_H_ */
