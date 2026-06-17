#pragma once

#include "types.h"

namespace dmx {

class DmxManager {
public:
    bool begin(const DmxPortConfig ports[DMX_PORT_COUNT]);
    void poll(uint32_t nowMs);
    DmxPortState* port(uint8_t index);
    const DmxPortState* port(uint8_t index) const;

private:
    bool initPort(uint8_t index, const DmxPortConfig& cfg);
    DmxPortState ports_[DMX_PORT_COUNT];
};

}  // namespace dmx
