#pragma once

#include "types.h"

namespace led {

class VirtualPixelBuffer {
public:
    bool begin(const LedOutputConfig& cfg);
    CRGB* data() { return leds_; }
    const CRGB* data() const { return leds_; }
    uint16_t count() const { return count_; }
    void clear(CRGB color = CRGB::Black);
    uint16_t stripStartIndex(uint8_t stripIndex) const;
    uint16_t stripLedCount(uint8_t stripIndex) const;

private:
    CRGB leds_[TOTAL_LEDS];
    LedOutputConfig config_;
    uint16_t count_ = TOTAL_LEDS;
};

class LED_OutputEngine {
public:
    bool begin(const LedOutputConfig& cfg, CRGB* leds);
    void show();
    bool isShowing() const;
    void setBrightness(uint8_t brightness);
    uint8_t brightness() const { return brightness_; }

private:
    void registerStrip(uint8_t index, uint8_t pin, CRGB* leds, uint16_t count);
    LedOutputConfig config_;
    CRGB* leds_ = nullptr;
    uint8_t brightness_ = 255;
    bool initialized_ = false;
};

}  // namespace led
