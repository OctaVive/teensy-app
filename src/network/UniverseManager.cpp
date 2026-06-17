#include "network/UniverseManager.h"

#include <string.h>

namespace net {

void UniverseManager::begin(const SacnConfig& sacn, GlobalMode mode) {
    config_ = sacn;
    mode_ = mode;
    allocateBuffers(mode);
}

void UniverseManager::allocateBuffers(GlobalMode mode) {
    bufferCount_ = 0;
    if (mode == GlobalMode::Pixel) {
        for (uint16_t i = 0; i < config_.universeCount && bufferCount_ < 32; ++i) {
            buffers_[bufferCount_].universe = static_cast<uint16_t>(config_.startUniverse + i);
            buffers_[bufferCount_].valid = false;
            buffers_[bufferCount_].lastSeenMs = 0;
            ++bufferCount_;
        }
    } else {
        buffers_[0].universe = config_.effectUniverse;
        buffers_[0].valid = false;
        buffers_[1].universe = config_.virtualAllUniverse;
        buffers_[1].valid = false;
        bufferCount_ = 2;
    }
}

UniverseBuffer* UniverseManager::findBuffer(uint16_t universe) {
    for (uint8_t i = 0; i < bufferCount_; ++i) {
        if (buffers_[i].universe == universe) {
            return &buffers_[i];
        }
    }
    return nullptr;
}

const UniverseBuffer* UniverseManager::getBuffer(uint16_t universe) const {
    for (uint8_t i = 0; i < bufferCount_; ++i) {
        if (buffers_[i].universe == universe) {
            return &buffers_[i];
        }
    }
    return nullptr;
}

void UniverseManager::updateUniverse(uint16_t universe, const uint8_t* data, uint16_t len,
                                     uint32_t nowMs) {
    UniverseBuffer* buffer = findBuffer(universe);
    if (!buffer) {
        return;
    }

    const uint16_t copyLen = len > DMX_SLOTS ? DMX_SLOTS : len;
    memcpy(buffer->data, data, copyLen);
    if (copyLen < DMX_SLOTS) {
        memset(buffer->data + copyLen, 0, DMX_SLOTS - copyLen);
    }
    buffer->lastSeenMs = nowMs;
    buffer->valid = true;
}

void UniverseManager::tick(uint32_t nowMs) {
    for (uint8_t i = 0; i < bufferCount_; ++i) {
        if (buffers_[i].valid && (nowMs - buffers_[i].lastSeenMs) > SACN_TIMEOUT_MS) {
            buffers_[i].valid = false;
        }
    }
}

bool UniverseManager::hasValidPixelData() const {
    if (mode_ != GlobalMode::Pixel) {
        return false;
    }
    for (uint8_t i = 0; i < bufferCount_; ++i) {
        if (buffers_[i].valid) {
            return true;
        }
    }
    return false;
}

bool UniverseManager::hasValidEffectData() const {
    if (mode_ != GlobalMode::Effect) {
        return false;
    }
    return buffers_[0].valid;
}

const uint8_t* UniverseManager::effectUniverseData() const {
    if (bufferCount_ == 0) {
        return nullptr;
    }
    return buffers_[0].data;
}

}  // namespace net
