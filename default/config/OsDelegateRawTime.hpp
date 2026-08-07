// ======================================================================
// \title config/OsDelegateRawTime.hpp
// \brief configured selection of Os OSAL implementations
//
// This header selects, at compile time, which implementation each Os OSAL
// symbol (e.g. Os::RawTime) refers to. By default the symbol is an alias for
// the link-time delegate (e.g. Os::DelegateRawTime), preserving the historical
// behavior of selecting the implementation at link time.
//
// A platform that prefers compile-time implementation selection (for the
// performance benefit of avoiding indirect/virtual dispatch) may override this
// header in its own config/ folder and alias the symbol directly to a concrete
// implementation.
// ======================================================================
#ifndef CONFIG_OS_DELEGATERAWTIME_HPP
#define CONFIG_OS_DELEGATERAWTIME_HPP

// #include "Os/DelegateRawTime.hpp"  // complete type for the aliased class

//!< Forward declaration of the link-time delegate
//!< Default: select the RawTime implementation at link time

namespace Os {

class DelegateRawTime;
using RawTime = DelegateRawTime;

}  // namespace Os

#define OS_RAW_TIME_HEADER <Os/DelegateRawTime.hpp>

#endif  // CONFIG_OS_DELEGATERAWTIME_HPP
