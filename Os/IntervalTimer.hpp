#ifndef _IntervalTimer_hpp_
#define _IntervalTimer_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Os {

    class IntervalTimer {
        public:
            
            // represents raw hardware time
            
            typedef struct {
                U32 upper;  //!<  Upper part of time value. Platform dependent.
                U32 lower; //!<  Lower part of time value. Platform dependent.
            } RawTime;
                        
            IntervalTimer(); //!<  Constructor
            virtual ~IntervalTimer(); //!<  Desctructor
            
            void start(void); //!<  store start time
            void stop(void); //!<  store end time
            
            U32 getDiffUsec(void); //!<  return usec difference between start and stop. Must have called start and stop previously.
            static U32 getDiffUsec(const RawTime& t1, const RawTime& t2); //!<  return microsec difference between two supplied values, t1-t2
            static U32 getDiffNsec(const RawTime& t1, const RawTime& t2); //!<  return nanosec difference between two supplied values, t1-t2

            RawTime getDiffRaw(void);  //!<  return raw difference between start and stop values
            static RawTime getDiffRaw(const RawTime& t1, const RawTime& t2); //!<  return raw difference between supplied values, t1-t2
            
            static RawTime getSumRaw(const RawTime& t1, const RawTime& t2); //!<  get the sum of the times in the raw representation
            
            static U32 nanoSecPerClockTick(void); //!<  Get the number of nanoseconds per clock tick. Platform dependent.
            static I32 toNanoSec(const RawTime& time); //!<  Convert raw time to nanoseconds. Platform dependent. Time since time source value was zero.
            
            static void getRawTime(RawTime& time);  //!<  Get the current raw time
            
        PRIVATE:

            RawTime m_startTime; //!<  Stored start time
            RawTime m_stopTime; //!<  Stored end time
            IntervalTimer(IntervalTimer&); //!<  disabled copy constructor

            static U32 getTimerFrequency(void); //!<  target specific calls to get the frequency of the timer for computing durations
            static U32 timerToUsec(const RawTime& val); //!<  convert the raw time to microseconds

    };
}

#endif
