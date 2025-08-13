// ======================================================================
// \title  FatalHandlerImpl.cpp
// \author lestarch
// \brief  cpp file for FatalHandler component implementation class
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>
#include <cstdlib>

namespace Svc {

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FatalHandlerComponentImpl::FatalReceive_handler(const FwIndexType portNum, FwEventIdType Id) {
    Fw::Logger::log("FATAL %" PRI_FwEventIdType "handled.\n", Id);
    while (true) {
    }  // Returning might be bad
}

}  // end namespace Svc
