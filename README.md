# 🌡️ IoT Temperature Monitoring System

This project is an IoT-based temperature monitoring system built using the **ESP32** microcontroller and a **BMP280** temperature sensor. It reads real-time temperature data, sends it via **HTTP POST** to a **Django backend**, and displays it on a simple web interface. Users can define **minimum and maximum temperature thresholds**, and the system will alert if the current temperature goes outside the defined range.

---

## 🚀 Features

- 📡 Real-time temperature readings from BMP280 via ESP32
- 🌐 HTTP POST communication to a Django server
- 🌡️ Web interface to display current temperature
- ⚠️ Alerts when temperature is out of user-defined range
- ✅ Threshold management (min & max) through the frontend

---

## 🧰 Technologies Used

### 🔌 Hardware
- ESP32 (via PlatformIO)
- BMP280 Temperature and Pressure Sensor

### 💻 Software
- Python (Django backend)
- HTML/CSS (Web UI)
- PlatformIO (ESP32 code)
- HTTP protocol for data transfer

---

## 🔁 System Workflow

1. BMP280 reads the temperature.
2. ESP32 sends this data to the Django server via an HTTP POST request.
3. Django receives and stores the data.
4. Web page displays the live temperature and compares it to the threshold.
5. If temperature < min or > max threshold → RED alert appears.  
   Else → GREEN "Normal" sign appears.

## 🛠️ Getting Started

### ⚙️ ESP32 Setup

1. Install [PlatformIO](https://platformio.org/install)
2. Connect BMP280 to ESP32 via I2C
3. Use the provided `.ino` or `.cpp` file in `esp32/` folder
4. Flash the ESP32 and check serial monitor for data

### 🌐 Django Setup

```bash
cd django-backend/
pip install -r requirements.txt
python manage.py runserver
