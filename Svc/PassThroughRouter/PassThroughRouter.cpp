// ======================================================================
// \title  PassThroughRouter.cpp
// \author kessler
// \brief  cpp file for PassThroughRouter component implementation class
// ======================================================================

#include "Svc/PassThroughRouter/PassThroughRouter.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

PassThroughRouter::PassThroughRouter(const char* const compName) : PassThroughRouterComponentBase(compName) {}

PassThroughRouter::~PassThroughRouter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void PassThroughRouter::allPacketsReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    ComCfg::FrameContext context = {};  // default context used to satisfy dataReturnOut port interface
    this->dataReturnOut_out(0, fwBuffer, context);
}

void PassThroughRouter::dataIn_handler(FwIndexType portNum,
                                       Fw::Buffer& packetBuffer,
                                       const ComCfg::FrameContext& context) {
    // allPacketsOut must always be connected. Users needing a null-router should write one explicitly.
    this->allPacketsOut_out(0, packetBuffer);
}

}  // namespace Svc
