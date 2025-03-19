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
    Giải mã payload dựa trên data_id (4 byte đầu của payload) với định dạng đã thống nhất.
    
    Các định dạng:
      - Date Data (data_id=1): 16 byte → [data_id (4), days (4), month (4), year (4)]
      - Time Data (data_id=2): 12 byte → [data_id (4), hour (1), pad (3), minute (2), second (2)]
      - ADC Data (data_id=3): 12 byte → [data_id (4), value (2), sample_count (4), pad (2)]
      - Button Data (data_id=5): 8 byte → [data_id (4), button_id (1), button_state (2), pad (1)]
      - Temperature Data (data_id=6): 8 byte → [data_id (4), mcu_temperature_in_c (2), pad (2)]
      - String Data (data_id=4): 4 + 2 + string_len byte
    """
    payload_bytes = frame["payload"]
    ps = frame["payload_size"]
    if ps < 4:
        print("Payload không chứa đủ data_id (4 byte).")
        return

    data_id = int.from_bytes(payload_bytes[0:4], byteorder='little')
    print("Data ID:", data_id)
    
    if data_id == 1:
        if ps == 16:
            try:
                unpacked = struct.unpack('<IIII', payload_bytes[0:16])
                print("Decoded Date Data:")
                print("  Days:", unpacked[1])
                print("  Month:", unpacked[2])
                print("  Year:", unpacked[3])
            except Exception as e:
                print("Error decoding Date Data:", e)
        else:
            print("Expected payload size 16 for Date Data, got", ps)
    elif data_id == 2:
        if ps == 12:
            try:
                # Time Data: [data_id (4), hour (1), padding (3), minute (2), second (2)]
                hour = payload_bytes[4]
                minute = int.from_bytes(payload_bytes[8:10], byteorder='little')
                second = int.from_bytes(payload_bytes[10:12], byteorder='little')
                print("Decoded Time Data:")
                print("  Hour:", hour)
                print("  Minute:", minute)
                print("  Second:", second)
            except Exception as e:
                print("Error decoding Time Data:", e)
        else:
            print("Expected payload size 12 for Time Data, got", ps)
    elif data_id == 3:
        if ps == 12:
            try:
                # ADC Data: [data_id (4), value (2), sample_count (4), padding (2)]
                value = int.from_bytes(payload_bytes[4:6], byteorder='little')
                sample_count = int.from_bytes(payload_bytes[6:10], byteorder='little')
                print("Decoded ADC Data:")
                print("  Value:", value)
                print("  Sample Count:", sample_count)
            except Exception as e:
                print("Error decoding ADC Data:", e)
        else:
            print("Expected payload size 12 for ADC Data, got", ps)
    elif data_id == 4:
        if ps < 6:
            print("Payload too small for String Data.")
        else:
            try:
                string_len = int.from_bytes(payload_bytes[4:6], byteorder='little')
                expected_size = 4 + 2 + string_len
                if ps != expected_size:
                    print("Expected payload size for String Data:", expected_size, "got", ps)
                else:
                    string_data = payload_bytes[6:6+string_len]
                    print("Decoded String Data:")
                    print("  String Length:", string_len)
                    print("  String:", string_data.decode('utf-8', errors='replace'))
            except Exception as e:
                print("Error decoding String Data:", e)
    elif data_id == 5:
        if ps == 8:
            try:
                # Button Data: [data_id (4), button_id (1), button_state (2), padding (1)]
                button_id = payload_bytes[4]
                button_state = int.from_bytes(payload_bytes[5:7], byteorder='little')
                print("Decoded Button Data:")
                print("  Button ID:", button_id)
                print("  Button State:", button_state)
            except Exception as e:
                print("Error decoding Button Data:", e)
        else:
            print("Expected payload size 8 for Button Data, got", ps)
    elif data_id == 6:
        if ps == 8:
            try:
                # Temperature Data: [data_id (4), mcu_temperature_in_c (2), padding (2)]
                temperature = int.from_bytes(payload_bytes[4:6], byteorder='little')
                print("Decoded Temperature Data:")
                print("  MCU Temperature (°C):", temperature)
            except Exception as e:
                print("Error decoding Temperature Data:", e)
        else:
            print("Expected payload size 8 for Temperature Data, got", ps)
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
