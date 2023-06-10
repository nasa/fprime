#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
    void IntervalTimer::getRawTime(RawTime& time) {
        time.lower = 0;
        time.upper = 0;
    }

    U32 IntervalTimer::getDiffUsec(const RawTime& t1In, const RawTime& t2In) {
        return 0;
    }
}
