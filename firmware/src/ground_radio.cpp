#include <SPI.h>
#include <SX128XLT.h>
#include "config.h"

SX128XLT LT;

enum RadioMode {
    MODE_IDLE,
    MODE_MASTER,
    MODE_LISTENER
};

RadioMode currentMode = MODE_IDLE;

void setupListener() {
    LT.setRangingRole(0x01); // Master setup as base
    // Advanced Ranging requires manual register/opcode setup
    LT.writeRegister(0x9A, 0x01); // SetAdvancedRanging
    // Enable bit 15 (0x8000) for AdvancedRangingDone IRQ
    LT.setIrqMask(0x8000 | 0x4000); // AdvancedRangingDone | RangingMasterTimeout
    LT.setRx(0x01, 0xFFFF); // Continuous Rx
    currentMode = MODE_LISTENER;
    Serial.println(F("MODE:LISTENER"));
}

void setupMaster() {
    LT.setupRanging(RANGING_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_MASTER);
    LT.setRangingCalibration(CALIBRATION);
    currentMode = MODE_MASTER;
    Serial.println(F("MODE:MASTER"));
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("ESP-Skytrace: Ground Radio Engine (ESP8285) Initialized"));

    SPI.begin();

    if (LT.begin(LORA_SS, LORA_NRESET, LORA_BUSY, LORA_DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 Initialized."));
    } else {
        Serial.println(F("SX1280 Initialize Fail!"));
        while (1) delay(1000);
    }

    // Default to Listener mode
    setupListener();
}

void loop() {
    // 1. Handle Serial Commands from ESP32-S3
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 'M') setupMaster();
        else if (cmd == 'L') setupListener();
        else if (cmd == '!' && currentMode == MODE_MASTER) {
            LT.transmitRanging(RANGING_ADDR, 500, TX_POWER, WAIT_TX);
            uint16_t irqStatus = LT.readIrqStatus();
            if (irqStatus & 0x0200) { // IRQ_RANGING_MASTER_RESULT_VALID
                float distance = LT.getRangingDistance(RANGING_RESULT_RAW, 0, 0);
                Serial.print(F("DIST:"));
                Serial.println(distance);
            } else {
                Serial.println(F("DIST:-1.0"));
            }
        }
    }

    // 2. Handle Advanced Ranging IRQs if in Listener Mode
    if (currentMode == MODE_LISTENER) {
        uint16_t irqStatus = LT.readIrqStatus();
        if (irqStatus & 0x8000) { // AdvancedRangingDone
            LT.clearIrqStatus(0x8000);
            
            // Strict 24-bit reading sequence
            LT.setStandby(STDBY_XOSC);
            
            // Enable clock in LoRa memory (set bit 1 of 0x97F)
            uint8_t reg97F = LT.readRegister(0x97F);
            LT.writeRegister(0x97F, reg97F | 0x02);
            
            // Read 24-bit result
            uint32_t rawResult = 0;
            rawResult |= (uint32_t)LT.readRegister(0x0961) << 16;
            rawResult |= (uint32_t)LT.readRegister(0x0962) << 8;
            rawResult |= (uint32_t)LT.readRegister(0x0963);
            
            LT.setStandby(STDBY_RC);
            
            Serial.print(F("ATDT:"));
            Serial.println(rawResult);
            
            // Re-enter Rx for next exchange
            LT.setRx(0x01, 0xFFFF);
        }
        
        if (irqStatus & 0x4000) { // Timeout
             LT.clearIrqStatus(0x4000);
             LT.setRx(0x01, 0xFFFF);
        }
    }
}
