#include <SPI.h>
#include <SX128XLT.h>
#include "config.h"

SX128XLT LT;
WiFiUDP udp;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("ESP-Skytrace: Ground Node Initialized"));

    // Connect to Backhaul
    WiFi.begin(BACKHAUL_SSID, BACKHAUL_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(F("Wi-Fi Connected."));

    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

    if (LT.begin(LORA_SS, LORA_NRESET, LORA_BUSY, LORA_DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 Initialized."));
    } else {
        Serial.println(F("SX1280 Initialize Fail!"));
        while (1);
    }

    LT.setupRanging(RANGING_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_MASTER);
    Serial.println(F("Ground Node in Ranging Master Mode."));
}

void loop() {
    Serial.print(F("Ranging... "));
    LT.transmitRanging(RANGING_ADDR, 500, TX_POWER, WAIT_TX);

    uint16_t irqStatus = LT.readIrqStatus();

    if (irqStatus & IRQ_RANGING_MASTER_RESULT_VALID) {
        float distance = LT.getRangingDistance(METRES, 0);
        Serial.printf("Distance: %.2fm\n", distance);

        // Send over UDP with Station ID
        // Note: Replace "G01" with unique ID for each station
        udp.beginPacket(LAPTOP_IP, UDP_PORT);
        udp.printf("ID:G01,DIST:%.2f", distance);
        udp.endPacket();
    } else {
        Serial.println(F("No response."));
    }

    delay(500);
}
