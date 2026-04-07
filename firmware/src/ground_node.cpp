#include <WiFi.h>
#include <WifiEspNow.h>
#include <WiFiUdp.h>
#include "config.h"

// Set this Node's Station ID (G01, G02, or G03)
#define STATION_ID "G01"
// Define which MAC address belongs to this node (1, 2, or 3)
#define NODE_INDEX 1

WiFiUDP udp;
bool hasToken = (NODE_INDEX == 1); // Node 1 starts with the token
uint8_t nextNodeMac[6];

// UART Bridge on Serial1
HardwareSerial esp8285Serial(1);

void onReceive(const uint8_t mac[6], const uint8_t* buf, size_t count, void* arg) {
    if (count > 0 && buf[0] == 'T') { // 'T' for Token
        hasToken = true;
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("ESP-Skytrace: Ground Node Router (ESP32-S3) Initialized");

    // Initialize UART bridge to ESP8285
    // Pins 18 (RX) and 17 (TX) - cross the lines!
    esp8285Serial.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
    
    // Ignore initial boot garbage from ESP8285 (74880 baud text)
    while(esp8285Serial.available()) esp8285Serial.read();

    // Initialize Wi-Fi in Station Mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Initialize ESP-NOW
    if (!WifiEspNow.begin()) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }
    WifiEspNow.onReceive(onReceive, nullptr);

    // Determine next node in the ring
    if (NODE_INDEX == 1) memcpy(nextNodeMac, NODE_MAC_2, 6);
    else if (NODE_INDEX == 2) memcpy(nextNodeMac, NODE_MAC_3, 6);
    else if (NODE_INDEX == 3) memcpy(nextNodeMac, NODE_MAC_1, 6);

    if (!WifiEspNow.addPeer(nextNodeMac)) {
        Serial.println("Failed to add peer in Ring");
    }

    // Connect to UDP Backhaul (Laptop/Base Station)
    // Note: This requires a Wi-Fi AP to be present or use softAP on one node.
    // Assuming the laptop is hosting an AP or there's a central AP.
    WiFi.begin("Skytrace_Backhaul", "password123"); 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nBackhaul Connected.");
}

void loop() {
    if (hasToken) {
        Serial.println("Holding Token. Triggering Ranging...");
        
        // Trigger ESP8285 Ranging Engine
        esp8285Serial.print('!');
        
        // Wait for response with timeout
        unsigned long startWait = millis();
        String distanceStr = "";
        while (millis() - startWait < 1000) {
            if (esp8285Serial.available()) {
                char c = esp8285Serial.read();
                if (c == '\n') break;
                if (c != '\r') distanceStr += c;
            }
        }

        if (distanceStr.length() > 0) {
            float distance = distanceStr.toFloat();
            if (distance > 0) {
                Serial.printf("Distance measured: %.2f m\n", distance);
                
                // Pack UDP and send to Laptop
                udp.beginPacket(LAPTOP_IP, UDP_PORT);
                udp.printf("ID:%s,DIST:%.2f", STATION_ID, distance);
                udp.endPacket();
            } else {
                Serial.println("Ranging failure or timeout.");
            }
        }

        // Delay slightly for stability then pass token
        delay(50);
        uint8_t tokenMsg = 'T';
        if (WifiEspNow.send(nextNodeMac, &tokenMsg, 1)) {
            Serial.println("Token passed to next node.");
            hasToken = false;
        } else {
            Serial.println("Failed to pass token! Retrying...");
        }
    }
}
