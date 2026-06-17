#include "pixel/PixelMapper.h"

namespace pixel {

void PixelMapper::begin(const SacnConfig& sacn, const LedOutputConfig& leds) {
    sacn_ = sacn;
    leds_ = leds;
}

void PixelMapper::mapToBuffer(const net::UniverseManager& universes, CRGB* leds, uint16_t count) {
    for (uint16_t u = 0; u < sacn_.universeCount; ++u) {
        const uint16_t universe = static_cast<uint16_t>(sacn_.startUniverse + u);
        const UniverseBuffer* buffer = universes.getBuffer(universe);
        if (!buffer || !buffer->valid) {
            continue;
        }

        const uint16_t basePixel = static_cast<uint16_t>(u * PIXELS_PER_UNIVERSE);
        for (uint8_t slot = 0; slot < PIXELS_PER_UNIVERSE * 3; slot += 3) {
            const uint16_t pixelIndex = static_cast<uint16_t>(basePixel + slot / 3);
            if (pixelIndex >= count) {
                break;
            }
            leds[pixelIndex].r = buffer->data[slot];
            leds[pixelIndex].g = buffer->data[slot + 1];
            leds[pixelIndex].b = buffer->data[slot + 2];
        }
    }
}

}  // namespace pixel
