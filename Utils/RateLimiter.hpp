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

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Time/Time.hpp>

namespace Utils {

class RateLimiter {
  public:
    //! Constructor with specified rate limiting cycles
    //!
    //! \param counterCycle Number of calls before allowing action (0 to disable counter-based limiting)
    //! \param timeCycle Minimum seconds between actions (0 to disable time-based limiting)
    //!
    //! \note If both cycles are set, satisfying either condition will trigger (OR logic)
    RateLimiter(U32 counterCycle, U32 timeCycle);

    //! Default constructor with both cycles disabled (set to 0)
    //!
    //! \note Cycles must be configured via setCounterCycle() or setTimeCycle() before use
    RateLimiter();

  public:
    //! Update the counter cycle threshold at runtime
    //!
    //! \param counterCycle New threshold for counter-based limiting (0 to disable)
    void setCounterCycle(U32 counterCycle);

    //! Update the time cycle threshold at runtime
    //!
    //! \param timeCycle New minimum time between actions in seconds (0 to disable)
    void setTimeCycle(U32 timeCycle);

    //! Main entry point - evaluates whether an action should trigger
    //!
    //! Evaluates counter or time limiting based on configured cycles.
    //! If both cycles are defined, satisfying either condition triggers (OR logic).
    //! For example, trigger once every X calls OR once every Y seconds, whichever comes first.
    //!
    //! \param time Current time for time-based evaluation
    //! \return true if action should trigger, false otherwise
    bool trigger(Fw::Time time);

    //! Trigger evaluation for counter-only rate limiters (no time argument)
    //!
    //! \note This is a shorthand for counter-only RateLimiters
    //! \warning If a time cycle is defined but this version is called, will assert
    //!
    //! \return true if action should trigger, false otherwise
    bool trigger();

    //! Reset both counter and time state to initial values
    void reset();

    //! Reset only the counter state to 0
    void resetCounter();

    //! Reset only the time state to negative infinity
    void resetTime();

    //! Manually set the counter state
    //!
    //! \param counter New counter value
    void setCounter(U32 counter);

    //! Manually set the time state
    //!
    //! \param time New time value
    void setTime(Fw::Time time);

  private:
    //! Evaluate if counter threshold has been reached
    bool shouldCounterTrigger();

    //! Evaluate if time threshold has been reached
    //!
    //! \param time Current time to evaluate against
    bool shouldTimeTrigger(Fw::Time time);

    //! Update counter state after trigger evaluation
    //!
    //! \param triggered Whether the action was triggered
    void updateCounter(bool triggered);

    //! Update time state after trigger evaluation
    //!
    //! \param triggered Whether the action was triggered
    //! \param time Current time
    void updateTime(bool triggered, Fw::Time time);

  private:
    // Configuration parameters
    U32 m_counterCycle;           //!< Number of trigger calls before allowing an action (0 = disabled)
    U32 m_timeCycle;              //!< Minimum time in seconds between allowed actions (0 = disabled)

    // Internal state tracking
    U32 m_counter;                //!< Current counter value, resets to 0 after triggering
    Fw::Time m_time;              //!< Timestamp of the last trigger event
    bool m_timeAtNegativeInfinity; //!< Flag indicating if time has never been set (initial state)
};

}  // end namespace Utils

#endif
