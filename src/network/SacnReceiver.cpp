#include "network/SacnReceiver.h"

#include <QNEthernet.h>
#include <string.h>

namespace net {

namespace {
using namespace qindesign::network;

constexpr uint16_t SACN_PORT = 5568;
constexpr size_t E131_MIN_PACKET = 126;

EthernetUDP udp;
bool receiverReady = false;
}

bool SacnReceiver::begin(const SacnConfig& sacn, GlobalMode mode) {
    config_ = sacn;
    mode_ = mode;

    if (!udp.begin(SACN_PORT)) {
        Serial.println("[sACN] UDP bind failed");
        return false;
    }

    if (config_.multicast) {
        if (mode_ == GlobalMode::Pixel) {
            for (uint16_t i = 0; i < config_.universeCount; ++i) {
                subscribeUniverse(static_cast<uint16_t>(config_.startUniverse + i));
            }
        } else {
            subscribeUniverse(config_.effectUniverse);
            subscribeUniverse(config_.virtualAllUniverse);
        }
    }

    receiverReady = true;
    Serial.println("[sACN] Receiver ready on port 5568");
    return true;
}

void SacnReceiver::setCallback(SacnPacketCallback cb, void* context) {
    callback_ = cb;
    callbackContext_ = context;
}

void SacnReceiver::subscribeUniverse(uint16_t universe) {
    if (!config_.multicast) {
        return;
    }
    (void)universe;
    // QNEthernet multicast receive setup differs by API version; keep receiver
    // active for unicast/multicast network forwarding in MVP.
}

bool SacnReceiver::parsePacket(const uint8_t* data, size_t len, SacnPacketInfo& out) {
    out = SacnPacketInfo{};
    if (len < E131_MIN_PACKET) {
        return false;
    }

    if (memcmp(&data[4], "ASC-E1.17", 9) != 0) {
        return false;
    }

    const uint8_t priority = data[108];
    const uint8_t sequence = data[111];
    const uint16_t universe = static_cast<uint16_t>(data[113] << 8) | data[114];

    if (priority < config_.priority) {
        return false;
    }

    const uint8_t slot = lastSequence_[universe % 256];
    if (sequence == slot) {
        return false;
    }
    lastSequence_[universe % 256] = sequence;

    const uint16_t propertyCount = static_cast<uint16_t>(data[123] << 8) | data[124];
    if (propertyCount < 1) {
        return false;
    }

    const uint16_t dmxLen = static_cast<uint16_t>(propertyCount - 1);
    if ((126u + static_cast<size_t>(dmxLen)) > len) {
        return false;
    }

    out.universe = universe;
    out.priority = priority;
    out.sequence = sequence;
    out.dmxData = &data[126];
    out.dmxLength = dmxLen > DMX_SLOTS ? DMX_SLOTS : dmxLen;
    out.valid = true;
    return true;
}

void SacnReceiver::poll() {
    if (!receiverReady || !callback_) {
        return;
    }

    int packetSize = 0;
    while ((packetSize = udp.parsePacket()) > 0) {
        uint8_t buffer[638];
        const int read = udp.read(buffer, sizeof(buffer));
        if (read <= 0) {
            continue;
        }

        SacnPacketInfo info;
        if (parsePacket(buffer, static_cast<size_t>(read), info)) {
            callback_(info, callbackContext_);
        }
    }
}

}  // namespace net
