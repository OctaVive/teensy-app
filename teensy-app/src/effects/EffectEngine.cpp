#include "effects/EffectEngine.h"
#include "effects/IEffect.h"

#include <FastLED.h>

namespace effects {

namespace {

class OffEffect : public IEffect {
public:
    uint8_t id() const override { return 0; }
    const char* name() const override { return "Off"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams&,
                uint32_t) override {
        for (uint16_t i = 0; i < count; ++i) {
            leds[start + i] = CRGB::Black;
        }
    }
};

class SolidColorEffect : public IEffect {
public:
    uint8_t id() const override { return 1; }
    const char* name() const override { return "SolidColor"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t) override {
        for (uint16_t i = 0; i < count; ++i) {
            leds[start + i] = params.color;
        }
    }
};

class RainbowEffect : public IEffect {
public:
    uint8_t id() const override { return 2; }
    const char* name() const override { return "Rainbow"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        const uint8_t speed = map(params.speed, 0, 255, 1, 40);
        for (uint16_t i = 0; i < count; ++i) {
            leds[start + i] = CHSV(static_cast<uint8_t>((start + i) * 2 + timeMs / speed), 255, 255);
        }
    }
};

class RainbowCycleEffect : public IEffect {
public:
    uint8_t id() const override { return 3; }
    const char* name() const override { return "RainbowCycle"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        const uint8_t speed = map(params.speed, 0, 255, 1, 60);
        const uint8_t hue = static_cast<uint8_t>(timeMs / speed);
        for (uint16_t i = 0; i < count; ++i) {
            leds[start + i] = CHSV(static_cast<uint8_t>(hue + (start + i) * 256 / count), 255, 255);
        }
    }
};

class ConfettiEffect : public IEffect {
public:
    uint8_t id() const override { return 4; }
    const char* name() const override { return "Confetti"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        fadeToBlackBy(&leds[start], count, 20);
        const uint8_t bursts = map(params.speed, 0, 255, 1, 8);
        for (uint8_t b = 0; b < bursts; ++b) {
            const uint16_t idx = start + random16(count);
            leds[idx] += params.color;
        }
        (void)timeMs;
    }
};

class SinelonEffect : public IEffect {
public:
    uint8_t id() const override { return 5; }
    const char* name() const override { return "Sinelon"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        fadeToBlackBy(&leds[start], count, 20);
        const uint8_t speed = map(params.speed, 0, 255, 2, 40);
        const uint16_t pos = start + (timeMs / speed) % count;
        leds[pos] = params.color;
    }
};

class BpmEffect : public IEffect {
public:
    uint8_t id() const override { return 6; }
    const char* name() const override { return "BPM"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        const uint8_t speed = map(params.speed, 0, 255, 20, 600);
        const uint8_t beat = beatsin8(60, 64, 255, 0, speed);
        for (uint16_t i = 0; i < count; ++i) {
            leds[start + i] = params.color;
            leds[start + i].fadeToBlackBy(255 - beat);
        }
        (void)timeMs;
    }
};

class JuggleEffect : public IEffect {
public:
    uint8_t id() const override { return 7; }
    const char* name() const override { return "Juggle"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        fadeToBlackBy(&leds[start], count, 20);
        const uint8_t speed = map(params.speed, 0, 255, 1, 20);
        const CHSV hsv = rgb2hsv_approximate(params.color);
        for (uint8_t i = 0; i < 3; ++i) {
            const uint16_t idx = start + (timeMs / speed + i * (count / 3)) % count;
            leds[idx] = CHSV(static_cast<uint8_t>(hsv.h + i * 80), 255, 255);
        }
    }
};

class FireEffect : public IEffect {
public:
    uint8_t id() const override { return 8; }
    const char* name() const override { return "Fire2012"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t) override {
        const uint8_t heatSpread = map(params.speed, 0, 255, 40, 90);
        static uint8_t heat[TOTAL_LEDS];
        for (uint16_t i = 0; i < count; ++i) {
            heat[start + i] = qsub8(heat[start + i], random8(0, heatSpread / 10));
        }
        for (int16_t k = static_cast<int16_t>(count) - 1; k >= 2; --k) {
            heat[start + k] = (heat[start + k - 1] + heat[start + k - 2] + heat[start + k - 2]) / 3;
        }
        if (random8() < heatSpread) {
            heat[start] = qadd8(heat[start], random8(160, 255));
        }
        for (uint16_t j = 0; j < count; ++j) {
            leds[start + j] = HeatColor(heat[start + j]);
        }
    }
};

class TwinkleEffect : public IEffect {
public:
    uint8_t id() const override { return 9; }
    const char* name() const override { return "Twinkle"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t) override {
        fadeToBlackBy(&leds[start], count, map(params.speed, 0, 255, 5, 40));
        if (random8() < 120) {
            leds[start + random16(count)] += params.color;
        }
    }
};

class NoiseEffect : public IEffect {
public:
    uint8_t id() const override { return 10; }
    const char* name() const override { return "Noise"; }
    void render(CRGB* leds, uint16_t start, uint16_t count, const EffectParams& params,
                uint32_t timeMs) override {
        const uint8_t speed = map(params.speed, 0, 255, 1, 30);
        for (uint16_t i = 0; i < count; ++i) {
            const uint8_t n = inoise8((start + i) * 30, timeMs * speed);
            leds[start + i] = CHSV(n, 255, 255);
        }
        (void)params;
    }
};

OffEffect offEffect;
SolidColorEffect solidEffect;
RainbowEffect rainbowEffect;
RainbowCycleEffect rainbowCycleEffect;
ConfettiEffect confettiEffect;
SinelonEffect sinelonEffect;
BpmEffect bpmEffect;
JuggleEffect juggleEffect;
FireEffect fireEffect;
TwinkleEffect twinkleEffect;
NoiseEffect noiseEffect;

IEffect* builtinEffects[] = {&offEffect,        &solidEffect,     &rainbowEffect, &rainbowCycleEffect,
                             &confettiEffect,   &sinelonEffect,   &bpmEffect,     &juggleEffect,
                             &fireEffect,       &twinkleEffect,   &noiseEffect};

}  // namespace

void EffectEngine::begin(const NodeConfig& cfg, CRGB* leds, uint16_t count) {
    config_ = cfg;
    leds_ = leds;
    ledCount_ = count;
    startMs_ = millis();

    for (IEffect* effect : builtinEffects) {
        registry_.registerEffect(effect);
    }

    buildInstances(cfg);
}

void EffectEngine::buildInstances(const NodeConfig& cfg) {
    instanceCount_ = 0;

    if (cfg.effectGroupCount > 0) {
        for (uint8_t g = 0; g < cfg.effectGroupCount && instanceCount_ < MAX_EFFECT_GROUPS; ++g) {
            const EffectGroupConfig& group = cfg.effectGroups[g];
            EffectInstance& inst = instances_[instanceCount_++];
            inst.targetType = group.type;
            inst.params = EffectParams{};
            inst.stripIndex = 0;

            if (group.type == EffectTargetType::VirtualAll) {
                inst.startIndex = 0;
                inst.ledCount = ledCount_;
            } else if (group.type == EffectTargetType::Group) {
                inst.startIndex = 0;
                inst.ledCount = 0;
                for (uint8_t s = 0; s < NUM_STRIPS; ++s) {
                    if (group.stripMask & (1u << s)) {
                        if (inst.ledCount == 0) {
                            uint16_t offset = 0;
                            for (uint8_t i = 0; i < s; ++i) {
                                offset += cfg.leds.strips[i].ledCount;
                            }
                            inst.startIndex = offset;
                        }
                        inst.ledCount += cfg.leds.strips[s].ledCount;
                    }
                }
            } else {
                continue;
            }
        }
    }

    for (uint8_t s = 0; s < NUM_STRIPS && instanceCount_ < (MAX_EFFECT_GROUPS + NUM_STRIPS); ++s) {
        EffectInstance& inst = instances_[instanceCount_++];
        inst.targetType = EffectTargetType::Individual;
        inst.stripIndex = s;
        uint16_t offset = 0;
        for (uint8_t i = 0; i < s; ++i) {
            offset += cfg.leds.strips[i].ledCount;
        }
        inst.startIndex = offset;
        inst.ledCount = cfg.leds.strips[s].ledCount;
        inst.params = EffectParams{};
    }
}

EffectParams paramsFromChannels(const uint8_t* data, uint16_t len, uint8_t channelOffset) {
    EffectParams params;
    if (len < channelOffset + EFFECT_CHANNELS_PER_STRIP) {
        return params;
    }
    const uint8_t* ch = &data[channelOffset];
    params.color = CRGB(ch[0], ch[1], ch[2]);
    params.programId = ch[3];
    params.dimmer = ch[4];
    params.speed = ch[5];
    params.active = params.programId > 0;
    params.lastPacketMs = millis();
    return params;
}

void EffectEngine::applyParamsFromUniverse(const uint8_t* data, uint16_t len) {
    for (uint8_t i = 0; i < instanceCount_; ++i) {
        EffectInstance& inst = instances_[i];
        uint8_t offset = 0;
        if (inst.targetType == EffectTargetType::Individual) {
            offset = static_cast<uint8_t>(inst.stripIndex * EFFECT_CHANNELS_PER_STRIP);
        } else if (inst.targetType == EffectTargetType::VirtualAll) {
            offset = 0;
        } else {
            continue;
        }
        inst.params = paramsFromChannels(data, len, offset);
        inst.programId = inst.params.programId;
        inst.effect = registry_.get(inst.programId);
    }
}

void EffectEngine::applyDimmer(CRGB* leds, uint16_t start, uint16_t count, uint8_t dimmer) {
    if (dimmer >= 255) {
        return;
    }
    for (uint16_t i = 0; i < count; ++i) {
        leds[start + i].fadeToBlackBy(255 - dimmer);
    }
}

void EffectEngine::renderInstance(EffectInstance& inst, uint32_t nowMs) {
    if (!inst.effect) {
        inst.effect = registry_.get(0);
    }
    if (!inst.effect || inst.ledCount == 0) {
        return;
    }
    inst.effect->render(leds_, inst.startIndex, inst.ledCount, inst.params, nowMs - startMs_);
    applyDimmer(leds_, inst.startIndex, inst.ledCount, inst.params.dimmer);
}

void EffectEngine::tick(uint32_t nowMs, const net::UniverseManager& universes) {
    const uint8_t* data = universes.effectUniverseData();
    if (data) {
        applyParamsFromUniverse(data, DMX_SLOTS);
    }

    for (uint8_t i = 0; i < instanceCount_; ++i) {
        if (instances_[i].targetType == EffectTargetType::VirtualAll) {
            renderInstance(instances_[i], nowMs);
            return;
        }
    }

    for (uint8_t i = 0; i < instanceCount_; ++i) {
        if (instances_[i].targetType != EffectTargetType::VirtualAll) {
            renderInstance(instances_[i], nowMs);
        }
    }
}

}  // namespace effects
