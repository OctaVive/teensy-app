#pragma once

#include "network/UniverseManager.h"
#include "types.h"

namespace pixel {

class PixelMapper {
public:
    void begin(const SacnConfig& sacn, const LedOutputConfig& leds);
    void mapToBuffer(const net::UniverseManager& universes, CRGB* leds, uint16_t count);

private:
    SacnConfig sacn_;
    LedOutputConfig leds_;
};

}  // namespace pixel
