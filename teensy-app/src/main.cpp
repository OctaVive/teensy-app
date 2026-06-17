#include "config/ConfigManager.h"
#include "core/Diagnostics.h"
#include "core/ModeController.h"
#include "core/Scheduler.h"
#include "core/Watchdog.h"
#include "dmx/DmxManager.h"
#include "effects/EffectEngine.h"
#include "led/LED_OutputEngine.h"
#include "network/EthernetManager.h"
#include "network/SacnReceiver.h"
#include "network/UniverseManager.h"
#include "pixel/PixelMapper.h"

#include <Arduino.h>

namespace {

config::ConfigManager configManager;
ModeController modeController;
Diagnostics diagnostics;
Watchdog watchdog;
Scheduler scheduler;
FrameStats* frameStats = nullptr;

led::VirtualPixelBuffer pixelBuffer;
led::LED_OutputEngine ledOutput;
net::EthernetManager ethernet;
net::SacnReceiver sacnReceiver;
net::UniverseManager universeManager;
pixel::PixelMapper pixelMapper;
effects::EffectEngine effectEngine;
dmx::DmxManager dmxManager;

NodeConfig nodeConfig;

void onSacnPacket(const net::SacnPacketInfo& info, void*) {
    if (!info.valid || !info.dmxData) {
        return;
    }
    universeManager.updateUniverse(info.universe, info.dmxData, info.dmxLength, millis());
    if (frameStats) {
        frameStats->sacnPacketsReceived++;
    }
}

void onLinkChange(bool up) {
    diagnostics.logLinkChange(up);
}

void renderFrame(void*) {
    const uint32_t now = millis();
    watchdog.feed();

    if (modeController.isPixelMode()) {
        pixelMapper.mapToBuffer(universeManager, pixelBuffer.data(), pixelBuffer.count());
    } else {
        effectEngine.tick(now, universeManager);
    }

    if (!ledOutput.isShowing()) {
        ledOutput.show();
    }

    dmxManager.poll(now);
    universeManager.tick(now);

    if (frameStats) {
        diagnostics.logFrame(*frameStats, modeController.mode());
    }
}

}  // namespace

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000) {
    }

    diagnostics.begin();
    configManager.begin();
    nodeConfig = configManager.get();

    modeController.begin(nodeConfig.mode);
    diagnostics.logBoot(nodeConfig);

    pixelBuffer.begin(nodeConfig.leds);
    pixelBuffer.clear(CRGB::Black);

    if (!ledOutput.begin(nodeConfig.leds, pixelBuffer.data())) {
        Serial.println("[ERROR] LED output init failed");
        diagnostics.blinkStatus(10);
    }

    if (!ethernet.begin(nodeConfig.sacn)) {
        Serial.println("[WARN] Ethernet init failed");
    }
    ethernet.onLinkChange(onLinkChange);

    universeManager.begin(nodeConfig.sacn, nodeConfig.mode);
    sacnReceiver.setCallback(onSacnPacket, nullptr);
    if (!sacnReceiver.begin(nodeConfig.sacn, nodeConfig.mode)) {
        Serial.println("[WARN] sACN receiver init failed");
    }

    pixelMapper.begin(nodeConfig.sacn, nodeConfig.leds);

    if (modeController.isEffectMode()) {
        effectEngine.begin(nodeConfig, pixelBuffer.data(), pixelBuffer.count());
    }

    if (!dmxManager.begin(nodeConfig.dmxPorts)) {
        Serial.println("[WARN] DMX manager init failed");
    }

    watchdog.begin();
    scheduler.begin(FRAME_INTERVAL_US);
    scheduler.setCallback(renderFrame, nullptr);
    frameStats = &scheduler.stats();

    Serial.println("[System] Ready");
}

void loop() {
    ethernet.poll();
    sacnReceiver.poll();
    scheduler.tick();
    watchdog.check();
}
