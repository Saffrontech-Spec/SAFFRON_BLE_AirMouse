#include <BleConnectionStatus.h>
#include <BleMouse.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "BleMouse.h"      // Use from ESP32-BLE-Mouse-SAFFRON
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_ble_api.h>
#include <EEPROM.h>


// ========== Pin Definitions ==========
#define LEFT_BUTTON  25
#define RIGHT_BUTTON 26
#define SCROLL_UP    27
#define SCROLL_DOWN  14
#define LED_BUILTIN  2   // Onboard LED for most ESP32 dev boards


// ========== Motion & Screen Settings ==========
#define MOVE_SENSITIVITY 3.0
#define MOVE_THRESHOLD 0.1
#define FILTER_SAMPLES 10
#define EEPROM_SIZE 64


#define TARGET_WIDTH  1920
#define TARGET_HEIGHT 1080


// ========== Globals ==========
BleMouse bleMouse("SAFFRON");
Adafruit_MPU6050 mpu;


float xBuffer[FILTER_SAMPLES];
float yBuffer[FILTER_SAMPLES];
int bufferIndex = 0;


float cursorX = 0, cursorY = 0;
float maxX = 500, maxY = 300;


int leftState, rightState, scrollUpState, scrollDownState;
unsigned long scrollPressStart = 0;


// ========== EEPROM Save / Load ==========
void saveToEEPROM(float x, float y) {
  EEPROM.put(0, x);
  EEPROM.put(sizeof(float), y);
  EEPROM.commit();
  Serial.println("Saved motion limits to EEPROM.");
}


void loadFromEEPROM() {
  float storedX, storedY;
  EEPROM.get(0, storedX);
  EEPROM.get(sizeof(float), storedY);


  if (isnan(storedX) || isnan(storedY) || storedX < 10 || storedY < 10) {
    Serial.println("No valid EEPROM data. Using defaults.");
    maxX = 500;
    maxY = 300;
  } else {
    maxX = storedX;
    maxY = storedY;
    Serial.print("Loaded limits: X=");
    Serial.print(maxX);
    Serial.print(" Y=");
    Serial.println(maxY);
  }
}


// ========== LED Blink ==========
void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayMs);
  }
}


// ========== Reset Calibration ==========
void resetCalibration() {
  maxX = 500;
  maxY = 300;
  cursorX = 0;
  cursorY = 0;
  saveToEEPROM(maxX, maxY);
  Serial.println("Calibration reset to default.");
  blinkLED(5, 100);
}


// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  Serial.println(F("==================================="));
  Serial.println(F("       SAFFRON BLE AIR MOUSE       "));
  Serial.println(F("   EEPROM Save + LED + Reset Ready "));
  Serial.println(F("==================================="));


  EEPROM.begin(EEPROM_SIZE);
  loadFromEEPROM();


  pinMode(LED_BUILTIN, OUTPUT);
  blinkLED(3, 200);


  // BLE
  bleMouse.begin();
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);


  // MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found! Check wiring.");
    while (1);
  }
  Serial.println("MPU6050 connected!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);


  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(SCROLL_UP, INPUT_PULLUP);
  pinMode(SCROLL_DOWN, INPUT_PULLUP);


  for (int i = 0; i < FILTER_SAMPLES; i++) {
    xBuffer[i] = 0;
    yBuffer[i] = 0;
  }


  delay(1000);
}


// ========== Main Loop ==========
void loop() {
  if (bleMouse.isConnected()) {
    leftState = digitalRead(LEFT_BUTTON);
    rightState = digitalRead(RIGHT_BUTTON);
    scrollUpState = digitalRead(SCROLL_UP);
    scrollDownState = digitalRead(SCROLL_DOWN);


    // --- Mouse Buttons ---
    if (leftState == LOW) bleMouse.press(MOUSE_LEFT);
    else bleMouse.release(MOUSE_LEFT);


    if (rightState == LOW) bleMouse.press(MOUSE_RIGHT);
    else bleMouse.release(MOUSE_RIGHT);


    // --- Scroll Wheel ---
    if (scrollUpState == LOW) { bleMouse.move(0, 0, 1); delay(200); }
    if (scrollDownState == LOW) { bleMouse.move(0, 0, -1); delay(200); }


    // --- Check for Reset Calibration (hold both scrolls for 2 sec) ---
    if (scrollUpState == LOW && scrollDownState == LOW) {
      if (scrollPressStart == 0) scrollPressStart = millis();
      else if (millis() - scrollPressStart > 2000) { // held 2s
        resetCalibration();
        scrollPressStart = 0;
      }
    } else {
      scrollPressStart = 0;
    }


    // --- Motion Data ---
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);


    // (User preference)
    // X-axis → Left/Right
    // Y-axis → Up/Down
    float rawX = a.acceleration.x;  // left/right
    float rawY = a.acceleration.y;  // up/down


    // --- Smoothing ---
    xBuffer[bufferIndex] = rawX;
    yBuffer[bufferIndex] = rawY;
    bufferIndex = (bufferIndex + 1) % FILTER_SAMPLES;


    float avgX = 0, avgY = 0;
    for (int i = 0; i < FILTER_SAMPLES; i++) {
      avgX += xBuffer[i];
      avgY += yBuffer[i];
    }
    avgX /= FILTER_SAMPLES;
    avgY /= FILTER_SAMPLES;


    if (abs(avgX) < MOVE_THRESHOLD) avgX = 0;
    if (abs(avgY) < MOVE_THRESHOLD) avgY = 0;


    // --- Cursor motion ---
    float dx = avgX * MOVE_SENSITIVITY;
    float dy = -avgY * MOVE_SENSITIVITY;


    cursorX += dx;
    cursorY += dy;


    // --- Auto-learn range ---
    if (abs(cursorX) > maxX) maxX = abs(cursorX);
    if (abs(cursorY) > maxY) maxY = abs(cursorY);


    // --- Map to screen ---
    int mappedX = map(cursorX, -maxX, maxX, 0, TARGET_WIDTH);
    int mappedY = map(cursorY, -maxY, maxY, 0, TARGET_HEIGHT);


    // --- Clamp (dead-end) ---
    if (mappedX < 0) mappedX = 0;
    if (mappedX > TARGET_WIDTH) mappedX = TARGET_WIDTH;
    if (mappedY < 0) mappedY = 0;
    if (mappedY > TARGET_HEIGHT) mappedY = TARGET_HEIGHT;


    static int lastX = TARGET_WIDTH / 2;
    static int lastY = TARGET_HEIGHT / 2;


    int dxFinal = mappedX - lastX;
    int dyFinal = mappedY - lastY;


    lastX = mappedX;
    lastY = mappedY;


    if (dxFinal != 0 || dyFinal != 0) {
      bleMouse.move(dxFinal, dyFinal);
    }


    // Debug info
    Serial.print("Mapped: ");
    Serial.print(mappedX);
    Serial.print(",");
    Serial.print(mappedY);
    Serial.print(" | Range: ");
    Serial.print(maxX);
    Serial.print(",");
    Serial.println(maxY);
  }

  delay(20);
}
