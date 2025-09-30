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

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/Time.hpp>

//! Maximum allowed tokens in a TokenBucket instance
//! This limit prevents excessive memory usage and ensures reasonable burst sizes
#define MAX_TOKEN_BUCKET_TOKENS 1000

namespace Utils {

class TokenBucket {
  public:
    //! Full constructor with all configuration parameters
    //!
    //! \param replenishInterval Time in microseconds between token replenishments
    //! \param maxTokens Maximum tokens that can be accumulated (capped at MAX_TOKEN_BUCKET_TOKENS)
    //! \param replenishRate Number of tokens added per replenishment interval
    //! \param startTokens Initial token count (typically maxTokens for full burst capacity)
    //! \param startTime Initial timestamp reference for replenishment calculation
    //!
    //! \note This allows fine-grained control over burst behavior and token accumulation rate
    TokenBucket(U32 replenishInterval, U32 maxTokens, U32 replenishRate, U32 startTokens, Fw::Time startTime);

    //! Simplified constructor with default parameters
    //!
    //! \param replenishInterval Time in microseconds between token replenishments
    //! \param maxTokens Maximum tokens that can be accumulated
    //!
    //! Equivalent to TokenBucket(replenishInterval, maxTokens, 1, maxTokens, Fw::Time(0,0))
    TokenBucket(U32 replenishInterval, U32 maxTokens);

  public:
    //! Update the maximum token capacity at runtime
    //!
    //! \param maxTokens New maximum token count (capped at MAX_TOKEN_BUCKET_TOKENS)
    void setMaxTokens(U32 maxTokens);

    //! Update the replenishment interval at runtime
    //!
    //! \param replenishInterval New interval in microseconds
    void setReplenishInterval(U32 replenishInterval);

    //! Update the replenishment rate at runtime
    //!
    //! \param replenishRate New number of tokens added per interval
    void setReplenishRate(U32 replenishRate);

    //! Get the current maximum token capacity
    //!
    //! \return Maximum tokens
    U32 getMaxTokens() const;

    //! Get the current replenishment interval
    //!
    //! \return Interval in microseconds
    U32 getReplenishInterval() const;

    //! Get the current replenishment rate
    //!
    //! \return Tokens added per interval
    U32 getReplenishRate() const;

    //! Get the current number of available tokens
    //!
    //! \return Current token count
    U32 getTokens() const;

    //! Manually add tokens according to the replenishment rate
    //!
    //! \note Tokens will not exceed maxTokens
    void replenish();

    //! Main entry point - evaluates and consumes a token if available
    //!
    //! Evaluates time since last trigger to determine number of tokens to replenish.
    //! If time moved backwards, always returns false.
    //! If tokens are available, consumes one and returns true.
    //!
    //! \param time Current time for replenishment calculation
    //! \return true if token was consumed (action allowed), false otherwise
    bool trigger(const Fw::Time time);

  private:
    // Configuration parameters (set at construction or via setters)
    U32 m_replenishInterval;  //!< Microseconds between token replenishments
    U32 m_maxTokens;          //!< Maximum token capacity (upper bound for accumulation)
    U32 m_replenishRate;      //!< Number of tokens added per replenishment interval

    // Runtime state
    U32 m_tokens;             //!< Current number of available tokens
    Fw::Time m_time;          //!< Timestamp of last trigger, used to calculate token replenishment
};

}  // end namespace Utils

#endif
