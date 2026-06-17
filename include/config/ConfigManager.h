#pragma once

#include "types.h"

namespace config {

void applyDefaults(NodeConfig& cfg);
uint32_t computeCrc(const NodeConfig& cfg);
bool validate(const NodeConfig& cfg);

class ConfigManager {
public:
    bool begin();
    const NodeConfig& get() const { return config_; }
    NodeConfig& mutableConfig() { return config_; }

private:
    bool loadFromSd();
    bool parseJson(const char* json, size_t len);
    NodeConfig config_;
};

}  // namespace config
