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
#define TX_POWER      13 // Standard ELRS RX power
#define CALIBRATION   11401

// Note: Using library defaults for WAIT_TX and WAIT_RX which are blocking (0x01)

// TDMA Settings (ESP-NOW)
// Replace these with actual MAC addresses of your ESP32-S3 boards
const uint8_t NODE_MAC_1[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01};
const uint8_t NODE_MAC_2[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x02};
const uint8_t NODE_MAC_3[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x03};

// Network Settings
const char* LAPTOP_IP = "192.168.4.1";
const uint16_t UDP_PORT = 12345;

#endif
