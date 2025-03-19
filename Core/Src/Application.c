/*
 * Application.c
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */
#include "Application.h"

#include <string.h>

static packet_t global_date_packet       = {0};
static packet_t global_time_packet       = {0};
static packet_t global_adc_packet        = {0};
static packet_t global_string_packet     = {0};
static packet_t global_button_packet     = {0};
static packet_t global_temperature_packet = {0};

void send_packet_data(packet_t* packet, void* data, uint16_t data_size)
{
    if (packet == NULL || data == NULL || data_size == 0) {
        return;
    }

    pack_packet(packet, (uint8_t*)data, data_size);
    send_packet(packet);
}


void send_date_data(uint32_t days, uint32_t month, uint32_t year)
{
    date_stream_data_t date_data;
    date_data.data_id =  DATE_STREAM_DATA_ID;
    date_data.days = days;
    date_data.month = month;
    date_data.year = year;

    send_packet_data(&global_date_packet, &date_data, sizeof(date_stream_data_t));
}

void send_time_data(uint8_t hour, uint16_t minute, uint16_t second)
{
    time_stream_data_t time_data;
    time_data.data_id =  TIME_STREAM_DATA_ID;
    time_data.hour = hour;
    time_data.minute = minute;
    time_data.second = second;

    send_packet_data(&global_time_packet, &time_data, sizeof(time_stream_data_t));
}

void send_adc_data(uint32_t sample_count, uint16_t value)
{
    adc_stream_data_t adc_data;
    adc_data.data_id = ADC_STREAM_DATA_ID;
    adc_data.sample_count = sample_count;
    adc_data.value = value;

    send_packet_data(&global_adc_packet, &adc_data, sizeof(adc_stream_data_t));
}

void send_string_data(uint16_t string_len, uint8_t *string)
{
    if (string_len > 1024) {
        string_len = 1024;
    }

    hello_world_stream_data_t string_data;
    string_data.data_id = HELLO_WORLD_DATA_ID;
    string_data.string_len = string_len;
    memcpy(string_data.string, string, string_len);

    send_packet_data(&global_string_packet, &string_data, sizeof(uint32_t) + sizeof(uint16_t) + string_len);
}

void send_button_data(uint8_t button_id, uint16_t button_state)
{
    button_state_data_t button_data;
    button_data.data_id = BUTTON_STATE_DATA_ID;
    button_data.button_id = button_id;
    button_data.button_state = button_state;

    send_packet_data(&global_button_packet, &button_data, sizeof(button_state_data_t));
}

void send_temperature(uint16_t mcu_temperature_in_c)
{
    mcu_temperature_data_t temperature_data;
    temperature_data.data_id  = MCU_TEMPERATURE_DATA_ID;
    temperature_data.mcu_temperature_in_c = mcu_temperature_in_c;

    send_packet_data(&global_temperature_packet, &temperature_data, sizeof(mcu_temperature_data_t));
}
