#define BLYNK_TEMPLATE_ID "your_blyn_tmp"
#define BLYNK_TEMPLATE_NAME "your_tmp_name"
#define BLYNK_AUTH_TOKEN "your_blynk_auth_token_here"

#include <Wire.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
#include <math.h>

MAX30105 particleSensor;

// ---------------- WiFi + Blynk ----------------
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "your_SSID";
char pass[] = "your_wifi_password";

// ---------------- ADXL335 Pins ----------------
const int xPin = 34;
const int yPin = 35;
const int zPin = 32;

// Motion variables
float prevMagnitude = 0;
const float motionThreshold = 0.5;
float motionLevel = 0.0;

// ---------------- HR/SpO2 Variables ----------------
uint32_t irBuffer[100];
uint32_t redBuffer[100];
int32_t spo2, heartRate;
int8_t validSPO2, validHeartRate;
int beatAvg = 0;
uint32_t tsLastReport = 0;
#define REPORTING_PERIOD_MS 1000

// ---------------- Control Variables ----------------
bool monitoringActive = false;

// ---------------- Blynk Button ----------------
BLYNK_WRITE(V0) {
  monitoringActive = param.asInt(); // 1 = start, 0 = stop
  if (monitoringActive) {
    Serial.println("Monitoring Started...");
    Blynk.virtualWrite(V1, "Monitoring Started...");
  } else {
    Serial.println("Monitoring Stopped.");
    Blynk.virtualWrite(V1, "Monitoring Stopped.");
  }
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Wire.begin();

  // Initialize MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 not found. Check wiring!");
    while (1);
  }
  particleSensor.setup(50, 1, 2, 100, 69, 4096);

  Serial.println("System Ready. Press Start on Blynk.");
}

// ---------------- Detect Motion ----------------
float detectMotion() {
  int xADC = analogRead(xPin);
  int yADC = analogRead(yPin);
  int zADC = analogRead(zPin);

  float x_g = (((xADC * 3.3 / 4095.0) - 1.65) / 0.330);
  float y_g = (((yADC * 3.3 / 4095.0) - 1.65) / 0.330);
  float z_g = (((zADC * 3.3 / 4095.0) - 1.65) / 0.330);

  float magnitude = sqrt(x_g * x_g + y_g * y_g + z_g * z_g);
  float delta = fabs(magnitude - prevMagnitude);
  prevMagnitude = magnitude;

  motionLevel = delta;
  return delta;
}

// ---------------- Main Loop ----------------
void loop() {
  Blynk.run();

  if (!monitoringActive)
    return;

  // 1. Collect 100 samples for PPG
  for (byte i = 0; i < 100; i++) {
    while (particleSensor.available() == false)
      particleSensor.check();

    irBuffer[i] = particleSensor.getIR();
    redBuffer[i] = particleSensor.getRed();
    particleSensor.nextSample();
  }

  // 2. Compute HR + SpO2
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer, 100, redBuffer,
    &spo2, &validSPO2, &heartRate, &validHeartRate);

  // 3. Every 1 second update Blynk
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    if (validHeartRate == 1 && heartRate > 20 && heartRate < 200)
      beatAvg = (beatAvg + heartRate) / 2;

    Blynk.virtualWrite(V3, beatAvg);

    // --- Motion Detection ---
    float motionVal = detectMotion();
    Blynk.virtualWrite(V5, motionVal);

    // --- Stress Logic ---
    String statusMsg;
    if (beatAvg > 120) {
      if (motionVal > motionThreshold)
        statusMsg = "Not Stressed 😊 - Have a good day!";
      else
        statusMsg = "Stressed 😟 - Try breathing in/out for 5 mins";
    } else {
      statusMsg = "Normal ❤️";
    }

    Blynk.virtualWrite(V1, statusMsg);

    // Debug Output
    Serial.print("HR=");
    Serial.print(beatAvg);
    Serial.print(" | Motion=");
    Serial.print(motionVal, 2);
    Serial.print(" | Status=");
    Serial.println(statusMsg);

    tsLastReport = millis();
  }
}
