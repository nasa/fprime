// ======================================================================
// \title  ActivePhaser.cpp
// \author shaokail
// \brief  cpp file for ActivePhaser component implementation class
// ======================================================================

#include "Svc/ActivePhaser/ActivePhaser.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ActivePhaser ::ActivePhaser(const char* const compName) : ActivePhaserComponentBase(compName) {}

ActivePhaser ::~ActivePhaser() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ActivePhaser ::CycleIn_handler(FwIndexType portNum, Os::RawTime& cycleStart) {
    // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined internal interfaces
// ----------------------------------------------------------------------

void ActivePhaser ::Tick_internalInterfaceHandler() {
    // TODO
}

}  // namespace Svc
