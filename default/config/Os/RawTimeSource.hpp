// ======================================================================
// \title Os/RawTimeSource.hpp
// \brief RawTime source selection enumeration
// ======================================================================
#ifndef OS_RAWTIMESOURCE_HPP_
#define OS_RAWTIMESOURCE_HPP_

#if defined(TGT_OS_TYPE_LINUX) || defined(TGT_OS_TYPE_DARWIN)
#include <time.h>
#endif

namespace Os {

//! \brief Timer source selection for RawTime
//!
//! Selects the clock read by `Os::RawTime::now()`. On POSIX platforms each enumerator holds the platform
//! `clockid_t` value passed directly to `clock_gettime()`. Projects may override this header in their
//! config directory to change the deployment-wide default (e.g. `RAWTIME_DEFAULT = CLOCK_MONOTONIC`) or to
//! add platform-specific sources. Values are not portable across platforms and are never serialized. The fixed
//! underlying type permits casting any platform clock id to `RawTimeSource`.
enum RawTimeSource : int {
#if defined(TGT_OS_TYPE_LINUX) || defined(TGT_OS_TYPE_DARWIN)
    RAWTIME_DEFAULT = CLOCK_REALTIME,     //!< Clock used by default-constructed RawTime objects
    RAWTIME_REALTIME = CLOCK_REALTIME,    //!< Wall-clock time; may step or slew when the system time is adjusted
    RAWTIME_MONOTONIC = CLOCK_MONOTONIC,  //!< Monotonic clock; unaffected by system time adjustments
#ifdef CLOCK_BOOTTIME
    RAWTIME_BOOTTIME = CLOCK_BOOTTIME,  //!< Monotonic clock that also advances during suspend (Linux only)
#endif
#else
    RAWTIME_DEFAULT = 0  //!< Platform's default timer
#endif
};

}  // namespace Os

#endif  // OS_RAWTIMESOURCE_HPP_
