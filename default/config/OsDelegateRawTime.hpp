// ======================================================================
// \title config/OsDelegateRawTime.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::RawTime resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::RawTime aliases to Os::DelegateRawTime,
//    which wraps a RawTimeInterface reference. At construction, DelegateRawTime
//    calls RawTimeInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., PosixRawTime, StubRawTime) via placement-new.
//    The linker selects which getDelegate() based on which DefaultRawTime.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::RawTime directly to a concrete implementation
//    (e.g., Va416x0Os::TimerRawTime). This eliminates wrapper and virtual
//    dispatch, enabling inlining and aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace Va416x0Os { class TimerRawTime; }
//     namespace Os { using RawTime = Va416x0Os::TimerRawTime; }
//     #define OS_RAW_TIME_HEADER "Va416x0/Os/TimerRawTime/TimerRawTime.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::RawTime alias.
//   - OS_RAW_TIME_HEADER should point to the implementation header, which will
//     be included AFTER RawTimeInterface.hpp in Os/RawTime.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATERAWTIME_HPP
#define CONFIG_OS_DELEGATERAWTIME_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the RawTime implementation at link time
namespace Os {

class DelegateRawTime;
using RawTime = DelegateRawTime;

}  // namespace Os

#define OS_RAW_TIME_HEADER <Os/DelegateRawTime.hpp>

#endif  // CONFIG_OS_DELEGATERAWTIME_HPP
