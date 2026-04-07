#include <Arduino.h>
#include <unity.h>
#include <SX128XLT.h>
#include "pins_config.h"

SX128XLT LT;

void test_sx1280_connection(void) {
    // Test if the chip is actually reachable over SPI
    bool success = LT.begin(NSS, RESET, BUSY, DIO1, DEVICE_SX1280);
    TEST_ASSERT_TRUE_MESSAGE(success, "SX1280 Hardware not found! Check wiring/pins.");
}

void test_sx1280_version(void) {
    // Every SX1280 should return a specific firmware version/ID
    uint8_t device_mode = LT.readRegister(0x0600); // Typical OpMode register
    TEST_ASSERT_NOT_EQUAL(0xFF, device_mode); // 0xFF usually means SPI fail
}

void setup() {
    delay(2000); // Wait for board to stabilize
    UNITY_BEGIN();
    RUN_TEST(test_sx1280_connection);
    RUN_TEST(test_sx1280_version);
    UNITY_END();
}

void loop() {}
