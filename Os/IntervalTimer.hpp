/**
 * IntervalTimer.hpp:
 *
 * Interval timer provides timing over a set interval to the caller. It is one of the core Os
 * package supplied items.
 */
#ifndef _IntervalTimer_hpp_
#define _IntervalTimer_hpp_

#include <FpConfig.hpp>
#include <Os/RawTime.hpp>

namespace Os {
    class IntervalTimer {
        public:

            IntervalTimer(); //!<  Constructor
            ~IntervalTimer() = default; //!<  Destructor

            //------------ Common Functions ------------
            // Common functions, typically do not need to be implemented by an OS support package.
            // Common implementations in IntervalTimerCommon.cpp.
            //------------------------------------------
            /**
             * Capture a start time of the interval timed by the interval timer. This fills the
             * start RawTime of the interval.
             */
            void start();
            /**
             * Capture a stop time of the interval timed by the interval timer. This fills the
             * stop RawTime of the interval.
             */
            void stop();
            /**
             * Returns the difference in usecond difference between start and stop times. The caller
             * must have called start and stop previously.
             * \return U32: microseconds difference in the interval
             */
            U32 getDiffUsec() const;

            //! \brief Get the time interval between the start and stop times
            Fw::TimeInterval getTimeInterval() const;

        PRIVATE:

            //------------ Internal Member Variables ------------
            RawTime m_startTime; //!<  Stored start time
            RawTime m_stopTime; //!<  Stored end time

            //------------ Disabled (private) Copy Constructor ------------
            IntervalTimer(IntervalTimer&); //!<  Disabled copy constructor
    };
}

#endif
