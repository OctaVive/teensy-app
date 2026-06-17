#include "dmx/DmxManager.h"

#include <Arduino.h>
#include <TeensyDMX.h>

namespace dmx {

namespace {
namespace teensydmx = ::qindesign::teensydmx;

teensydmx::Sender sender0(Serial1);
teensydmx::Receiver receiver0(Serial1);
teensydmx::Sender sender1(Serial2);
teensydmx::Receiver receiver1(Serial2);
teensydmx::Sender sender2(Serial3);
teensydmx::Receiver receiver2(Serial3);
teensydmx::Sender sender3(Serial4);
teensydmx::Receiver receiver3(Serial4);
teensydmx::Sender sender4(Serial5);
teensydmx::Receiver receiver4(Serial5);
teensydmx::Sender sender5(Serial6);
teensydmx::Receiver receiver5(Serial6);
teensydmx::Sender sender6(Serial7);
teensydmx::Receiver receiver6(Serial7);
teensydmx::Sender sender7(Serial8);
teensydmx::Receiver receiver7(Serial8);

teensydmx::Sender* senders[] = {&sender0, &sender1, &sender2, &sender3,
                                &sender4, &sender5, &sender6, &sender7};
teensydmx::Receiver* receivers[] = {&receiver0, &receiver1, &receiver2, &receiver3,
                                    &receiver4, &receiver5, &receiver6, &receiver7};

}  // namespace

bool DmxManager::begin(const DmxPortConfig ports[DMX_PORT_COUNT]) {
    bool ok = true;
    for (uint8_t i = 0; i < DMX_PORT_COUNT; ++i) {
        ports_[i].config = ports[i];
        ports_[i].lastActivityMs = millis();
        ports_[i].active = initPort(i, ports[i]);
        ok = ok && ports_[i].active;
    }
    Serial.println("[DMX] Manager initialized");
    return ok;
}

bool DmxManager::initPort(uint8_t index, const DmxPortConfig& cfg) {
    if (index >= DMX_PORT_COUNT) {
        return false;
    }

    if (cfg.direction == DmxDirection::Output) {
        senders[index]->begin();
        senders[index]->set(1, 0);
    } else {
        receivers[index]->begin();
    }
    return true;
}

void DmxManager::poll(uint32_t nowMs) {
    for (uint8_t i = 0; i < DMX_PORT_COUNT; ++i) {
        DmxPortState& port = ports_[i];
        if (!port.active) {
            continue;
        }

        if (port.config.direction == DmxDirection::Input) {
            const int size = receivers[i]->readPacket(port.buffer, 1, DMX_SLOTS, nullptr);
            if (size > 0) {
                port.lastActivityMs = nowMs;
            } else if ((nowMs - port.lastActivityMs) > port.config.inputTimeoutMs) {
                // Hold last frame on timeout (plan default)
            }
        } else {
            senders[i]->set(1, port.buffer[0]);
            for (uint16_t ch = 1; ch < DMX_SLOTS; ++ch) {
                senders[i]->set(static_cast<int16_t>(ch + 1), port.buffer[ch]);
            }
            port.lastActivityMs = nowMs;
        }
    }
}

DmxPortState* DmxManager::port(uint8_t index) {
    return index < DMX_PORT_COUNT ? &ports_[index] : nullptr;
}

const DmxPortState* DmxManager::port(uint8_t index) const {
    return index < DMX_PORT_COUNT ? &ports_[index] : nullptr;
}

}  // namespace dmx
