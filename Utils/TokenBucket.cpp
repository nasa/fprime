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

namespace Utils {

  TokenBucket ::
    TokenBucket (
        U32 replenishInterval,
        U32 maxTokens,
        U32 replenishRate,
        U32 startTokens,
        Fw::Time startTime
    ) :
      m_replenishInterval(replenishInterval),
      m_maxTokens(maxTokens),
      m_replenishRate(replenishRate),
      m_tokens(startTokens),
      m_time(startTime)
  {
  }

  TokenBucket ::
    TokenBucket (
        U32 replenishInterval,
        U32 maxTokens
    ) :
      m_replenishInterval(replenishInterval),
      m_maxTokens(maxTokens),
      m_replenishRate(1),
      m_tokens(maxTokens),
      m_time(0, 0)
  {
    FW_ASSERT(this->m_maxTokens <= MAX_TOKEN_BUCKET_TOKENS, static_cast<FwAssertArgType>(this->m_maxTokens));
  }

  void TokenBucket ::
    setReplenishInterval(
        U32 replenishInterval
    )
  {
    this->m_replenishInterval = replenishInterval;
  }

  void TokenBucket ::
    setMaxTokens(
        U32 maxTokens
    )
  {
    this->m_maxTokens = maxTokens;
  }

  void TokenBucket ::
    setReplenishRate(
        U32 replenishRate
    )
  {
    this->m_replenishRate = replenishRate;
  }

  void TokenBucket ::
    replenish()
  {
    if (this->m_tokens < this->m_maxTokens) {
      this->m_tokens = this->m_maxTokens;
    }
  }

  U32 TokenBucket ::
    getReplenishInterval() const
  {
    return this->m_replenishInterval;
  }

  U32 TokenBucket ::
    getMaxTokens() const
  {
    return this->m_maxTokens;
  }

  U32 TokenBucket ::
    getReplenishRate() const
  {
    return this->m_replenishRate;
  }

  U32 TokenBucket ::
    getTokens() const
  {
    return this->m_tokens;
  }

  bool TokenBucket ::
    trigger(
        const Fw::Time time
    )
  {
    // attempt replenishing
    if (this->m_replenishRate > 0) {
      Fw::Time replenishInterval = Fw::Time(this->m_replenishInterval / 1000000, this->m_replenishInterval % 1000000);
      Fw::Time nextTime = Fw::Time::add(this->m_time, replenishInterval);
      while (this->m_tokens < this->m_maxTokens && nextTime <= time) {
        // replenish by replenish rate, or up to maxTokens
        this->m_tokens += FW_MIN(this->m_replenishRate, this->m_maxTokens - this->m_tokens);
        this->m_time = nextTime;
        nextTime = Fw::Time::add(this->m_time, replenishInterval);
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

} // end namespace Utils
