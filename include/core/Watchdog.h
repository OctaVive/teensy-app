#pragma once

#include "types.h"
#include <stdint.h>

class Watchdog {
public:
    void begin(uint32_t timeoutMs = WATCHDOG_TIMEOUT_MS);
    void feed();
    void check();
    bool enabled() const { return enabled_; }

private:
    bool enabled_ = false;
    uint32_t timeoutMs_ = WATCHDOG_TIMEOUT_MS;
    uint32_t lastFeedMs_ = 0;
};
