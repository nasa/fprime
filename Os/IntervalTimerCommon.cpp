#include <Os/IntervalTimer.hpp>

#include <string.h>

namespace Os {
    
    IntervalTimer::IntervalTimer() {
        memset(&this->m_startTime,0,sizeof(this->m_startTime));
        memset(&this->m_stopTime,0,sizeof(this->m_stopTime));
    }
    
    IntervalTimer::~IntervalTimer() {
    }
    
    void IntervalTimer::start() {
        getRawTime(this->m_startTime);
    }
    
    void IntervalTimer::stop() {
        getRawTime(this->m_stopTime);
    }
    
    U32 IntervalTimer::getDiffUsec(void) {
        return getDiffUsec(this->m_stopTime,this->m_startTime);
    }
    
    U32 IntervalTimer::getDiffUsec(const RawTime& t1, const RawTime& t2) {
        return timerToUsec(getDiffRaw(t1, t2));
    }
    
    U32 IntervalTimer::getDiffNsec(const RawTime& t1, const RawTime& t2) {
//        const U64 al = (((U64) p_subtrahend->upper << 32) + (U64) p_subtrahend->lower);
//        const U64 bl = (((U64) p_minuend->upper << 32) + (U64) p_minuend->lower);
//        return (U32)((bl - al) * osal_nsec_per_clock_tick());
        return 0;
    }

    IntervalTimer::RawTime IntervalTimer::getDiffRaw(void) {
        return getDiffRaw(this->m_stopTime,this->m_stopTime);
    }
    
    IntervalTimer::RawTime IntervalTimer::getDiffRaw(const RawTime& t1, const RawTime& t2) {
        RawTime diff;
        const U64 al = (((U64) t1.upper << 32) + (U64) t1.lower);
        const U64 bl = (((U64) t2.upper << 32) + (U64) t2.lower);
        if (t2.lower > t1.lower) {
            diff.lower = 0xFFFFFFFF - (t2.lower - t1.lower - 1);
            diff.upper = t1.upper - t2.upper - 1;
        } else {
            diff.lower = (U32) (al - bl);
            diff.upper = (U32) ((U64)(al - bl) >> 32);
        }
        return diff;
    }
    
    IntervalTimer::RawTime IntervalTimer::getSumRaw(const RawTime& t1, const RawTime& t2) {
        RawTime sum;
        const U64 al = (((U64) t1.upper << 32) + (U64) t1.lower);
        const U64 bl = (((U64) t2.upper << 32) + (U64) t2.lower);
        sum.lower = (U32) (al + bl);
        sum.upper = (U32) ((U64)(al + bl) >> 32);
        return sum;
    }
    
    U32 IntervalTimer::timerToUsec(const RawTime& val) {
        const U64 n = (((U64) val.upper << 32) + (U64) val.lower);
       
        // Conversion explained.
        //
        // n                      = number of ticks
        // 1,000,000              = microseconds per second.
        // osal_timer_frequency() = system ticks per second.
        //
        // microseconds = (n ticks) * (microseconds/tick)
       
        return (U32)((n * 1000000) / getTimerFrequency());
        
    }
    
    U32 IntervalTimer::nanoSecPerClockTick(void) {
        return 1000000000/IntervalTimer::getTimerFrequency();
    }
    
    I32 IntervalTimer::toNanoSec(const RawTime& time) {
        return (U32)((((U64) time.upper << 32) + (U64) time.lower) * (U32)IntervalTimer::nanoSecPerClockTick());
    }
                
    
}

extern "C" {
    void testTimer(U32 sec);
}
