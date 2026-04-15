#include <SPI.h>
#include <SX128XLT.h>
#include "config.h"

SX128XLT LT;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("ESP-Skytrace: Transponder (ESP8285) Initialized"));

    SPI.begin(); // Standard pins used for ESP8285

    if (LT.begin(LORA_SS, LORA_NRESET, LORA_BUSY, LORA_DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 Initialized."));
    } else {
        Serial.println(F("SX1280 Initialize Fail!"));
        while (1);
    }

    LT.setupRanging(RANGING_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_SLAVE);
    Serial.println(F("Transponder in Ranging Slave Mode."));
}

void loop() {
    // 1. Wait for Sync Packet on Home Frequency
    LT.setupLoRa(SYNC_FREQ, 0, LORA_SF10, LORA_BW_0800, LORA_CR_4_5);
    uint8_t syncBuf[2];
    if (LT.receive(syncBuf, 2, 0, WAIT_RX) > 0) {
        // Sync received! Start hopping.
        for (int i = 0; i < NUM_HOPS; i++) {
            // Set Frequency for this hop
            LT.setMode(MODE_STDBY_RC);
            LT.setRfFrequency(CHANNELS_BLE[i], 0);
            
            // Switch to Ranging Slave Mode
            LT.setupRanging(CHANNELS_BLE[i], 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_SLAVE);
            
            // Wait for Master's ping on this frequency
            // Timeout after 30ms to prevent stalling if a frequency is blocked
            LT.receiveRanging(RANGING_ADDR, 30, TX_POWER, WAIT_RX);
        }
    }
}
