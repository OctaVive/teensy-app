#pragma once

#include "types.h"

namespace net {

class UniverseManager {
public:
    void begin(const SacnConfig& sacn, GlobalMode mode);
    void updateUniverse(uint16_t universe, const uint8_t* data, uint16_t len, uint32_t nowMs);
    const UniverseBuffer* getBuffer(uint16_t universe) const;
    UniverseBuffer* findBuffer(uint16_t universe);
    void tick(uint32_t nowMs);
    bool hasValidPixelData() const;
    bool hasValidEffectData() const;
    const uint8_t* effectUniverseData() const;
    uint16_t subscribedUniverseCount() const { return bufferCount_; }

private:
    void allocateBuffers(GlobalMode mode);
    SacnConfig config_;
    GlobalMode mode_ = GlobalMode::Pixel;
    UniverseBuffer buffers_[32];
    uint8_t bufferCount_ = 0;
};

}  // namespace net
