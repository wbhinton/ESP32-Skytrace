#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// Standard ELRS RX SPI Pin Defines for ESP32-S3
#define LORA_SCK  5
#define LORA_MISO 6
#define LORA_MOSI 7
#define LORA_SS   8
#define LORA_BUSY 9
#define LORA_NRESET 10
#define LORA_DIO1 11

// Ranging Settings
#define RANGING_ADDR  0x12345678
#define RANGING_FREQ  2445000000
#define LORA_SF       LORA_SF10
#define LORA_BW       LORA_BW_800
#define LORA_CR       LORA_CR_4_5
#define TX_POWER      10

// Network Settings
const char* LAPTOP_IP = "192.168.4.1";
const uint16_t UDP_PORT = 12345;

#endif
