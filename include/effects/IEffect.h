#pragma once

#include "types.h"
#include <FastLED.h>

namespace effects {

class IEffect {
public:
    virtual ~IEffect() = default;
    virtual uint8_t id() const = 0;
    virtual const char* name() const = 0;
    virtual void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                        uint32_t timeMs) = 0;
};

}  // namespace effects
