#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <SX128XLT.h>

// Platform specific pin definitions
#if defined(ESP8266) // ESP8285
    #define LORA_SCK  14
    #define LORA_MISO 12
    #define LORA_MOSI 13
    #define LORA_SS   15
    #define LORA_BUSY 5
    #define LORA_NRESET -1 // Not used on most ELRS ESP8285
    #define LORA_DIO1 4
#elif defined(ESP32) // ESP32-S3
    #define LORA_SCK  5
    #define LORA_MISO 6
    #define LORA_MOSI 7
    #define LORA_SS   8
    #define LORA_BUSY 9
    #define LORA_NRESET 10
    #define LORA_DIO1 11
#endif

// UART Bridge Pins for ESP32-S3
#define RX1_PIN 18
#define TX1_PIN 17

// Ranging Settings
#define RANGING_ADDR  0x12345678
#define RANGING_FREQ  2445000000 
#define LORA_SF       LORA_SF10
#define LORA_BW       LORA_BW_0800
#define LORA_CR       LORA_CR_4_5
#define TX_POWER      13 
#define CALIBRATION   11401

// Advanced RF Mitigation Settings
#define NUM_HOPS 40
const uint32_t CHANNELS_BLE[40] = {
    2402000000, 2404000000, 2406000000, 2408000000, 2410000000,
    2412000000, 2414000000, 2416000000, 2418000000, 2420000000,
    2422000000, 2424000000, 2426000000, 2428000000, 2430000000,
    2432000000, 2434000000, 2436000000, 2438000000, 2440000000,
    2442000000, 2444000000, 2446000000, 2448000000, 2450000000,
    2452000000, 2454000000, 2456000000, 2458000000, 2460000000,
    2462000000, 2464000000, 2466000000, 2468000000, 2470000000,
    2472000000, 2474000000, 2476000000, 2478000000, 2480000000
};

#define FEI_GRADIENT -1.737

// LNA RSSI Correction LUT (RSSI dBm, Correction Meters)
struct RSSI_Corr { int8_t rssi; float corr; };
const RSSI_Corr LNA_LUT[5] = {
    {-30, 0.0},
    {-50, -1.0},
    {-70, -3.5},
    {-90, -6.0},
    {-110, -9.0}
};

// Sync Settings
#define SYNC_FREQ 2440000000
#define SYNC_ADDR 0xDE01

// TDMA Settings (ESP-NOW)
// Replace these with actual MAC addresses of your ESP32-S3 boards
const uint8_t NODE_MAC_1[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01};
const uint8_t NODE_MAC_2[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x02};
const uint8_t NODE_MAC_3[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x03};

// Network Settings
const char* LAPTOP_IP = "192.168.4.1";
const uint16_t UDP_PORT = 12345;

#endif
