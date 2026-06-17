#pragma once

#include <FastLED.h>
#include <stdint.h>

#ifndef NUM_STRIPS
#define NUM_STRIPS 10
#endif

#ifndef LEDS_PER_STRIP
#define LEDS_PER_STRIP 300
#endif

constexpr uint16_t TOTAL_LEDS = NUM_STRIPS * LEDS_PER_STRIP;
constexpr uint8_t DMX_PORT_COUNT = 8;
constexpr uint16_t DMX_SLOTS = 512;
constexpr uint8_t PIXELS_PER_UNIVERSE = 170;
constexpr uint8_t EFFECT_CHANNELS_PER_STRIP = 6;
constexpr uint8_t MAX_EFFECT_GROUPS = 16;
constexpr uint32_t SACN_TIMEOUT_MS = 1000;
constexpr uint32_t FRAME_INTERVAL_US = 25000;  // 40 fps
constexpr uint32_t WATCHDOG_TIMEOUT_MS = 2000;

enum class GlobalMode : uint8_t { Pixel, Effect };

enum class DmxDirection : uint8_t { Input, Output };

enum class EffectTargetType : uint8_t { Individual, Group, VirtualAll };

struct SacnConfig {
    uint16_t startUniverse = 1;
    uint16_t universeCount = 18;
    uint16_t effectUniverse = 100;
    uint16_t virtualAllUniverse = 101;
    uint8_t priority = 100;
    bool multicast = true;
    char ip[16] = "192.168.1.177";
    char subnet[16] = "255.255.255.0";
    char gateway[16] = "192.168.1.1";
};

struct LedStripConfig {
    uint8_t pin = 2;
    uint16_t ledCount = LEDS_PER_STRIP;
    bool serpentine = false;
};

struct LedOutputConfig {
    static constexpr uint8_t MAX_STRIPS = NUM_STRIPS;
    LedStripConfig strips[MAX_STRIPS];
    uint16_t totalLeds = TOTAL_LEDS;
};

struct DmxPortConfig {
    DmxDirection direction = DmxDirection::Input;
    uint16_t outputRefreshHz = 44;
    uint16_t inputTimeoutMs = 500;
};

struct EffectGroupConfig {
    char name[16] = {};
    EffectTargetType type = EffectTargetType::Individual;
    uint16_t stripMask = 0;
    uint8_t sacnChannelOffset = 0;
};

struct NodeConfig {
    GlobalMode mode = GlobalMode::Pixel;
    SacnConfig sacn;
    LedOutputConfig leds;
    DmxPortConfig dmxPorts[DMX_PORT_COUNT];
    EffectGroupConfig effectGroups[MAX_EFFECT_GROUPS];
    uint8_t effectGroupCount = 0;
    uint32_t configVersion = 1;
    uint32_t configCrc = 0;
    bool loadedFromSd = false;
};

struct EffectParams {
    CRGB color = CRGB::Black;
    uint8_t programId = 0;
    uint8_t dimmer = 255;
    uint8_t speed = 128;
    uint32_t lastPacketMs = 0;
    bool active = false;
};

namespace effects {
class IEffect;
}

struct EffectInstance {
    EffectTargetType targetType = EffectTargetType::Individual;
    uint16_t startIndex = 0;
    uint16_t ledCount = 0;
    EffectParams params;
    uint8_t programId = 0;
    effects::IEffect* effect = nullptr;
    uint8_t stripIndex = 0;
};

struct UniverseBuffer {
    uint16_t universe = 0;
    uint8_t data[DMX_SLOTS] = {};
    uint32_t lastSeenMs = 0;
    bool valid = false;
    uint8_t sequence = 0;
};

struct PixelBuffer {
    CRGB* leds = nullptr;
    uint16_t count = 0;
};

struct DmxPortState {
    DmxPortConfig config;
    uint8_t buffer[DMX_SLOTS] = {};
    uint32_t lastActivityMs = 0;
    bool active = false;
};

struct FrameStats {
    uint32_t frameCount = 0;
    uint32_t lastFrameUs = 0;
    uint32_t avgFrameUs = 0;
    uint32_t sacnPacketsReceived = 0;
    uint32_t sacnPacketsDropped = 0;
    float outputFps = 0.0f;
};
