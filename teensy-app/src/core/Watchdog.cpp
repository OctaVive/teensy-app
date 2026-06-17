#include "core/Watchdog.h"

#include <Arduino.h>
#include <imxrt.h>

void Watchdog::begin(uint32_t timeoutMs) {
    timeoutMs_ = timeoutMs;
    lastFeedMs_ = millis();
    enabled_ = true;
    Serial.printf("[Watchdog] Software watchdog enabled (%ums)\n", timeoutMs_);
}

void Watchdog::feed() {
    lastFeedMs_ = millis();
}

void Watchdog::check() {
    if (!enabled_) {
        return;
    }
    if ((millis() - lastFeedMs_) > timeoutMs_) {
        Serial.println("[Watchdog] TIMEOUT - resetting");
        delay(50);
        SCB_AIRCR = 0x05FA0004;
        while (true) {
        }
    }
}
