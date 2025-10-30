# 🖱️ SAFFRON BLE AirMouse  
### (ESP32 + MPU6050 + BLE Mouse + Buttons + EEPROM + Smooth Filter)

---

## 🔍 Project Overview

The **SAFFRON BLE AirMouse** is a **wireless motion-controlled Bluetooth mouse** powered by **ESP32** and **MPU6050**.  
It translates your hand’s tilt and movement into real-time cursor motion — no dongle needed!

🧩 **Core Highlights:**
- Built on **ESP32** with **SAFFRON’s custom BLE Mouse library**
- **EEPROM integration** to remember motion calibration even after restart
- **Bluetooth connectivity up to 40 meters**, ensuring stable, long-range use
- Four tactile buttons for **left/right clicks** and **scroll up/down**

When you move or tilt the device, the MPU6050 captures acceleration and angular velocity data.  
The ESP32 processes it, applies a smoothing algorithm, and sends movement signals via BLE.

> ⚡ **The custom library Updated by SAFFRON** is fully compatible with the **latest ESP32 Core v3.3.2**.

> 📦 **Download the SAFFRON BLE Mouse Custom Library ZIP file** from this repository’s `/libraries` folder for best compatibility and performance.

## 📦 Custom SAFFRON BLE Mouse Library  

Download the latest version of our custom BLE library (v3.3.2) from below and add it to your Arduino IDE via  
**Sketch → Include Library → Add .ZIP Library**  

👉 [Download SAFFRON BLE Mouse Library (ZIP)](https://github.com/Saffrontech-Spec/SAFFRON_BLE_AirMouse/blob/main/ESP32-BLE-Mouse-SAFFRON-FIXED-LIB.zip)

---

## 🧠 Working Principle

1. **Sensor Data Acquisition:**  
   The MPU6050 continuously reads acceleration and gyro data from three axes.

2. **Data Processing & Filtering:**  
   The ESP32 collects and smooths motion using a **moving average filter**, ensuring stable cursor behavior.

3. **EEPROM Integration:**  
   - Saves and restores motion calibration (`maxX`, `maxY`)  
   - Retains user’s physical movement range even after power-off  
   - Visual confirmation through LED blink  

4. **Bluetooth Mouse Emulation:**  
   The ESP32 uses **BLE HID** protocol to emulate a mouse, capable of connecting to PCs, phones, and tablets.

5. **User Interaction:**  
   - Tilt → Cursor movement  
   - Buttons → Clicks and scrolling  

6. **Connectivity:**  
   The optimized BLE power setting extends range to **≈ 40 m** in open environments without signal drop.

---

## 🧩 Components Used

| Component | Quantity | Description |
|------------|-----------|-------------|
| ESP32 Development Board | 1 | Microcontroller with built-in BLE |
| MPU6050 Sensor | 1 | 3-axis accelerometer + 3-axis gyroscope |
| Push Buttons | 4 | Two black (clicks), two red (scrolls) |
| Breadboard | 1 | For prototyping |
| Jumper Wires | — | For connections |
| LED (Onboard) | 1 | Visual feedback (boot, calibration, save) |
| Power Source | 1 | USB or rechargeable Li-ion battery |

---

## 🧰 Libraries Used

| Library | Purpose |
|----------|----------|
| `Adafruit_MPU6050` | Read accelerometer and gyro data |
| `Adafruit_Sensor` | Base sensor interface |
| `BleMouse` *(SAFFRON Custom)* | BLE HID mouse emulation |
| `Wire` | I²C communication |
| `EEPROM` | Store motion calibration permanently |

📦 *Install these via Arduino Library Manager before uploading the sketch.*

---

## ⚙️ Circuit Diagram

Below is the official circuit layout for the **SAFFRON BLE AirMouse**:

![SAFFRON BLE AirMouse Circuit](https://github.com/Saffrontech-Spec/SAFFRON_BLE_AirMouse/blob/main/%40Saffrontech_.png)

| Component | ESP32 Pin | Description |
|------------|------------|-------------|
| MPU6050 VCC | 3.3 V | Power |
| MPU6050 GND | GND | Ground |
| MPU6050 SDA | 21 | I²C Data |
| MPU6050 SCL | 22 | I²C Clock |
| Left Button (Black) | 25 | Left Click |
| Right Button (Black) | 26 | Right Click |
| Scroll Up (Red) | 27 | Scroll Up |
| Scroll Down (Red) | 14 | Scroll Down |

---

## 🧪 Verification & Testing  

Hardware and firmware were verified step-by-step in **WOWKI Simulations** and on the real hardware.

### 1️⃣ Wiring Verification  
Ensured proper I²C and button connections between ESP32 and MPU6050.  
![Wiring Verification](/assets/Wiring_Check.jpg)
 
Confirmed MPU6050 found at address `0x68`.  

---

### 2️⃣ Button Function Test  
Checked Serial Monitor outputs (`L`, `R`, `SU`, `SD`) for each button.  

When a button is **not pressed**, you should see `1`.  
When **pressed**, it should show `0`.  

![Button Test](/assets/Buttons_Check.jpg)

---

### 3️⃣ MPU + BLE Interface Test  
Validated motion tracking and BLE HID operation.  

Expected Result:  
You’ll see values like:  
```

Accel X: -0.45  Y: 0.12  Z: 9.81

```
Move the sensor — the numbers should change.  
If you see that, your MPU6050 wiring (SDA = 21, SCL = 22, VCC = 3.3 V, GND = GND) is correct.

![MPU + BLE Mouse Test](/assets/MPU+Mouse_motion_Control_Check.jpg)

---

## 5⃣ How the Circuit Works with the Code

### Circuit Elements:
1. **ESP32 Board**  
   - Acts as controller and BLE module.  
2. **MPU6050 Sensor**  
   - Connected via I²C (SDA/SCL) pins.  
   - Measures acceleration and gyroscope data.  
3. **Buttons**  
   - Connected to GPIOs (25, 26, 27, 14).  
   - Uses internal pull-up resistors.

### Circuit Operation:
1. ESP32 reads MPU6050 tilt → converts to cursor movement.  
2. ESP32 reads button states → triggers mouse clicks or scrolls.  
3. BLE transmits HID mouse commands.  
4. Host device moves cursor or registers clicks in real time.  

---

## 6⃣ Strengths of this Setup

- Wireless mouse → no wires required  
- Smooth motion → moving average prevents jitter  
- Customizable sensitivity → `MOVE_SENSITIVITY` can be tuned  
- Low cost → ESP32 and MPU6050 are affordable and easy to source  
- Good range → BLE transmit power maximized (~40 m)

---

## 7⃣ Limitations

- BLE range varies (10–50 m depending on environment)  
- No precise distance measurement (requires extra code)  
- Continuous use consumes battery power  
- Only supports vertical scroll — horizontal scroll not implemented yet  

---

## 💾 EEPROM Functionality

The EEPROM stores the automatically learned motion range:

| Function | Description |
|-----------|-------------|
| `EEPROM.get()` | Loads saved limits on startup |
| `EEPROM.put()` | Writes new limits dynamically |
| Auto-Learn | Updates when larger motion is detected |
| LED Blink | Indicates EEPROM write operation |

This allows the AirMouse to **retain calibration even after restart**.

---

## 📊 Feature Summary

| Feature | Description |
|----------|-------------|
| MPU Orientation | X = Left/Right, Y = Up/Down |
| EEPROM Support | Saves motion range permanently |
| Auto-Scaling | Adapts to your physical hand movement range |
| LED Blink | 3× on startup, 5× on reset |
| Calibration Reset | Hold both scroll buttons (UP + DOWN) ≥ 2 seconds |
| Dead-End Boundaries | Cursor stops at virtual screen edges |
| BLE Name | “SAFFRON” |
| ESP32 Core | Fully compatible with v3.3.2 |

---

## 🚀 Features

✅ Fully wireless Bluetooth HID mouse  
✅ 40 m BLE range with power optimization  
✅ EEPROM-based calibration memory  
✅ Smooth motion using moving average filter  
✅ Four buttons (Left, Right, Scroll Up, Scroll Down)  
✅ Cross-platform compatibility  
✅ LED feedback on startup and reset  

---

## 💡 Applications

- Touchless / gesture-based control  
- Wireless presentation pointer  
- DIY VR motion controller  
- Educational IoT + sensor project  

---

## 🎥 Demo & Prototype Showcase

This section will include demonstration photos and prototype working videos once the build is complete.

### 📸 Demo Images
*(Coming Soon – Prototype Assembly & Final Product Photos)*

### ▶️ YouTube Demo
*(YouTube link will be added after prototype testing)*

---

## 🔮 Future Add-ons

🚧 Planned enhancements for future versions:
- Horizontal scroll functionality  
- Gesture recognition (swipe and tap actions)  
- Rechargeable battery integration  
- Low-power sleep mode  
- Optional OLED display for status  
- Support for ESP32-S3 and ESP32-C6  
- Improved smoothing algorithm with Kalman filter  

---

## 🧾 Summary

The **SAFFRON BLE AirMouse** efficiently transforms an **ESP32 + MPU6050** into a **smart motion-controlled Bluetooth mouse**.

- Smooth and responsive cursor control  
- Stable BLE range (~40 m)  
- EEPROM memory retention  
- Custom SAFFRON BLE library for ESP32 Core v3.3.2  

It’s a robust, educational, and fully functional prototype merging **motion sensing**, **wireless connectivity**, and **embedded software design**.

---

## 📁 Repository Structure

```

SAFFRON_BLE_AirMouse/
├── 📄 README.md
├── 🧠 SAFFRON_BLE_AirMouse.ino          # Main Arduino code
├── 🧩 libraries/
│   ├── BleMouse/                        # Custom SAFFRON BLE library
│   ├── Adafruit_MPU6050/
│   └── Adafruit_Sensor/
├── 📸 assets/
│   ├── @Saffrontech_.png                # Circuit diagram
│   ├── Wiring_Check.jpg                 # Wiring test
│   ├── Buttons_Check.jpg                # Button test
│   └── MPU+Mouse_motion_Control_Check.jpg # MPU+BLE test
└── ⚙️ LICENSE

```

---

## 🧠 Author & Credits

**Developed by:** [SAFFRON TECH](https://github.com/Saffrontech-spec-OFFICIAL)  
*[Updated the Original BLE Library]*  
_All credits for the Original BleMouse Library go to the original author._  

**Version:** v3.3.2 Stable  
**License:** [MIT](https://opensource.org/licenses/MIT)  
**GitHub Repository:** [SAFFRON BLE AirMouse](https://github.com/Saffrontech-spec/SAFFRON_BLE_AirMouse)

---

> 🪶 *Created and Updated by SAFFRON TECH — Innovation Through Motion Control.*

⭐ **If you found this project helpful, please give it a star!**
