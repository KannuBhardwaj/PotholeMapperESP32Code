// ESP32 + MPU6050 serial logger (no SD card needed)
//
// Press button once  -> prints "READY", LED turns on
// Press button again -> records for RUN_SECONDS (or until you press the button again)
//                       and prints CSV rows to the Serial Monitor
// After a run it goes back to idle. Press once to get READY again.
//
// Lines starting with "#" are status messages. Everything else is CSV data.
// Serial Monitor baud rate: 115200

#include <Wire.h>

// ---------- settings you can change ----------
const unsigned long RUN_SECONDS = 90;      // max length of one recording
const unsigned long SAMPLE_US   = 10000;   // 10000 us = 100 samples per second

// ---------- pins ----------
const int BUTTON_PIN = 4;    // button between GPIO4 and GND
const int LED_PIN    = 2;    // built-in LED on most ESP32 boards
// MPU6050: SDA = GPIO21, SCL = GPIO22

const uint8_t MPU_ADDR = 0x68;

enum State { IDLE, READY };
State state = IDLE;


// ---------- MPU6050 ----------
bool mpuInit() {
  Wire.begin();
  Wire.setClock(400000);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); Wire.write(0x00);      // wake up
  if (Wire.endTransmission() != 0) return false;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A); Wire.write(0x02);      // low-pass filter ~94 Hz
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C); Wire.write(0x10);      // accel range +/-8 g
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B); Wire.write(0x08);      // gyro range +/-500 deg/s
  Wire.endTransmission();
  return true;
}

bool mpuRead(float *v) {   // v[0..2] = accel in g, v[3..5] = gyro in deg/s
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)MPU_ADDR, 14) != 14) return false;

  int16_t raw[7];
  for (int i = 0; i < 7; i++) {
    int16_t hi = Wire.read();
    int16_t lo = Wire.read();
    raw[i] = (hi << 8) | lo;
  }
  v[0] = raw[0] / 4096.0;
  v[1] = raw[1] / 4096.0;
  v[2] = raw[2] / 4096.0;
  v[3] = raw[4] / 65.5;      // raw[3] is temperature, skipped
  v[4] = raw[5] / 65.5;
  v[5] = raw[6] / 65.5;
  return true;
}

// ---------- button ----------
// returns true once per press (with debounce)
bool buttonPressed() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(30);
    if (digitalRead(BUTTON_PIN) == LOW) {
      while (digitalRead(BUTTON_PIN) == LOW) {}   // wait for release
      delay(30);
      return true;
    }
  }
  return false;
}
// ---------- one recording ----------
void recordRun() {
  Serial.println("# RECORDING");
  Serial.println("time_ms,ax_g,ay_g,az_g,gx_dps,gy_dps,gz_dps,label");

  unsigned long start = millis();
  unsigned long nextSample = micros();
  float v[6];

  while (millis() - start < RUN_SECONDS * 1000UL) {
    // press the button during a run to stop early
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(30);
      if (digitalRead(BUTTON_PIN) == LOW) {
        while (digitalRead(BUTTON_PIN) == LOW) {}
        Serial.println("# STOPPED EARLY");
        break;
      }
    }

    if ((long)(micros() - nextSample) >= 0) {
      nextSample += SAMPLE_US;
      digitalWrite(LED_PIN, (millis() / 100) % 2);   // fast blink while recording
      if (mpuRead(v)) {
        Serial.print(millis() - start); Serial.print(',');
        Serial.print(v[0], 3); Serial.print(',');
        Serial.print(v[1], 3); Serial.print(',');
        Serial.print(v[2], 3); Serial.print(',');
        Serial.print(v[3], 2); Serial.print(',');
        Serial.print(v[4], 2); Serial.print(',');
        Serial.print(v[5], 2); Serial.println(',');
      }
    }
  }

  digitalWrite(LED_PIN, LOW);
  Serial.println("# DONE");
}
// ---------- setup / loop ----------
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(13, 14);   // Wire.begin(SDA, SCL) → SDA = GPIO13, SCL = GPIO14

  delay(500);

  if (!mpuInit()) {
    Serial.println("# ERROR: MPU6050 not found. Check wiring (SDA=21, SCL=22, 3.3V, GND).");
    while (true) {
      digitalWrite(LED_PIN, HIGH); delay(80);
      digitalWrite(LED_PIN, LOW);  delay(80);
    }
  }
  Serial.println("# Idle. Press the button to get READY.");
}

void loop() {
  if (buttonPressed()) {
    if (state == IDLE) {
      state = READY;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("# READY - press the button again to start recording");
    } else {
      state = IDLE;
      recordRun();
      Serial.println("# Idle. Press the button to get READY.");
    }
  }
}
