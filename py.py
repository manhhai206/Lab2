import serial
import time
import struct
import csv

def calculate_crc16(data: bytes) -> int:
    """Tính CRC16 (CRC-16 Modbus) cho dữ liệu."""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc

def decode_frame(buffer: bytearray):
    """
    Giải mã một frame từ buffer.
    Cấu trúc frame:
      - Overhead: 6 byte (header, timestamp, payload_size)
      - Payload: payload_size byte
      - Checksum: 2 byte
    Trả về (frame_dict, remaining_buffer).
    Nếu dữ liệu chưa đủ, trả về (None, buffer).
    """
    min_frame_length = 6 + 0 + 2  # ít nhất 8 byte
    if len(buffer) < min_frame_length:
        return None, buffer

    # Kiểm tra header: phải là b'\xde\xab'
    if buffer[0:2] != b'\xde\xab':
        idx = buffer.find(0xDE)
        if idx == -1:
            return None, bytearray()
        buffer = buffer[idx:]
        if len(buffer) < min_frame_length or buffer[0:2] != b'\xde\xab':
            return None, buffer

    timestamp = int.from_bytes(buffer[2:4], byteorder='little')
    payload_size = int.from_bytes(buffer[4:6], byteorder='little')
    total_length = 6 + payload_size + 2
    if len(buffer) < total_length:
        return None, buffer

    payload = buffer[6:6+payload_size]
    checksum = int.from_bytes(buffer[6+payload_size:6+payload_size+2], byteorder='little')
    computed_crc = calculate_crc16(buffer[0:6] + payload)

    frame = {
        "header": buffer[0:2],
        "timestamp": timestamp,
        "payload_size": payload_size,
        "payload": payload,  # raw bytes
        "checksum": checksum,
        "computed_crc": computed_crc,
        "valid": (checksum == computed_crc)
    }
    remaining = buffer[total_length:]
    return frame, remaining

def decode_payload(frame):
    """
    Giải mã payload dựa trên data_id (1 byte đầu của payload) với định dạng mới.
    
    Các định dạng:
      - Date Data (data_id=1): 13 byte → [data_id (1), days (4), month (4), year (4)]
      - Time Data (data_id=2): 6 byte  → [data_id (1), hour (1), minute (2), second (2)]
      - ADC Data (data_id=3): 7 byte   → [data_id (1), sample_count (4), value (2)]
      - String Data (data_id=4): (1 + 2 + string_len) byte → [data_id (1), string_len (2), string (string_len)]
      - Button Data (data_id=5): 4 byte → [data_id (1), button_id (1), button_state (2)]
      - Temperature Data (data_id=6): 3 byte → [data_id (1), mcu_temperature_in_c (2)]
    """
    payload_bytes = frame["payload"]
    ps = frame["payload_size"]
    if ps < 1:
        return None
    data_id = payload_bytes[0]
    if data_id == 1 and ps == 13:
        try:
            unpacked = struct.unpack('<BIII', payload_bytes[0:13])
            return ("Date", unpacked[1], unpacked[2], unpacked[3])
        except Exception as e:
            print("Error decoding Date Data:", e)
            return None
    elif data_id == 2 and ps == 6:
        try:
            unpacked = struct.unpack('<BBHH', payload_bytes[0:6])
            return ("Time", unpacked[1], unpacked[2], unpacked[3])
        except Exception as e:
            print("Error decoding Time Data:", e)
            return None
    elif data_id == 3 and ps == 7:
        try:
            unpacked = struct.unpack('<BIH', payload_bytes[0:7])
            return ("ADC", unpacked[1], unpacked[2])
        except Exception as e:
            print("Error decoding ADC Data:", e)
            return None
    elif data_id == 4:
        if ps < 3:
            return None
        try:
            string_len = int.from_bytes(payload_bytes[1:3], byteorder='little')
            expected_size = 1 + 2 + string_len
            if ps != expected_size:
                print("Expected payload size for String Data:", expected_size, "got", ps)
                return None
            string_data = payload_bytes[3:3+string_len].decode('utf-8', errors='replace')
            return ("String", string_data)
        except Exception as e:
            print("Error decoding String Data:", e)
            return None
    elif data_id == 5 and ps == 4:
        try:
            unpacked = struct.unpack('<BBH', payload_bytes[0:4])
            return ("Button", unpacked[1], unpacked[2])
        except Exception as e:
            print("Error decoding Button Data:", e)
            return None
    elif data_id == 6 and ps == 3:
        try:
            unpacked = struct.unpack('<BH', payload_bytes[0:3])
            return ("Temperature", unpacked[1])
        except Exception as e:
            print("Error decoding Temperature Data:", e)
            return None
    else:
        print("Unrecognized data type or payload size mismatch.")
        return None

def main():
    ser = serial.Serial("COM6", 115200, timeout=0.1)
    
    csv_file = open('data.csv', 'w', newline='')
    csv_writer = csv.writer(csv_file)
    csv_writer.writerow(["Client Timestamp", "Interval (ms)", "Type", "Data..."])
    
    log_file = open("log.txt", "a")
    
    last_client_timestamp = None
    buffer = bytearray()
    try:
        while True:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                buffer.extend(data)
            
            frame, buffer = decode_frame(buffer)
            if frame:
                if not frame["valid"]:
                    log_file.write(f"Corrupted message at system time {time.time()}: {frame}\n")
                    log_file.flush()
                    continue  # Bỏ qua frame lỗi

                current_timestamp = frame["timestamp"]
                if last_client_timestamp is not None:
                    interval = current_timestamp - last_client_timestamp
                    print(f"Received Data Interval: {interval} ms")
                else:
                    interval = None
                last_client_timestamp = current_timestamp

                payload_info = decode_payload(frame)
                if payload_info:
                    row = [current_timestamp, interval]
                    row.extend(payload_info)
                    csv_writer.writerow(row)
                    csv_file.flush()
                    print("Logged row:", row)
                else:
                    log_file.write(f"Failed to decode payload at system time {time.time()}\n")
                    log_file.flush()
            time.sleep(0.05)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        csv_file.close()
        log_file.close()
        ser.close()

if __name__ == "__main__":
    main()
