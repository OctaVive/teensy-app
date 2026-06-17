#pragma once

#include "types.h"

class ModeController {
public:
    void begin(GlobalMode mode);
    GlobalMode mode() const { return mode_; }
    bool isPixelMode() const { return mode_ == GlobalMode::Pixel; }
    bool isEffectMode() const { return mode_ == GlobalMode::Effect; }

private:
    GlobalMode mode_ = GlobalMode::Pixel;
};
