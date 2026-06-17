#include "config/ConfigManager.h"
#include "pinmap.h"

#include <ArduinoJson.h>
#include <SdFat.h>
#include <stddef.h>
#include <string.h>

namespace config {

namespace {

SdFat sd;
constexpr const char* CONFIG_PATH = "config.json";

GlobalMode parseMode(const char* value) {
    if (value && strcmp(value, "effect") == 0) {
        return GlobalMode::Effect;
    }
    return GlobalMode::Pixel;
}

DmxDirection parseDirection(const char* value) {
    if (value && strcmp(value, "output") == 0) {
        return DmxDirection::Output;
    }
    return DmxDirection::Input;
}

EffectTargetType parseTargetType(const char* value) {
    if (!value) {
        return EffectTargetType::Individual;
    }
    if (strcmp(value, "group") == 0) {
        return EffectTargetType::Group;
    }
    if (strcmp(value, "virtualAll") == 0) {
        return EffectTargetType::VirtualAll;
    }
    return EffectTargetType::Individual;
}

uint16_t computeUniverseCount(uint16_t totalLeds) {
    return static_cast<uint16_t>((totalLeds + PIXELS_PER_UNIVERSE - 1) / PIXELS_PER_UNIVERSE);
}

}  // namespace

void applyDefaults(NodeConfig& cfg) {
    cfg.mode = GlobalMode::Pixel;
    cfg.configVersion = 1;
    cfg.configCrc = 0;
    cfg.loadedFromSd = false;

    cfg.sacn = SacnConfig{};
    cfg.sacn.startUniverse = 1;
    cfg.sacn.universeCount = computeUniverseCount(TOTAL_LEDS);
    cfg.sacn.effectUniverse = 100;
    cfg.sacn.virtualAllUniverse = 101;
    cfg.sacn.priority = 100;
    cfg.sacn.multicast = true;
    strncpy(cfg.sacn.ip, "192.168.1.177", sizeof(cfg.sacn.ip) - 1);
    strncpy(cfg.sacn.subnet, "255.255.255.0", sizeof(cfg.sacn.subnet) - 1);
    strncpy(cfg.sacn.gateway, "192.168.1.1", sizeof(cfg.sacn.gateway) - 1);

    cfg.leds.totalLeds = 0;
    for (uint8_t i = 0; i < NUM_STRIPS; ++i) {
        cfg.leds.strips[i].pin = pinmap::LED_STRIP_PINS[i];
        cfg.leds.strips[i].ledCount = LEDS_PER_STRIP;
        cfg.leds.strips[i].serpentine = false;
        cfg.leds.totalLeds += cfg.leds.strips[i].ledCount;
    }
    cfg.sacn.universeCount = computeUniverseCount(cfg.leds.totalLeds);

    for (uint8_t i = 0; i < DMX_PORT_COUNT; ++i) {
        cfg.dmxPorts[i].direction = DmxDirection::Input;
        cfg.dmxPorts[i].outputRefreshHz = 44;
        cfg.dmxPorts[i].inputTimeoutMs = 500;
    }

    cfg.effectGroupCount = 1;
    cfg.effectGroups[0].type = EffectTargetType::VirtualAll;
    cfg.effectGroups[0].stripMask = 0x3FF;
    cfg.effectGroups[0].sacnChannelOffset = 0;
    strncpy(cfg.effectGroups[0].name, "VirtualAll", sizeof(cfg.effectGroups[0].name) - 1);
}

uint32_t computeCrc(const NodeConfig& cfg) {
    uint32_t crc = 0xFFFFFFFFu;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&cfg);
    constexpr size_t skipOffset = offsetof(NodeConfig, configCrc);
    for (size_t i = 0; i < skipOffset; ++i) {
        crc ^= bytes[i];
        for (uint8_t bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xEDB88320u & (-(crc & 1u)));
        }
    }
    return ~crc;
}

bool validate(const NodeConfig& cfg) {
    if (cfg.leds.totalLeds == 0 || cfg.leds.totalLeds > TOTAL_LEDS) {
        return false;
    }
    uint16_t sum = 0;
    for (uint8_t i = 0; i < NUM_STRIPS; ++i) {
        if (cfg.leds.strips[i].ledCount == 0) {
            return false;
        }
        sum += cfg.leds.strips[i].ledCount;
    }
    if (sum != cfg.leds.totalLeds) {
        return false;
    }
    if (cfg.sacn.universeCount == 0 || cfg.sacn.universeCount > 32) {
        return false;
    }
    return true;
}

bool ConfigManager::begin() {
    applyDefaults(config_);
    if (!loadFromSd()) {
        Serial.println("[Config] Using embedded defaults (SD load failed)");
    }
    if (!validate(config_)) {
        Serial.println("[Config] Validation failed, reverting to defaults");
        applyDefaults(config_);
    }
    config_.configCrc = computeCrc(config_);
    return true;
}

bool ConfigManager::loadFromSd() {
    if (!sd.begin(SdioConfig(FIFO_SDIO))) {
        return false;
    }

    FsFile file = sd.open(CONFIG_PATH, O_RDONLY);
    if (!file) {
        return false;
    }

    const size_t size = file.size();
    if (size == 0 || size > 8192) {
        file.close();
        return false;
    }

    char* buffer = new char[size + 1];
    if (!buffer) {
        file.close();
        return false;
    }

    file.read(buffer, size);
    buffer[size] = '\0';
    file.close();

    const bool ok = parseJson(buffer, size);
    delete[] buffer;
    return ok;
}

bool ConfigManager::parseJson(const char* json, size_t len) {
    JsonDocument doc;
    const DeserializationError err = deserializeJson(doc, json, len);
    if (err) {
        Serial.printf("[Config] JSON error: %s\n", err.c_str());
        return false;
    }

    NodeConfig parsed;
    applyDefaults(parsed);

    const char* mode = doc["mode"] | "pixel";
    parsed.mode = parseMode(mode);

    JsonObject sacn = doc["sacn"];
    if (!sacn.isNull()) {
        parsed.sacn.startUniverse = sacn["startUniverse"] | parsed.sacn.startUniverse;
        parsed.sacn.universeCount = sacn["universeCount"] | parsed.sacn.universeCount;
        parsed.sacn.effectUniverse = sacn["effectUniverse"] | parsed.sacn.effectUniverse;
        parsed.sacn.virtualAllUniverse = sacn["virtualAllUniverse"] | parsed.sacn.virtualAllUniverse;
        parsed.sacn.priority = sacn["priority"] | parsed.sacn.priority;
        parsed.sacn.multicast = sacn["multicast"] | parsed.sacn.multicast;
        const char* ip = sacn["ip"];
        if (ip) {
            strncpy(parsed.sacn.ip, ip, sizeof(parsed.sacn.ip) - 1);
        }
        const char* subnet = sacn["subnet"];
        if (subnet) {
            strncpy(parsed.sacn.subnet, subnet, sizeof(parsed.sacn.subnet) - 1);
        }
        const char* gateway = sacn["gateway"];
        if (gateway) {
            strncpy(parsed.sacn.gateway, gateway, sizeof(parsed.sacn.gateway) - 1);
        }
    }

    JsonArray strips = doc["leds"]["strips"];
    if (!strips.isNull()) {
        parsed.leds.totalLeds = 0;
        uint8_t index = 0;
        for (JsonObject strip : strips) {
            if (index >= NUM_STRIPS) {
                break;
            }
            parsed.leds.strips[index].pin = strip["pin"] | pinmap::LED_STRIP_PINS[index];
            parsed.leds.strips[index].ledCount = strip["ledCount"] | LEDS_PER_STRIP;
            parsed.leds.strips[index].serpentine = strip["serpentine"] | false;
            parsed.leds.totalLeds += parsed.leds.strips[index].ledCount;
            ++index;
        }
        parsed.sacn.universeCount = computeUniverseCount(parsed.leds.totalLeds);
    }

    JsonArray dmxPorts = doc["dmx"]["ports"];
    if (!dmxPorts.isNull()) {
        uint8_t index = 0;
        for (JsonObject port : dmxPorts) {
            if (index >= DMX_PORT_COUNT) {
                break;
            }
            const char* direction = port["direction"];
            parsed.dmxPorts[index].direction = parseDirection(direction);
            parsed.dmxPorts[index].outputRefreshHz = port["outputRefreshHz"] | 44;
            parsed.dmxPorts[index].inputTimeoutMs = port["inputTimeoutMs"] | 500;
            ++index;
        }
    }

    JsonArray groups = doc["effectGroups"];
    if (!groups.isNull()) {
        parsed.effectGroupCount = 0;
        for (JsonObject group : groups) {
            if (parsed.effectGroupCount >= MAX_EFFECT_GROUPS) {
                break;
            }
            EffectGroupConfig& g = parsed.effectGroups[parsed.effectGroupCount];
            const char* name = group["name"];
            if (name) {
                strncpy(g.name, name, sizeof(g.name) - 1);
            }
            const char* type = group["type"];
            g.type = parseTargetType(type);
            g.stripMask = group["stripMask"] | 0;
            g.sacnChannelOffset = group["sacnChannelOffset"] | 0;
            ++parsed.effectGroupCount;
        }
    }

    parsed.configVersion = doc["configVersion"] | 1u;
    parsed.loadedFromSd = true;
    config_ = parsed;
    Serial.println("[Config] Loaded config.json from SD");
    return true;
}

}  // namespace config
