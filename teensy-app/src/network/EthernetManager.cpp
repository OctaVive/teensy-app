#include "network/EthernetManager.h"

#include <QNEthernet.h>
#include <stdio.h>

namespace net {

namespace {
using namespace qindesign::network;

IPAddress ipFromString(const char* value, IPAddress fallback) {
    if (!value) {
        return fallback;
    }
    uint8_t a = 0;
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t d = 0;
    if (sscanf(value, "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d) == 4) {
        return IPAddress(a, b, c, d);
    }
    return fallback;
}

}  // namespace

bool EthernetManager::begin(const SacnConfig& sacn) {
    const IPAddress ip = ipFromString(sacn.ip, IPAddress(192, 168, 1, 177));
    const IPAddress subnet = ipFromString(sacn.subnet, IPAddress(255, 255, 255, 0));
    const IPAddress gateway = ipFromString(sacn.gateway, IPAddress(192, 168, 1, 1));

    if (!Ethernet.begin(ip, subnet, gateway)) {
        Serial.println("[Ethernet] begin failed");
        return false;
    }

    linkUp_ = Ethernet.linkState();
    Serial.print("[Ethernet] IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("[Ethernet] Link: ");
    Serial.println(linkUp_ ? "UP" : "DOWN");
    return true;
}

void EthernetManager::poll() {
    const bool up = Ethernet.linkState();
    if (up != linkUp_) {
        linkUp_ = up;
        if (linkCallback_) {
            linkCallback_(up);
        }
    }
}

void EthernetManager::onLinkChange(void (*callback)(bool up)) {
    linkCallback_ = callback;
}

}  // namespace net
