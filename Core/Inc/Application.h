/*
 * Application.h
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "Protocol.h"
#include <stdint.h>

typedef enum {
    DATE_STREAM_DATA_ID = 1,
    TIME_STREAM_DATA_ID = 2,
    ADC_STREAM_DATA_ID = 3,
    HELLO_WORLD_DATA_ID = 4,
    BUTTON_STATE_DATA_ID = 5,
    MCU_TEMPERATURE_DATA_ID = 6
} data_id_t;


typedef enum {
 date_stream_data_rate_hz = 1,
 time_stream_data_rate_hz = 1,
 adc_stream_data_rate_hz = 50,
 hello_world_data_rate_hz = 2,
 button_state_data_rate_hz = 0,
 mcu_temperature_data_rate_hz = 0
} freq_t;


typedef struct
{
	uint32_t data_id;
    uint32_t days;
    uint32_t month;
    uint32_t year;
} date_stream_data_t;

typedef struct
{
	uint32_t data_id;
    uint8_t hour;
    uint16_t minute;
    uint16_t second;
} time_stream_data_t;

typedef struct
{
	uint32_t data_id;
	uint32_t sample_count;
	uint16_t value;
} adc_stream_data_t;

typedef struct
{
	uint32_t data_id;
    uint16_t string_len;
    uint8_t string[1024];
} hello_world_stream_data_t;

typedef struct
{
	uint32_t data_id;
    uint8_t button_id;
    uint16_t button_state;
} button_state_data_t;

typedef struct
{
	uint32_t data_id;
    uint16_t mcu_temperature_in_c;
} mcu_temperature_data_t;

void send_packet_data(packet_t* packet, void* data, uint16_t data_size);
void send_date_data(uint32_t days,uint32_t month, uint32_t year);
void send_time_data(uint8_t hour, uint16_t minute, uint16_t second);
void send_adc_data(uint32_t sample_count, uint16_t value);
void send_string_data(uint16_t string_len, uint8_t *string);
void send_button_data(uint8_t button_id, uint16_t button_state);
void send_temperature(uint16_t mcu_temperature_in_c);

#endif /* INC_APPLICATION_H_ */
