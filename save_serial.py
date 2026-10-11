"""
Saves the ESP32's serial output to a CSV file on your laptop,
and lets you type commands to the ESP32 (just press Enter to move to the next stage).

Setup (once):   pip install pyserial
Run:            python save_serial.py COM3            (Windows, use your port)
                python save_serial.py /dev/ttyUSB0    (Linux)
                python save_serial.py /dev/cu.usbserial-0001  (Mac)

Close the Arduino Serial Monitor first, only one program can use the port.
Press Enter in this terminal to advance a stage (READY -> RECORDING -> DONE).
Press Ctrl+C to quit. Lines starting with "#" are shown on screen but not saved.
"""
import sys
import time
import threading
import serial

if len(sys.argv) < 2:
    print("Usage: python save_serial.py <port>   e.g. python save_serial.py COM3")
    sys.exit(1)

port = sys.argv[1]
filename = time.strftime("data_%Y%m%d_%H%M%S.csv")

ser = serial.Serial(port, 115200, timeout=1)
print(f"Listening on {port}. Saving data to {filename}.")
print("Press Enter to advance a stage. Ctrl+C to quit.\n")


def send_enter_presses():
    while True:
        try:
            text = input()
            ser.write((text + "\n").encode())
        except (EOFError, OSError):
            break


threading.Thread(target=send_enter_presses, daemon=True).start()

with open(filename, "w") as f:
    try:
        while True:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue
            print(line)                      # show everything on screen
            if not line.startswith("#"):     # only save CSV rows
                f.write(line + "\n")
                f.flush()
    except KeyboardInterrupt:
        print(f"\nStopped. Saved to {filename}")
