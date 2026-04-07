import serial
import json
import time

def handle_serial_input(port, baudrate=115200):
    """
    Stubs for handling incoming data from ground nodes via Serial.
    Data could be forwarded via a backhaul node connected to the laptop.
    """
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Listening on {port}...")
        
        while True:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(f"Received: {line}")
                # Parse ranging data and pass to engine
                # data = parse_ranging_line(line)
                # solve_position(...)
            
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")

if __name__ == "__main__":
    # handle_serial_input("/dev/tty.usbserial-xxx")
    pass
