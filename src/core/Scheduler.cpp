#include "core/Scheduler.h"

#include <Arduino.h>

void Scheduler::begin(uint32_t intervalUs) {
    intervalUs_ = intervalUs;
    lastFrameUs_ = micros();
    stats_ = FrameStats{};
}

void Scheduler::setCallback(FrameCallback cb, void* ctx) {
    callback_ = cb;
    context_ = ctx;
}

bool Scheduler::tick() {
    const uint32_t now = micros();
    if ((now - lastFrameUs_) < intervalUs_) {
        return false;
    }

    const uint32_t frameUs = now - lastFrameUs_;
    lastFrameUs_ = now;

    stats_.frameCount++;
    stats_.lastFrameUs = frameUs;
    stats_.avgFrameUs = (stats_.avgFrameUs * 7 + frameUs) / 8;
    stats_.outputFps = 1000000.0f / static_cast<float>(frameUs);

    if (callback_) {
        callback_(context_);
    }
    return true;
}
