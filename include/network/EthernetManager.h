#pragma once

#include "types.h"

namespace net {

class EthernetManager {
public:
    bool begin(const SacnConfig& sacn);
    void poll();
    bool isLinkUp() const { return linkUp_; }
    void onLinkChange(void (*callback)(bool up));

private:
    bool linkUp_ = false;
    void (*linkCallback_)(bool) = nullptr;
};

}  // namespace net
