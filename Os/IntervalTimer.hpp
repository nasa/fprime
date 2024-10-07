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
            U32 getDiffUsec();

            //------------ Platform Functions ------------
            // Platform functions, typically do need to be implemented by an OS support package, as
            // they are dependent on the platform definition of "RawTime".
            //------------------------------------------

            /**
             * Returns the difference in microseconds between the supplied times t1, and t2. This
             * calculation is done with respect to the semantic meaning of the times, and thus is
             * dependent on the platform's representation of the RawTime object.
             * \return U32 microsecond difference between two supplied values, t1-t2.
             */
            static U32 getDiffUsec(const RawTime& t1, const RawTime& t2);
            /**
             * Fills the RawTime object supplied with the current raw time in a platform dependent
             * way.
             */
            static void getRawTime(RawTime& time);
        PRIVATE:

            //------------ Internal Member Variables ------------
            RawTime m_startTime; //!<  Stored start time
            RawTime m_stopTime; //!<  Stored end time

            //------------ Disabled (private) Copy Constructor ------------
            IntervalTimer(IntervalTimer&); //!<  Disabled copy constructor
    };
}

#endif
