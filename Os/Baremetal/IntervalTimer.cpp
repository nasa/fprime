#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
    IntervalTimer::IntervalTimer() {}

    IntervalTimer::~IntervalTimer() {}

    U32 IntervalTimer::getTimerFrequency(void) {
        return 1;
    }

    void IntervalTimer::getRawTime(RawTime& time) {
    	time.lower = 0;
    	time.upper = 0;
    }

    U32 IntervalTimer::getDiffUsec(void) {
        return 0;
    }
    U32 IntervalTimer::getDiffUsec(const RawTime& t1In, const RawTime& t2In) {
    	return 0;
    }

    void IntervalTimer::start() {}

    void IntervalTimer::stop() {}

    IntervalTimer::RawTime IntervalTimer::getDiffRaw(const RawTime& t1, const RawTime& t2) {
        RawTime diff;
        diff.lower = 0;
        diff.upper = 0;
        return diff;
    }


        
}


