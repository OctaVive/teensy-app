#pragma once

#include "effects/IEffect.h"

namespace effects {

class EffectRegistry {
public:
    static constexpr uint8_t MAX_EFFECTS = 16;

    void registerEffect(IEffect* effect);
    IEffect* get(uint8_t id) const;
    uint8_t count() const { return count_; }

private:
    IEffect* effects_[MAX_EFFECTS] = {};
    uint8_t count_ = 0;
};

}  // namespace effects
