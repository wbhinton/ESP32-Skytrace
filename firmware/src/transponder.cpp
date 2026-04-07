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
    // Continuously respond to ranging pings
    LT.receiveRanging(RANGING_ADDR, 0, TX_POWER, WAIT_RX);
}
