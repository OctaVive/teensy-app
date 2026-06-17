#include "core/Diagnostics.h"
#include "pinmap.h"

#include <Arduino.h>

void Diagnostics::begin() {
    pinMode(pinmap::STATUS_LED, OUTPUT);
    digitalWrite(pinmap::STATUS_LED, LOW);
}

void Diagnostics::logBoot(const NodeConfig& cfg) {
    Serial.println("=== Teensy sACN/DMX LED Node ===");
    Serial.printf("Mode: %s\n", cfg.mode == GlobalMode::Pixel ? "pixel" : "effect");
    Serial.printf("LEDs: %u total (%u strips x %u)\n", cfg.leds.totalLeds, NUM_STRIPS,
                  cfg.leds.strips[0].ledCount);
    Serial.printf("sACN universes: %u (start %u)\n", cfg.sacn.universeCount,
                  cfg.sacn.startUniverse);
    Serial.printf("Config source: %s\n", cfg.loadedFromSd ? "SD card" : "embedded defaults");
}

void Diagnostics::logFrame(const FrameStats& stats, GlobalMode mode) {
    const uint32_t now = millis();
    if ((now - lastLogMs_) < 5000) {
        return;
    }
    lastLogMs_ = now;
    Serial.printf("[Frame] fps=%.1f avg=%uus mode=%s sacn_rx=%u drop=%u\n", stats.outputFps,
                  stats.avgFrameUs, mode == GlobalMode::Pixel ? "pixel" : "effect",
                  stats.sacnPacketsReceived, stats.sacnPacketsDropped);
}

void Diagnostics::logSacnPacket(uint16_t universe, bool accepted) {
    Serial.printf("[sACN] universe=%u %s\n", universe, accepted ? "OK" : "DROP");
}

void Diagnostics::logLinkChange(bool up) {
    Serial.printf("[Ethernet] Link %s\n", up ? "UP" : "DOWN");
    blinkStatus(up ? 1 : 3);
}

void Diagnostics::blinkStatus(uint8_t pattern) {
    for (uint8_t i = 0; i < pattern; ++i) {
        digitalWrite(pinmap::STATUS_LED, HIGH);
        delay(100);
        digitalWrite(pinmap::STATUS_LED, LOW);
        delay(100);
    }
}
