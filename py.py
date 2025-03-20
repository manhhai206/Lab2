import serial
import time
import struct

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
        "payload_hex": payload.hex(),
        "payload_bin": ''.join(format(b, '08b') for b in payload),
        "checksum": checksum,
        "computed_crc": computed_crc,
        "valid": (checksum == computed_crc)
    }
    remaining = buffer[total_length:]
    return frame, remaining

def check_and_decode_data(frame: dict):
    """
    Giải mã payload dựa trên data_id (1 byte đầu của payload) với định dạng mới (data_id là 8-bit).
    
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
        print("Payload không chứa đủ data_id (1 byte).")
        return

    # Đọc 1 byte đầu của payload làm data_id
    data_id = payload_bytes[0]
    print("Data ID:", data_id)
    
    if data_id == 1:
        if ps == 13:
            try:
                # Date Data: data_id (1), days (4), month (4), year (4)
                unpacked = struct.unpack('<BIII', payload_bytes[0:13])
                print("Decoded Date Data:")
                print("  Days:", unpacked[1])
                print("  Month:", unpacked[2])
                print("  Year:", unpacked[3])
            except Exception as e:
                print("Error decoding Date Data:", e)
        else:
            print("Expected payload size 13 for Date Data, got", ps)
    elif data_id == 2:
        if ps == 6:
            try:
                # Time Data: data_id (1), hour (1), minute (2), second (2)
                unpacked = struct.unpack('<BBHH', payload_bytes[0:6])
                print("Decoded Time Data:")
                print("  Hour:", unpacked[1])
                print("  Minute:", unpacked[2])
                print("  Second:", unpacked[3])
            except Exception as e:
                print("Error decoding Time Data:", e)
        else:
            print("Expected payload size 6 for Time Data, got", ps)
    elif data_id == 3:
        if ps == 7:
            try:
                # ADC Data: data_id (1), sample_count (4), value (2)
                unpacked = struct.unpack('<BIH', payload_bytes[0:7])
                print("Decoded ADC Data:")
                print("  Sample Count:", unpacked[1])
                print("  Value:", unpacked[2])
            except Exception as e:
                print("Error decoding ADC Data:", e)
        else:
            print("Expected payload size 7 for ADC Data, got", ps)
    elif data_id == 4:
        if ps < 3:
            print("Payload too small for String Data.")
        else:
            try:
                # String Data: data_id (1), string_len (2), string (string_len)
                string_len = int.from_bytes(payload_bytes[1:3], byteorder='little')
                expected_size = 1 + 2 + string_len
                if ps != expected_size:
                    print("Expected payload size for String Data:", expected_size, "got", ps)
                else:
                    string_data = payload_bytes[3:3+string_len]
                    print("Decoded String Data:")
                    print("  String Length:", string_len)
                    print("  String:", string_data.decode('utf-8', errors='replace'))
            except Exception as e:
                print("Error decoding String Data:", e)
    elif data_id == 5:
        if ps == 4:
            try:
                # Button Data: data_id (1), button_id (1), button_state (2)
                unpacked = struct.unpack('<BBH', payload_bytes[0:4])
                print("Decoded Button Data:")
                print("  Button ID:", unpacked[1])
                print("  Button State:", unpacked[2])
            except Exception as e:
                print("Error decoding Button Data:", e)
        else:
            print("Expected payload size 4 for Button Data, got", ps)
    elif data_id == 6:
        if ps == 3:
            try:
                # Temperature Data: data_id (1), mcu_temperature_in_c (2)
                unpacked = struct.unpack('<BH', payload_bytes[0:3])
                print("Decoded Temperature Data:")
                print("  MCU Temperature (°C):", unpacked[1])
            except Exception as e:
                print("Error decoding Temperature Data:", e)
        else:
            print("Expected payload size 3 for Temperature Data, got", ps)
    else:
        print("Unrecognized data type or payload size mismatch.")


def main():
    uart_port = "COM6"
    baudrate = 115200
    try:
        ser = serial.Serial(uart_port, baudrate, timeout=0.1)
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return

    buffer = bytearray()
    print(f"Listening on {uart_port} at {baudrate} baud...")
    try:
        while True:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                buffer.extend(data)
            
            frame, buffer = decode_frame(buffer)
            if frame:
                print("Decoded frame:")
                print("  Header:          ", frame["header"].hex())
                print("  Timestamp:       ", frame["timestamp"])
                print("  Payload Size:    ", frame["payload_size"])
                print("  Payload (hex):   ", frame["payload_hex"])
                print("  Payload (binary):", frame["payload_bin"])
                print("  Checksum:        ", frame["checksum"])
                print("  Computed CRC:    ", frame["computed_crc"])
                print("  Valid:           ", frame["valid"])
                print("-" * 40)
                check_and_decode_data(frame)
                print("=" * 40)
            time.sleep(0.05)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        ser.close()

if __name__ == "__main__":
    main()
