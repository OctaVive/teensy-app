#pragma once

#include "core/Scheduler.h"
#include "types.h"

class Diagnostics {
public:
    void begin();
    void logBoot(const NodeConfig& cfg);
    void logFrame(const FrameStats& stats, GlobalMode mode);
    void logSacnPacket(uint16_t universe, bool accepted);
    void logLinkChange(bool up);
    void blinkStatus(uint8_t pattern);

private:
    uint32_t lastLogMs_ = 0;
};
