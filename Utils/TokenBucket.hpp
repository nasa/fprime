// ====================================================================== 
// \title  TokenBucket.hpp
// \author vwong
// \brief  hpp file for a rate limiter utility class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef TokenBucket_HPP
#define TokenBucket_HPP

#include <FpConfig.hpp>
#include <Fw/Time/Time.hpp>

#define MAX_TOKEN_BUCKET_TOKENS 1000

namespace Utils {

  class TokenBucket
  {

    public:

      // Full constructor
      //
      // replenishInterval is in microseconds
      //
      TokenBucket(U32 replenishInterval, U32 maxTokens, U32 replenishRate, U32 startTokens, Fw::Time startTime);

      // replenishRate=1, startTokens=maxTokens, startTime=0
      TokenBucket(U32 replenishInterval, U32 maxTokens);

    public:

      // Adjust settings at runtime
      void setMaxTokens(U32 maxTokens);
      void setReplenishInterval(U32 replenishInterval);
      void setReplenishRate(U32 replenishRate);

      U32 getMaxTokens() const;
      U32 getReplenishInterval() const;
      U32 getReplenishRate() const;
      U32 getTokens() const;

      // Manual replenish
      void replenish();

      // Main point of entry
      //
      // Evaluates time since last trigger to determine number of tokens to
      // replenish. If time moved backwards, always returns false.
      //
      // If number of tokens is not zero, consumes one and returns true.
      // Otherwise, returns false.
      //
      bool trigger(const Fw::Time time);

    private:

      // parameters
      U32 m_replenishInterval;
      U32 m_maxTokens;
      U32 m_replenishRate;

      // state
      U32 m_tokens;
      Fw::Time m_time;
  };

} // end namespace Utils

#endif
