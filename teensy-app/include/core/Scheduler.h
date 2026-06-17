#pragma once

#include "types.h"

class Scheduler {
public:
    using FrameCallback = void (*)(void* ctx);

    void begin(uint32_t intervalUs = FRAME_INTERVAL_US);
    void setCallback(FrameCallback cb, void* ctx);
    bool tick();
    FrameStats& stats() { return stats_; }
    const FrameStats& stats() const { return stats_; }

private:
    FrameCallback callback_ = nullptr;
    void* context_ = nullptr;
    uint32_t intervalUs_ = FRAME_INTERVAL_US;
    uint32_t lastFrameUs_ = 0;
    FrameStats stats_;
};
