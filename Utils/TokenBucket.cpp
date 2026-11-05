// ======================================================================
// \title  TokenBucket.cpp
// \author vwong
// \brief  cpp file for a rate limiter utility class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <Utils/TokenBucket.hpp>
#include <algorithm>

namespace Utils {

TokenBucket ::TokenBucket(U32 replenishInterval, U32 maxTokens, U32 replenishRate, U32 startTokens, Fw::Time startTime)
    : m_replenishInterval(replenishInterval),
      m_maxTokens(maxTokens),
      m_replenishRate(replenishRate),
      m_tokens(startTokens),
      m_time(startTime) {}

TokenBucket ::TokenBucket(U32 replenishInterval, U32 maxTokens)
    : m_replenishInterval(replenishInterval),
      m_maxTokens(maxTokens),
      m_replenishRate(1),
      m_tokens(maxTokens),
      m_time(0, 0) {
    FW_ASSERT(this->m_maxTokens <= MAX_TOKEN_BUCKET_TOKENS, static_cast<FwAssertArgType>(this->m_maxTokens));
}

void TokenBucket ::setReplenishInterval(U32 replenishInterval) {
    this->m_replenishInterval = replenishInterval;
}

void TokenBucket ::setMaxTokens(U32 maxTokens) {
    this->m_maxTokens = maxTokens;
}

void TokenBucket ::setReplenishRate(U32 replenishRate) {
    this->m_replenishRate = replenishRate;
}

void TokenBucket ::replenish() {
    if (this->m_tokens < this->m_maxTokens) {
        this->m_tokens = this->m_maxTokens;
    }
}

U32 TokenBucket ::getReplenishInterval() const {
    return this->m_replenishInterval;
}

U32 TokenBucket ::getMaxTokens() const {
    return this->m_maxTokens;
}

U32 TokenBucket ::getReplenishRate() const {
    return this->m_replenishRate;
}

U32 TokenBucket ::getTokens() const {
    return this->m_tokens;
}

bool TokenBucket ::trigger(const Fw::Time time) {
    // attempt replenishing
    if (this->m_replenishRate > 0) {
        // A stored time in another base (e.g. TB_NONE from the short constructor) is incomparable
        // with the caller's time, so elapsed time is unknown: restart replenishment from this time
        if (this->m_time.getTimeBase() != time.getTimeBase()) {
            this->m_time = time;
        }
        const U32 intervalSeconds = this->m_replenishInterval / 1000000;
        const U32 intervalUSeconds = this->m_replenishInterval % 1000000;
        // Member add keeps the time base/context of m_time so nextTime stays comparable with time
        Fw::Time nextTime = this->m_time;
        nextTime.add(intervalSeconds, intervalUSeconds);
        while (this->m_tokens < this->m_maxTokens && nextTime <= time) {
            // replenish by replenish rate, or up to maxTokens
            this->m_tokens += std::min(this->m_replenishRate, this->m_maxTokens - this->m_tokens);
            this->m_time = nextTime;
            nextTime.add(intervalSeconds, intervalUSeconds);
        }
        if (this->m_tokens >= this->m_maxTokens && this->m_time < time) {
            this->m_time = time;
        }
    }

    // attempt consuming token
    if (this->m_tokens > 0) {
        this->m_tokens--;
        return true;

    } else {
        return false;
    }
}

}  // end namespace Utils
