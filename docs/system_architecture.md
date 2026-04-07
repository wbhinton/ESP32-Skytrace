# System Architecture: ESP-Skytrace

This document describes the Time of Flight (ToF) flow and system architecture for the ESP-Skytrace Local Positioning System.

## Components

1.  **Anchors (Ground Nodes)**: 3x ESP32-S3 units equipped with SX1280 LoRa transceivers. These are fixed at known (X, Y, Z) coordinates.
2.  **Transponder (Aircraft Unit)**: 1x ESP32-S3 + SX1280 unit mounted on the aircraft.
3.  **Backhaul**: The anchors communicate with each other and a central processor via WiFi/ESP-NOW or serial link.
4.  **Processor**: A central laptop running the Python trilateration engine.

## Time of Flight (ToF) Flow

The ranging process follows the standard SX1280 Ranging procedure:

1.  **Initiation**: A Ground Node (Anchor) acts as the **Master** and sends a ranging request packet to the Transponder.
2.  **Response**: The Transponder acts as the **Slave** and immediately sends a response packet after a fixed internal delay.
3.  **Calculation**: The Master Anchor measures the round-trip time, subtracts the Slave's internal delay, and calculates the distance based on the speed of light.
4.  **Reporting**: The Master Anchor reports the distance and Transponder ID to the Central Processor.

```mermaid
sequence_diagram
    participant Anchor as Anchor (Master)
    participant XPNDR as Transponder (Slave)
    participant PC as Laptop (Processor)

    Anchor->>XPNDR: Ranging Request (ToF Start)
    XPNDR-->>Anchor: Ranging Response (ToF End)
    Anchor->>PC: Distance Data (Serial/UDP)
    PC->>PC: Solves Trilateration
```

## Solving for Position

The Python engine uses **Non-linear Least Squares** optimization to find the intersection of the spheres defined by the anchor coordinates and measured distances.

Given anchor coordinates $(x_i, y_i, z_i)$ and distances $d_i$:
$$ \sqrt{(x-x_i)^2 + (y-y_i)^2 + (z-z_i)^2} = d_i $$
The solver minimizes the residuals to find the best-fit $(x, y, z)$.
