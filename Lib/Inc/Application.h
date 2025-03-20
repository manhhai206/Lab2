/*
 * Application.h
 *
 *  Created on: Mar 18, 2025
 *      Author: MACH TRONG HAI
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include <Protocol.h>
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


#pragma pack(push, 1)
typedef struct {
    uint8_t  data_id;
    uint32_t days;
    uint32_t month;
    uint32_t year;
} date_stream_data_t;

typedef struct {
    uint8_t  data_id;
    uint8_t  hour;
    uint16_t minute;
    uint16_t second;
} time_stream_data_t;

typedef struct {
    uint8_t  data_id;
    uint32_t sample_count;
    uint16_t value;
} adc_stream_data_t;

typedef struct {
    uint8_t  data_id;
    uint16_t string_len;
    uint8_t  string[1024];
} hello_world_stream_data_t;

typedef struct {
    uint8_t  data_id;
    uint8_t  button_id;
    uint16_t button_state;
} button_state_data_t;

typedef struct {
    uint8_t  data_id;
    uint16_t mcu_temperature_in_c;
} mcu_temperature_data_t;
#pragma pack(pop)


/**
 * @brief Gửi một gói tin dữ liệu
 * @param[in]: packet Con trỏ đến cấu trúc packet_t để gửi
 * @param[in]: data Con trỏ đến dữ liệu cần gửi
 * @param[in]: data_size Kích thước dữ liệu
 */
void send_packet_data(packet_t* packet, void* data, uint16_t data_size);


/**
 * @brief Gửi dữ liệu ngày (Date)
 * @param[in]: days Số ngày trong tháng
 * @param[in]: month Tháng hiện tại
 * @param[in]: year Năm hiện tại
 */
void send_date_data(uint32_t days, uint32_t month, uint32_t year);


/**
 * @brief Gửi dữ liệu thời gian (Time)
 * @param[in]: hour Giờ hiện tại
 * @param[in]: minute Phút hiện tại
 * @param[in]: second Giây hiện tại
 */
void send_time_data(uint8_t hour, uint16_t minute, uint16_t second);


/**
 * @brief Gửi dữ liệu ADC
 * @param[in]: sample_count Số lượng mẫu ADC đo được
 * @param[in]: value Giá trị ADC hiện tại
 */
void send_adc_data(uint32_t sample_count, uint16_t value);


/**
 * @brief Gửi dữ liệu dạng chuỗi
 * @param[in]: string_len Độ dài chuỗi
 * @param[in]: string Con trỏ đến dữ liệu chuỗi cần gửi
 */
void send_string_data(uint16_t string_len, uint8_t *string);


/**
 * @brief Gửi trạng thái nút nhấn
 * @param[in]: button_id ID của nút bấm
 * @param[in]; button_state Trạng thái hiện tại của nút
 */
void send_button_data(uint8_t button_id, uint16_t button_state);


/**
 * @brief Gửi nhiệt độ của MCU
 * @param[in]: mcu_temperature_in_c Nhiệt độ của MCU theo đơn vị °C
 */
void send_temperature(uint16_t mcu_temperature_in_c);

#endif /* INC_APPLICATION_H_ */
