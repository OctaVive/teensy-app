#include "led/LED_OutputEngine.h"

namespace led {

bool VirtualPixelBuffer::begin(const LedOutputConfig& cfg) {
    config_ = cfg;
    count_ = cfg.totalLeds;
    clear(CRGB::Black);
    return count_ > 0;
}

void VirtualPixelBuffer::clear(CRGB color) {
    for (uint16_t i = 0; i < count_; ++i) {
        leds_[i] = color;
    }
}

uint16_t VirtualPixelBuffer::stripStartIndex(uint8_t stripIndex) const {
    uint16_t offset = 0;
    for (uint8_t i = 0; i < stripIndex && i < NUM_STRIPS; ++i) {
        offset += config_.strips[i].ledCount;
    }
    return offset;
}

uint16_t VirtualPixelBuffer::stripLedCount(uint8_t stripIndex) const {
    if (stripIndex >= NUM_STRIPS) {
        return 0;
    }
    return config_.strips[stripIndex].ledCount;
}

namespace {

template<uint8_t PIN>
void addStripController(CRGB* leds, uint16_t offset, uint16_t count) {
    FastLED.addLeds<WS2812, PIN, GRB>(leds, offset, count);
}

void registerStripByPin(uint8_t pin, CRGB* leds, uint16_t offset, uint16_t count) {
    switch (pin) {
        case 2: addStripController<2>(leds, offset, count); break;
        case 3: addStripController<3>(leds, offset, count); break;
        case 4: addStripController<4>(leds, offset, count); break;
        case 5: addStripController<5>(leds, offset, count); break;
        case 6: addStripController<6>(leds, offset, count); break;
        case 9: addStripController<9>(leds, offset, count); break;
        case 10: addStripController<10>(leds, offset, count); break;
        case 11: addStripController<11>(leds, offset, count); break;
        case 12: addStripController<12>(leds, offset, count); break;
        case 16: addStripController<16>(leds, offset, count); break;
        default: break;
    }
}

}  // namespace

bool LED_OutputEngine::begin(const LedOutputConfig& cfg, CRGB* leds) {
    config_ = cfg;
    leds_ = leds;
    FastLED.clear(true);

    uint16_t offset = 0;
    for (uint8_t i = 0; i < NUM_STRIPS; ++i) {
        const uint16_t count = cfg.strips[i].ledCount;
        registerStripByPin(cfg.strips[i].pin, leds_, offset, count);
        offset += count;
    }

    FastLED.setBrightness(brightness_);
    FastLED.setMaxRefreshRate(0, true);
    initialized_ = offset > 0;
    return initialized_;
}

void LED_OutputEngine::show() {
    if (initialized_) {
        FastLED.show();
    }
}

bool LED_OutputEngine::isShowing() const {
    return false;
}

void LED_OutputEngine::setBrightness(uint8_t brightness) {
    brightness_ = brightness;
    FastLED.setBrightness(brightness_);
}

}  // namespace led
