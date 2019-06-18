/**
 * File: Os/LogPrintf.cpp
 * Description: an implementation on the Os::Log abstraction that routes log messages into standard
 * printf calls.
 */
#include <Os/Log.hpp>

#include <stdio.h>

namespace Os {
    // Instance implementation
    void Log::log(
        const char* fmt,
        POINTER_CAST a1,
        POINTER_CAST a2,
        POINTER_CAST a3,
        POINTER_CAST a4,
        POINTER_CAST a5,
        POINTER_CAST a6
    ) {
        (void)printf(fmt, a1, a2, a3, a4, a5, a6);
    }
}

