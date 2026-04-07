#include <SPI.h>
#include <SX128XLT.h>
#include "config.h"

SX128XLT LT;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("ESP-Skytrace: Transponder Initialized"));

    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

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
    // respond automatically to ranging master
    LT.receiveRangingSlave(RANGING_ADDR, 0, WAIT_RX);

    uint16_t irqStatus = LT.readIrqStatus();
    if (irqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE) {
        Serial.println(F("Ranging request responded."));
    }
}
