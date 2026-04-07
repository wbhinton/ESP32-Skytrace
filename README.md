# ESP-Skytrace

3D aircraft tracking system using high-speed 2.4GHz LoRa ranging (Local Positioning System).

## Overview

This project implements a "DIY GPS" for high-precision aircraft tracking. It utilizes Time-of-Flight (ToF) ranging between synchronized ground nodes and a transponder on the aircraft.

### Hardware Architecture

The system features a streamlined dual-microcontroller architecture to isolate critical RF timing from high-level network tasks:

- **Ground Nodes (3x)**:
    - **Ranging Engine (ESP8285)**: Runs on the onboard microcontroller of a standard ELRS RX. Handles the SX1280 LoRa hardware and executes point-to-point ranging requests.
    - **Network Router (ESP32-S3)**: Acts as the "brains" of the node. Manages Wi-Fi backhaul, ESP-NOW coordination, and communicates with the Ranging Engine via a UART bridge (GPIO 17/18).
- **Aircraft Transponder (1x)**:
    - **Single-MCU (ESP8285)**: Runs standalone on a small ELRS RX. Continuously echoes ranging pings from the ground nodes.

### TDMA Token-Passing Scheme

To prevent RF collisions and ensure consistent sample rates, the system uses a **Time-Division Multiple Access (TDMA)** scheme implemented via an **ESP-NOW Token Ring**:

1.  **Node 1** holds the "token," triggers its ranging engine, and reports the distance to the laptop.
2.  **Node 1** passes the token to **Node 2** via ESP-NOW.
3.  **Node 2** performs its ranging and passes the token to **Node 3**.
4.  **Node 3** completes its turn and passes the token back to **Node 1**, closing the loop.

This ensures that only one ground node is pinging the transponder at any given microsecond, maximizing reliability and signal integrity.

## Firmware Structure

- `transponder`: Firmware for the aircraft ELRS RX (ESP8285).
- `ground-radio`: Firmware for the ground node's radio engine (ESP8285).
- `ground-node`: Firmware for the ground node's network router (ESP32-S3).

## Setup & Configuration

1.  **MAC Addresses**: Configure your ESP32-S3 MAC addresses in `firmware/src/config.h` to form the TDMA ring.
2.  **UART Bridge**: Connect the ELRS RX TX/RX pads to GPIO 18 (RX) and GPIO 17 (TX) on the ESP32-S3.
3.  **Build**: Use PlatformIO to flash the corresponding environments to each board.

```bash
# Example build command
pio run -e ground-node -e transponder -e ground-radio
```

## Coordinate System

The system uses a local Cartesian coordinate system (X, Y, Z) centered on a reference ground node, with the Python `processor` solving for position using trilateration.

---
