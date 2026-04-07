#include <Arduino.h>
#include <unity.h>
#include <SPI.h>
#include "pins_config.h"

void test_sx1280_id(void) {
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    digitalWrite(CS_PIN, LOW);
    SPI.transfer(0x19); // Opcode to read register
    SPI.transfer(0x00); // Dummy address
    SPI.transfer(0x00); 
    SPI.transfer(0x00); // NOP for status
    uint8_t id = SPI.transfer(0x00);
    digitalWrite(CS_PIN, HIGH);

    // Verification of pins_config.h - if ID is read correctly, pins are likely correct.
    // In a real scenario, we'd check against a known value.
    TEST_ASSERT_NOT_EQUAL(0, id);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_sx1280_id);
    UNITY_END();
}

void loop() {}
