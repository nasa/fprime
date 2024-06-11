// ====================================================================== 
// \title  RateLimiter.hpp
// \author vwong
// \brief  hpp file for a rate limiter utility class
//
// \copyright
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef RateLimiter_HPP
#define RateLimiter_HPP

#include <FpConfig.hpp>
#include <Fw/Time/Time.hpp>

namespace Utils {

  class RateLimiter
  {

    public:

      // Construct with defined cycles
      RateLimiter(U32 counterCycle, U32 timeCycle);

      // Construct with cycles set to 0
      RateLimiter();

    public:

      // Adjust cycles at run-time
      void setCounterCycle(U32 counterCycle);
      void setTimeCycle(U32 timeCycle);

      // Main point of entry
      //
      // It will only factor in counter or time, whichever one has a cycle defined
      //
      // If both are defined, then satisfying _either_ one will work
      // e.g. I want to trigger only once every X times or once every Y
      // seconds, whichever comes first
      //
      // The argument-less version is a shorthand for counter-only RateLimiters
      // If a time cycle is defined but the argument-less version is called,
      // RateLimiter assumes the client forgot to supply a time, and asserts
      //
      bool trigger(Fw::Time time);
      bool trigger();

      // Manual state adjustments, if necessary
      void reset();
      void resetCounter();
      void resetTime();
      void setCounter(U32);
      void setTime(Fw::Time time);

    private:

      // Helper functions to update each independently
      bool shouldCounterTrigger();
      bool shouldTimeTrigger(Fw::Time time);
      void updateCounter(bool triggered);
      void updateTime(bool triggered, Fw::Time time);

    private:

      // parameters
      U32 m_counterCycle;
      U32 m_timeCycle;

      // state
      U32 m_counter;
      Fw::Time m_time;
      bool m_timeAtNegativeInfinity;
  };

} // end namespace Utils

#endif
