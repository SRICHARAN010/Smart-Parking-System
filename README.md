# 🚗 Smart Parking System

An **IoT-enabled Smart Parking System** developed as a **Cyber-Physical Systems (CPS)** project. The system integrates **vehicle entry management, real-time parking-slot monitoring, and exit management** using embedded controllers, sensors, and cloud-based communication.

---

## 📌 Overview

The Smart Parking System is designed to automate and monitor different stages of a parking facility. The project consists of three major modules:

- 🚘 **Entry System**
- 🅿️ **Parking Slot Monitoring System**
- 🚪 **Exit System**

The system combines embedded hardware, sensor interfacing, programming, Wi-Fi communication, and cloud APIs to provide real-time monitoring and management of parking slots.

The parking module uses an **ESP32-S3** and **IR sensors** to detect vehicle occupancy. Parking-slot information is synchronized with a cloud server, while LED indicators and an OLED display provide real-time visual feedback.

---

# ⚙️ System Modules

## 🚘 Entry System

The entry module handles vehicle entry into the parking system and performs the required entry operations before allowing access to the parking area.

The module forms the first stage of the Smart Parking System and works together with the parking monitoring system to manage vehicle movement within the facility.

---

## 🅿️ Parking Slot Monitoring System

The parking monitoring module continuously monitors the occupancy status of three parking slots.

The system uses **IR sensors** to detect the presence of vehicles and an **ESP32-S3** to process the sensor data.

Each parking slot has a visual LED indicator:

| Parking Status | Indicator |
|---|---|
| 🟢 Available | Green LED |
| 🟡 Reserved | Yellow LED |
| 🔴 Occupied | Red LED |

The OLED display shows the total number of available parking slots in real time.

---

## 🚪 Exit System

The exit module handles vehicle exit operations from the parking facility.

It works as the final stage of the Smart Parking System and supports the overall flow of vehicles through the parking system.

---

# 🌐 Cloud Integration

The parking monitoring system uses **Wi-Fi connectivity** to communicate with a cloud server.

The ESP32-S3 performs two main cloud operations:

### 1. Fetch Parking Slot Status

The system periodically retrieves parking-slot information from the server using a `GET` request.

```text
GET /api/slots
```

### 2. Update Parking Slot Status

When the occupancy status of a parking slot changes, the ESP32-S3 sends an update to the server using a `POST` request.

```text
POST /api/update-slot
```
