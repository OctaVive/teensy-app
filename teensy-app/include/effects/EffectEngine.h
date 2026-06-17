#pragma once

#include "effects/EffectRegistry.h"
#include "network/UniverseManager.h"
#include "types.h"

namespace effects {

class EffectEngine {
public:
    void begin(const NodeConfig& cfg, CRGB* leds, uint16_t count);
    void tick(uint32_t nowMs, const net::UniverseManager& universes);
    void buildInstances(const NodeConfig& cfg);

private:
    void applyParamsFromUniverse(const uint8_t* data, uint16_t len);
    void renderInstance(EffectInstance& inst, uint32_t nowMs);
    void applyDimmer(CRGB* leds, uint16_t start, uint16_t count, uint8_t dimmer);

    EffectRegistry registry_;
    EffectInstance instances_[MAX_EFFECT_GROUPS + NUM_STRIPS + 1];
    uint8_t instanceCount_ = 0;
    CRGB* leds_ = nullptr;
    uint16_t ledCount_ = 0;
    NodeConfig config_;
    uint32_t startMs_ = 0;
};

}  // namespace effects
