// ======================================================================
// \title Os/IntervalTimer.hpp
// \brief Definition for Os::IntervalTimer
// ======================================================================
#ifndef _IntervalTimer_hpp_
#define _IntervalTimer_hpp_

#include <FpConfig.hpp>
#include <Os/RawTime.hpp>

namespace Os {
//! \brief Os::IntervalTimer measures time intervals using start/stop functionality.
//!
//! The IntervalTimer class provides methods to capture the start and stop times of an interval
//! and calculate the difference between these times. It is useful for measuring the duration
//! of operations or events. Intervals can be returned in Fw::TimeInterval or as a microsecond U32.
//!
//! \note The caller must ensure that the start() method is called before the stop() method to get
//! a relevant time interval.
//!
//! \example
//! IntervalTimer timer;
//! timer.start();
//! // Perform some operations
//! timer.stop();
//! Fw::TimeInterval interval = timer.getTimeInterval();
class IntervalTimer {
  public:
    //! \brief Constructor
    IntervalTimer();

    //! \brief Destructor
    ~IntervalTimer() = default;

    //! \brief Capture the start time of the interval.
    //!
    //! This method records the current time as the start time of the interval for this timer instance.
    void start();

    //! \brief Capture the stop time of the interval.
    //!
    //! This method records the current time as the stop time of the interval for this timer instance.
    void stop();

    //! \brief Get the difference between start and stop times in microseconds.
    //!
    //! This method calculates and returns the time difference between the start and stop times
    //! in microseconds. The start() and stop() methods must be called before calling this method.
    //!
    //! \warning Users should prefer the getTimeInterval() method for better error handling.
    //! \warning This function will return the maximum U32 value if the time difference is too large to fit in a U32.
    //! \warning This means the largest time difference that can be measured is 2^32 microseconds (about 71 minutes).
    //!
    //! \return U32: The time difference in microseconds.
    U32 getDiffUsec() const;

    //! \brief Get the time interval between the start and stop times.
    //!
    //! This method calculates and returns the time interval between the recorded start and stop times
    //! as a Fw::TimeInterval object.
    //!
    //! \param interval [out] A reference to a Fw::TimeInterval object where the calculated interval will be stored.
    //! \return bool: True if the interval was successfully calculated, false otherwise.
    Os::RawTime::Status getTimeInterval(Fw::TimeInterval& interval) const;

  PRIVATE:
    RawTime m_startTime;  //!< Stored start time
    RawTime m_stopTime;   //!< Stored end time

    //! Disabled (private) Copy Constructor
    IntervalTimer(IntervalTimer&);

};  // class IntervalTimer

}  // namespace Os

#endif
