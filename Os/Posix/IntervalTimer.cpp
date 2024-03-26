/**
 * Posix/IntervalTimer.cpp:
 *
 * The Posix implementation of the interval timer shares the same raw setup as other X86
 * implementations. That is: the lower U32 of the RawTime is nano-seconds, and the upper U32 of
 * RawTime object is seconds. Thus only the "getRawTime" function differs from the base X86
 * version of this file.
 */
#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>
#include <ctime>
#include <cerrno>

namespace Os {
    void IntervalTimer::getRawTime(RawTime& time) {
        timespec t;
        PlatformIntType status = clock_gettime(CLOCK_REALTIME,&t);
        FW_ASSERT(status == 0,errno);
        time.upper = static_cast<U32>(t.tv_sec);
        time.lower = static_cast<U32>(t.tv_nsec);
    }

    // Adapted from: http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
    // should be t1In - t2In
    U32 IntervalTimer::getDiffUsec(const RawTime& t1In, const RawTime& t2In) {
        RawTime result = {t1In.upper - t2In.upper, 0};
        if (t1In.lower < t2In.lower) {
            result.upper -= 1; // subtract nsec carry to seconds
            result.lower = t1In.lower + (1000000000 - t2In.lower);
        } else {
            result.lower = t1In.lower - t2In.lower;
        }
        return (result.upper * 1000000) + (result.lower / 1000);
    }
}
