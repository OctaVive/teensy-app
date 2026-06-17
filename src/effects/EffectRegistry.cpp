#include "effects/EffectRegistry.h"

namespace effects {

void EffectRegistry::registerEffect(IEffect* effect) {
    if (!effect || count_ >= MAX_EFFECTS) {
        return;
    }
    effects_[count_++] = effect;
}

IEffect* EffectRegistry::get(uint8_t id) const {
    for (uint8_t i = 0; i < count_; ++i) {
        if (effects_[i] && effects_[i]->id() == id) {
            return effects_[i];
        }
    }
    return effects_[0];
}

}  // namespace effects
