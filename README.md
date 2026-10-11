# Pothole & Road Anomaly Telemetry: ESP32 Firmware

Firmware for an edge-AI system that detects potholes and rough-road anomalies from a two-wheeler. An ESP32 reads motion data from an IMU, tags it with GPS location, and (eventually) classifies bumps on-device with a TinyML model.

This repo holds the **ESP32 code only**. The dashboard lives in a separate repo.

> Part of the B.Tech project *"Edge-AI Based Real-Time Pothole & Road Anomaly Telemetry System for Two-Wheelers"*, which grew out of a 24-hour hackathon build (*Pothole & Rough Road Mapper for Two-Wheelers*).

## Status

| Feature | State |
|---|---|
| Read MPU6050 over I2C | Working |
| Log readings to CSV | Working |
| Onboard flash storage | In progress |
| GPS (NEO-6M) integration | Waiting on hardware |
| SD card logging | Waiting on hardware |
| TinyML classifier (Edge Impulse) | Not started, needs data first |
| Cloud sync (Firebase) | Planned |

## Hardware

- ESP32-S2 DevKit
- MPU6050 (accelerometer + gyro)
- NEO-6M GPS module *(pending)*
- microSD card module *(pending)*
- 10000 mAh power bank for standalone use

### Wiring

| MPU6050 | ESP32-S2 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO `13` |
| SCL | GPIO `14` |


## Getting Started

1. Install [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) with ESP32 board support.
2. Select **ESP32S2 Dev Module** as the board.
3. Install the required libraries:
   - `Adafruit MPU6050` 
   - `TinyGPS++` *(for GPS, once added)*
4. Clone and open the project:
   ```bash
   git clone https://github.com/<your-username>/<repo-name>.git
   ```
5. Flash the sketch and open Serial Monitor at `115200` baud.

## Data Format

Readings are logged as CSV:

```
timestamp_ms,ax,ay,az,gx,gy,gz
```

GPS columns (`lat,lon,speed`) will be appended once the module is integrated.

## Repo Structure

```
.
├── src/            # firmware source
├── data/           # sample logged CSVs
├── docs/           # wiring diagrams, notes
└── README.md
```


## Roadmap

1. Validate power bank runtime (5, 10, 15 minute logging tests)
2. Add GPS and SD logging
3. Collect ride data on real roads (potholes vs. swerves vs. smooth)
4. Convert vibration windows to spectrograms and train a classifier in Edge Impulse
5. Deploy the model on-device for real-time detection
6. Push events to the dashboard (Firebase + Leaflet.js map)

## Team

Built by a team of four. 


## License

Add a license (MIT is a common default for student projects).
