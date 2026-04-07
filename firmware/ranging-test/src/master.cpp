#include <Arduino.h>
#include <SPI.h>
#include <SX128XLT.h>
#include "pins_config.h"
#include "RangingLogic.h"

SX128XLT LT;

#define RANGING_ADDR 0x12345678
#define TXPOWER 10

void setup() {
    Serial.begin(115200);
    Serial.println(F("Ranging Master starting..."));

    SPI.begin();

    if (LT.begin(NSS, RESET, BUSY, DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 found!"));
    } else {
        Serial.println(F("SX1280 not found!"));
        while (1);
    }

    // Setup for ranging: Frequency, Offset, Spreading Factor, Bandwidth, Coding Rate, Address, Role
    // LORA_BW_800 is 812500 Hz
    LT.setupRanging(2445000000, 0, LORA_SF10, LORA_BW_800, LORA_CR_4_5, RANGING_ADDR, RANGING_MASTER);

    Serial.println(F("Ranging Master ready."));
}

void loop() {
    Serial.print(F("Ranging... "));
    
    // transmitRanging(address, timeout, txpower, wait)
    // timeout in ms, wait=1 means wait for completion
    LT.transmitRanging(RANGING_ADDR, 1000, TXPOWER, WAIT_TX);

    uint16_t irqStatus = LT.readIrqStatus();
    float rawDistance = 0;
    if (irqStatus & IRQ_RANGING_MASTER_RESULT_VALID) {
        rawDistance = LT.getRangingDistance(METRES, 0);
    }

    RangingResult result = RangingLogic::interpretMasterIrq(irqStatus, rawDistance);

    if (result.status == RANGING_SUCCESS) {
        Serial.print(F("Distance: "));
        Serial.print(result.distance, 1);
        Serial.println(F("m"));
    } else if (result.status == RANGING_TIMEOUT) {
        Serial.println(F("Ranging timed out."));
    } else {
        Serial.println(F("Ranging failed."));
    }

    delay(1000);
}
