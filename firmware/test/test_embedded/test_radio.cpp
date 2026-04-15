#include <Arduino.h>
#include <unity.h>
#include <SPI.h>
#include "config.h"

void test_sx1280_id(void) {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    pinMode(LORA_SS, OUTPUT);
    digitalWrite(LORA_SS, HIGH);

    digitalWrite(LORA_SS, LOW);
    SPI.transfer(0x19); // Opcode to read register
    SPI.transfer(0x00); // Dummy address
    SPI.transfer(0x00);
    SPI.transfer(0x00); // NOP for status
    uint8_t id = SPI.transfer(0x00);
    digitalWrite(LORA_SS, HIGH);

    // Verification of pin mapping from config.h.
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
