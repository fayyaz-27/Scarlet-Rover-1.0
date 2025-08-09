# Scarlet Rover

**Gesture-Controlled ESP32 Rover with Collision Detection and Prevention**  
Built entirely with **ESP-IDF** 

---

## Overview
Scarlet Rover is a **gesture-controlled, collision-aware rover** powered by Espressif's ESP32.  
An **MPU6050** motion sensor on your hand captures gestures, which are wirelessly transmitted to the rover using **ESP-NOW**.  
The rover uses an **IR sensor** for instant collision prevention and includes an **onboard temperature sensor** for remote sensing.

---

## Features
- **Gesture-based control**:
  - Palm facing forward → Forward
  - Back of hand forward → Reverse
  - Tilt right → Turn right
  - Tilt left → Turn left
- **ESP-NOW communication** → Ultra-low latency.
- **Smooth collision avoidance** → Rover reverses instantly before hitting obstacles.
- **Onboard temperature sensing**.
- **Fully modular ESP-IDF drivers** 

---

## Hardware Used
- **2 × ESP32** (controller & rover)
- **MPU6050** accelerometer 
- **IR obstacle sensor**
- **Temperature sensor**
- **Motor driver** 
- **2 × DC motors** + chassis & wheels
- **Power bank** (controller)
- **Battery pack** (rover)

---
