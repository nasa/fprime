/**
 * X86/IntervalTimer.cpp:
 *
 * This file supports the core functions of the IntervalTimer for X86 implementations that support
 * the following specification for the "RawTime" object:
 *
 * RawTime.lower = nanoseconds of time
 * RawTime.upper = seconds of time.
 *
 * Any implementation that fills "RawTime" via this specification can use these basic implementations.
 *
 * Note: this file is cloned from the original Linux implementation.
 */
#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>

namespace Os {

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
