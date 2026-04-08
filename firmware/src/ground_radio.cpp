#include <SPI.h>
#include <SX128XLT.h>
#include <algorithm>
#include "config.h"

SX128XLT LT;

enum RadioMode {
    MODE_IDLE,
    MODE_MASTER,
    MODE_LISTENER
};

RadioMode currentMode = MODE_IDLE;

float get_lna_correction(int8_t rssi) {
    if (rssi >= LNA_LUT[0].rssi) return LNA_LUT[0].corr;
    if (rssi <= LNA_LUT[4].rssi) return LNA_LUT[4].corr;
    
    for (int i = 0; i < 4; i++) {
        if (rssi <= LNA_LUT[i].rssi && rssi > LNA_LUT[i+1].rssi) {
            float ratio = (float)(rssi - LNA_LUT[i+1].rssi) / (LNA_LUT[i].rssi - LNA_LUT[i+1].rssi);
            return LNA_LUT[i+1].corr + ratio * (LNA_LUT[i].corr - LNA_LUT[i+1].corr);
        }
    }
    return 0;
}

float apply_median_filter(float* results, int count) {
    if (count == 0) return -1.0;
    std::sort(results, results + count);
    if (count % 2 == 0) return (results[count/2 - 1] + results[count/2]) / 2.0;
    return results[count/2];
}

void setupListener() {
    LT.setRangingRole(0x01); 
    LT.writeRegister(0x9A, 0x01); 
    LT.setIrqMask(0x8000 | 0x4000); 
    currentMode = MODE_LISTENER;
    Serial.println(F("MODE:LISTENER"));
}

void setupMaster() {
    LT.setupRanging(RANGING_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_MASTER);
    LT.setRangingCalibration(CALIBRATION);
    currentMode = MODE_MASTER;
    Serial.println(F("MODE:MASTER"));
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(F("ESP-Skytrace: Ground Radio Engine (ESP8285) Initialized"));
    SPI.begin();
    if (LT.begin(LORA_SS, LORA_NRESET, LORA_BUSY, LORA_DIO1, DEVICE_SX1280)) {
        Serial.println(F("SX1280 Initialized."));
    } else {
        Serial.println(F("SX1280 Initialize Fail!"));
        while (1) delay(1000);
    }
    setupListener();
}

void loop() {
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 'M') setupMaster();
        else if (cmd == 'L') setupListener();
        else if (cmd == '!' && currentMode == MODE_MASTER) {
            // 1. Send Sync Packet
            LT.setupLoRa(SYNC_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, 0x01);
            uint8_t syncMsg[] = {0xAA, 0x55};
            LT.transmit(syncMsg, 2, 0, TX_POWER, WAIT_TX);
            delay(10); 

            float results[NUM_HOPS];
            int validCount = 0;
            float totalFEI = 0;

            // 2. 40-Channel Hopping Loop
            for (int i = 0; i < NUM_HOPS; i++) {
                LT.setupRanging(CHANNELS_BLE[i], 0, LORA_SF, LORA_BW, LORA_CR, RANGING_ADDR, RANGING_MASTER);
                LT.setRangingCalibration(CALIBRATION);
                
                if (LT.transmitRanging(RANGING_ADDR, 50, TX_POWER, WAIT_TX)) {
                    uint16_t irqStatus = LT.readIrqStatus();
                    if (irqStatus & 0x0200) {
                        float dist = LT.getRangingDistance(RANGING_RESULT_RAW, 0, 0);
                        int8_t rssi = LT.getRangingRSSI();
                        float ferror = LT.getFrequencyErrorHz();
                        
                        // Apply LNA correction
                        dist += get_lna_correction(rssi);
                        
                        results[validCount++] = dist;
                        totalFEI += (ferror / 1000000.0); // FEI in MHz
                    }
                }
            }

            if (validCount > 0) {
                float finalDist = apply_median_filter(results, validCount);
                float avgFEI = totalFEI / validCount;
                // Range' = Range - (m * ferror)
                finalDist = finalDist - (FEI_GRADIENT * avgFEI);
                Serial.print(F("DIST:"));
                Serial.println(finalDist);
            } else {
                Serial.println(F("DIST:-1.0"));
            }
        }
    }

    if (currentMode == MODE_LISTENER) {
        // Wait for Sync
        LT.setupLoRa(SYNC_FREQ, 0, LORA_SF, LORA_BW, LORA_CR, 0x01);
        uint8_t syncBuf[2];
        if (LT.receive(syncBuf, 2, 50, WAIT_RX) > 0) {
            uint32_t rawResults[NUM_HOPS];
            int validCount = 0;
            
            for (int i = 0; i < NUM_HOPS; i++) {
                // Setup Advanced Listener for this hop
                LT.setMode(MODE_STDBY_RC);
                LT.setFrequency(CHANNELS_BLE[i]);
                LT.setRangingRole(0x01); 
                LT.writeRegister(0x9A, 0x01);
                LT.setIrqMask(0x8000 | 0x4000);
                LT.setRx(0x01, 40); // 40ms timeout

                unsigned long start = millis();
                while (millis() - start < 45) {
                    uint16_t irqStatus = LT.readIrqStatus();
                    if (irqStatus & 0x8000) {
                        LT.clearIrqStatus(0x8000);
                        LT.setStandby(STDBY_XOSC);
                        uint8_t reg97F = LT.readRegister(0x97F);
                        LT.writeRegister(0x97F, reg97F | 0x02);
                        uint32_t raw = (uint32_t)LT.readRegister(0x0961) << 16 | (uint32_t)LT.readRegister(0x0962) << 8 | LT.readRegister(0x0963);
                        rawResults[validCount++] = raw;
                        break;
                    }
                    if (irqStatus & 0x4000) break;
                }
            }
            
            if (validCount > 0) {
                // Median filter raw 24-bit results
                std::sort(rawResults, rawResults + validCount);
                uint32_t finalRaw = rawResults[validCount/2];
                Serial.print(F("ATDT:"));
                Serial.println(finalRaw);
            }
            // Return to sync frequency
        }
    }
}
