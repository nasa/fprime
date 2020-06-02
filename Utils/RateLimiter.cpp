// ====================================================================== 
// \title  RateLimiter.cpp
// \author vwong
// \brief  cpp file for a rate limiter utility class
//
// \copyright
// Copyright (C) 2009-2020 California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#include <Utils/RateLimiter.hpp>

namespace Utils {

  RateLimiter ::
    RateLimiter (
        U32 counterCycle,
        U32 timeCycle
    ) :
      m_counterCycle(counterCycle),
      m_timeCycle(timeCycle)
  {
    this->reset();
  }

  RateLimiter ::
    RateLimiter () :
      m_counterCycle(0),
      m_timeCycle(0)
  {
    this->reset();
  }

  void RateLimiter ::
    setCounterCycle(
        U32 counterCycle
    )
  {
    this->m_counterCycle = counterCycle;
  }

  void RateLimiter ::
    setTimeCycle(
        U32 timeCycle
    )
  {
    this->m_timeCycle = timeCycle;
  }

  void RateLimiter ::
    reset()
  {
    this->resetCounter();
    this->resetTime();
  }

  void RateLimiter ::
    resetCounter()
  {
    this->m_counter = 0;
  }

  void RateLimiter ::
    resetTime()
  {
    this->m_time = Fw::Time();
    this->m_timeAtNegativeInfinity = true;
  }

  void RateLimiter ::
    setCounter(
        U32 counter
    )
  {
    this->m_counter = counter;
  }

  void RateLimiter ::
    setTime(
        Fw::Time time
    )
  {
    this->m_time = time;
    this->m_timeAtNegativeInfinity = false;
  }

  bool RateLimiter ::
    trigger(
        Fw::Time time
    )
  {
    // NB: this implements a 4-bit decision, logically equivalent to this pseudo-code
    //
    // A = HAS_COUNTER, B = HAS_TIME, C = COUNTER_TRIGGER, D = TIME_TRIGGER
    //
    // if (!A && !B) => true
    // if (A && B) => C || D
    // if (A) => C
    // if (B) => D
    // false
    //
    if (this->m_counterCycle == 0 && this->m_timeCycle == 0) {
      return true;
    }

    // evaluate trigger criteria
    bool shouldTrigger = false;
    if (this->m_counterCycle > 0) {
      shouldTrigger = shouldTrigger || this->shouldCounterTrigger();
    }
    if (this->m_timeCycle > 0) {
      shouldTrigger = shouldTrigger || this->shouldTimeTrigger(time);
    }

    // update states
    if (this->m_counterCycle > 0) {
      this->updateCounter(shouldTrigger);
    }
    if (this->m_timeCycle > 0) {
      this->updateTime(shouldTrigger, time);
    }

    return shouldTrigger;
  }

  bool RateLimiter ::
    trigger()
  {
    FW_ASSERT(this->m_timeCycle == 0);
    return trigger(Fw::Time::zero());
  }

  bool RateLimiter ::
    shouldCounterTrigger()
  {
    FW_ASSERT(this->m_counterCycle > 0);

    // trigger at 0
    bool shouldTrigger = (this->m_counter == 0);

    return shouldTrigger;
  }

  bool RateLimiter ::
    shouldTimeTrigger(Fw::Time time)
  {
    FW_ASSERT(this->m_timeCycle > 0);

    // trigger at prev trigger time + time cycle seconds OR when time is at negative infinity
    Fw::Time timeCycle = Fw::Time(this->m_timeCycle, 0);
    Fw::Time nextTrigger = Fw::Time::add(this->m_time, timeCycle);
    bool shouldTrigger = (time >= nextTrigger) || this->m_timeAtNegativeInfinity;

    return shouldTrigger;
  }

  void RateLimiter ::
    updateCounter(bool triggered)
  {
    FW_ASSERT(this->m_counterCycle > 0);

    if (triggered) {
      // triggered, set to next state
      this->m_counter = 1;

    } else {
      // otherwise, just increment and maybe wrap
      if (++this->m_counter >= this->m_counterCycle) {
        this->m_counter = 0;
      }
    }
  }

  void RateLimiter ::
    updateTime(bool triggered, Fw::Time time)
  {
    FW_ASSERT(this->m_timeCycle > 0);

    if (triggered) {
      // mark time of trigger
      this->m_time = time;
    }
    this->m_timeAtNegativeInfinity = false;
  }

} // end namespace Utils
