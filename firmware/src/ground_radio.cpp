#include <SPI.h>
#include <SX128XLT.h>
#include "config.h"

SX128XLT LT;

void setup() {
    Serial.begin(115200);
    delay(2000);
    // Note: Boot garbage from ESP8285 at 74880 baud is expected.
    Serial.println(F("ESP-Skytrace: Ground Radio Engine (ESP8285) Initialized"));

    SPI.begin();

    if (LT.begin(LORA_SS, LORA_NRESET, LORA_BUSY, LORA_DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 Initialized."));
    } else {
        Serial.println(F("SX1280 Initialize Fail!"));
        while (1) {
            delay(1000);
        }
    }

    LT.setupRanging(RANGING_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_MASTER);
    LT.setRangingCalibration(CALIBRATION);
    Serial.println(F("Ground Radio in Ranging Master Mode. Ready."));
}

void loop() {
    // Listen for trigger character from ESP32-S3
    if (Serial.available()) {
        char trigger = Serial.read();
        
        // Using '!' as the trigger character
        if (trigger == '!') {
            LT.transmitRanging(RANGING_ADDR, 500, TX_POWER, WAIT_TX);
            
            uint16_t irqStatus = LT.readIrqStatus();
            
            // IRQ_RANGING_MASTER_RESULT_VALID is 0x0200
            if (irqStatus & 0x0200) {
                float distance = LT.getRangingDistance(RANGING_RESULT_RAW, 0, 0);
                // Return distance string over Serial
                Serial.println(distance);
            } else {
                Serial.println("-1.0"); // Error indicator
            }
        }
    }
}
