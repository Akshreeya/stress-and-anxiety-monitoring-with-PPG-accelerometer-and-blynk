# Proactive Mental Health Monitor  
ESP32 + MAX30102 + ADXL335 | Real-time Heart Rate, SpO₂ & Motion-Based Stress Detection

This project is a wearable-style health monitoring prototype built using an ESP32 microcontroller.  
It measures:

- Heart Rate (HR) using MAX30102 optical PPG sensor  
- SpO₂ (oxygen saturation) using MAX30102  
- Motion activity using ADXL335 analog accelerometer  
- Stress level using simple rule-based classification (HR + motion)

All data is streamed to the Blynk IoT app, where users can start/stop monitoring and view live readings.

#  Components Required

### 🔧 Hardware
| Component | Quantity | Notes |
|----------|----------|-------|
| ESP32 Development Board | 1 | Any 30/38-pin ESP32 dev board |
| MAX30102 Heart Rate + SpO₂ Sensor | 1 | I²C connection |
| ADXL335 3-Axis Analog Accelerometer | 1 | X/Y/Z output pins |
| Jumper wires | – | For sensor connections |
| Breadboard | 1 | (Optional) |
| USB Cable | 1 | For powering & uploading code |

###  Software
- Arduino IDE or PlatformIO
- Blynk IoT App** (Android/iOS)
- Required Arduino libraries:
  - `BlynkSimpleEsp32`
  - `MAX30105`
  - `heartRate.h`
  - `spo2_algorithm.h`
  - `Wire.h`

---

#  Blynk App Setup
You can refer the images attached for template

### 1. Install the Blynk App
- Android/iOS → search **“Blynk IoT”**

### 2. Create a New Template
In Blynk:
1. Tap **Templates → New Template**
2. Set:
   - **Template Name:** `Proactive Mental Health Monitor`
   - **Hardware:** `ESP32`
   - **Connection Type:** `WiFi`
3. Save.

### 3. Get Your Blynk Credentials
Inside the template → go to **Info** tab.  
You will see:

- **BLYNK_TEMPLATE_ID**
- **BLYNK_TEMPLATE_NAME**
- **BLYNK_AUTH_TOKEN**

Copy these values and paste them into your code **locally**.

> **Important:** Never upload real credentials to GitHub.  
> The version in the repository should always use placeholders like:
> ```cpp
> char auth[] = "your_blynk_auth_token_here";
> ```

### 4. Set Up Widgets in Blynk Dashboard
Add the following widgets and map them to virtual pins:

| Widget | Virtual Pin | Purpose |
|--------|--------------|---------|
| Button | V0 | Start/Stop Monitoring |
| Label | V1 | Status (Normal / Stressed / Active) |
| Gauge | V3 | Heart Rate (bpm) |
| Gauge or Value Display | V5 | Motion Value |

---


