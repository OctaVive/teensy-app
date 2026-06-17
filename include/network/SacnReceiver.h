#pragma once

#include "types.h"
#include <stddef.h>
#include <stdint.h>

namespace net {

struct SacnPacketInfo {
    uint16_t universe = 0;
    uint8_t priority = 0;
    uint8_t sequence = 0;
    const uint8_t* dmxData = nullptr;
    uint16_t dmxLength = 0;
    bool valid = false;
};

using SacnPacketCallback = void (*)(const SacnPacketInfo& info, void* context);

class SacnReceiver {
public:
    bool begin(const SacnConfig& sacn, GlobalMode mode);
    void poll();
    void setCallback(SacnPacketCallback cb, void* context);
    bool parsePacket(const uint8_t* data, size_t len, SacnPacketInfo& out);

private:
    void subscribeUniverse(uint16_t universe);

    SacnConfig config_;
    GlobalMode mode_ = GlobalMode::Pixel;
    SacnPacketCallback callback_ = nullptr;
    void* callbackContext_ = nullptr;
    uint8_t lastSequence_[256] = {};
};

}  // namespace net
