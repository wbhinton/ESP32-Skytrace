# ESP-Skytrace

3D aircraft tracking system (Local Positioning System).

## Overview

This project aims to build a "DIY GPS" for aircraft tracking. It uses Time of Flight (ToF) ranging between ground stations (anchors) and an aircraft transponder.

### Hardware

- **Ground Nodes**: 3x ESP32-S3 + SX1280 (ELRS RX).
- **Aircraft Transponder**: 1x ESP32-S3 + SX1280 (ELRS RX).

### Architecture

- **Ground Nodes**: Sync via ESP32-S3 backhaul and report distances to a central laptop.
- **Central Processor**: A Python engine performing Trilateration/Multilateration to solve for X, Y, Z coordinates.

## Coordinate System

The system uses a local Cartesian coordinate system (X, Y, Z) centered on a reference ground node.

---

