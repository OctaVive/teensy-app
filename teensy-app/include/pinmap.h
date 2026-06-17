#pragma once

#include <stdint.h>

namespace pinmap {

constexpr uint8_t LED_STRIP_PINS[] = {2, 3, 4, 5, 6, 9, 10, 11, 12, 16};
constexpr uint8_t NUM_LED_PINS = sizeof(LED_STRIP_PINS) / sizeof(LED_STRIP_PINS[0]);

constexpr uint8_t STATUS_LED = 13;

}  // namespace pinmap
