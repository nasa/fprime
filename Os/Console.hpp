// ======================================================================
// \title Os/Console.hpp
// \brief public Os::Console interface and alias
//
// This header aggregates all definitions needed to use Os::Console:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsDelegateConsole.hpp (CFG) defines the Os::Console type alias by
//      forward-declaring a link-time delegate (e.g. DelegateConsole) or directly
//      aliasing a concrete implementation (e.g. Va416x0::GdsConsole).
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/ConsoleInterface.hpp.
//
//   2. Os/ConsoleInterface.hpp (IF) includes CFG first (so the Os::Console alias
//      is available), then defines ConsoleHandle and ConsoleInterface.
//
//   3. OS_CONSOLE_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateConsole.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation (e.g. GdsConsole.hpp).
//
// ConsoleInterface.hpp must precede OS_CONSOLE_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef Os_Console_hpp_
#define Os_Console_hpp_

#include "Os/ConsoleInterface.hpp"

// Validate that OS_CONSOLE_HEADER was defined by config/OsDelegateConsole.hpp
#ifndef OS_CONSOLE_HEADER
#error "OS_CONSOLE_HEADER must be defined in config/OsDelegateConsole.hpp"
#endif

#include OS_CONSOLE_HEADER

#endif
