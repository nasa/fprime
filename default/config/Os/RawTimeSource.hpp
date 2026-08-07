// ======================================================================
// \title Os/RawTimeSource.hpp
// \brief RawTime source selection enumeration
// ======================================================================
#ifndef OS_RAWTIMESOURCE_HPP_
#define OS_RAWTIMESOURCE_HPP_

namespace Os {

//! \brief Timer source selection for RawTime
//!
//! This enumeration allows platforms to define multiple timer sources.
//! Platform implementations can override this header in their config
//! directory to add platform-specific timer sources.
enum RawTimeSource {
    RAWTIME_DEFAULT = 0  //!< Platform's default timer (maintains current behavior)
};

}  // namespace Os

#endif  // OS_RAWTIMESOURCE_HPP_
