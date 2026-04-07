#include <Arduino.h>
#include <SPI.h>
#include <SX128XLT.h>
#include "pins_config.h"
#include "RangingLogic.h"

SX128XLT LT;

#define RANGING_ADDR 0x12345678

void setup() {
    Serial.begin(115200);
    Serial.println(F("Ranging Slave starting..."));

    SPI.begin();

    if (LT.begin(NSS, RESET, BUSY, DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 found!"));
    } else {
        Serial.println(F("SX1280 not found!"));
        while (1);
    }

    // Setup for ranging: Frequency, Offset, Spreading Factor, Bandwidth, Coding Rate, Address, Role
    LT.setupRanging(2445000000, 0, LORA_SF10, LORA_BW_800, LORA_CR_4_5, RANGING_ADDR, RANGING_SLAVE);

    Serial.println(F("Ranging Slave ready."));
}

void loop() {
    // receiveRangingSlave(address, timeout, wait)
    // 0 timeout means wait forever
    LT.receiveRangingSlave(RANGING_ADDR, 0, WAIT_RX);

    uint16_t irqStatus = LT.readIrqStatus();
    RangingStatus status = RangingLogic::interpretSlaveIrq(irqStatus);

    if (status == RANGING_SUCCESS) {
        Serial.println(F("Ranging request responded."));
    } else if (status == RANGING_TIMEOUT) {
        Serial.println(F("Ranging timed out."));
    } else {
        Serial.println(F("Ranging error."));
    }
}
