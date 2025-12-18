// ======================================================================
// \title  CfdpManager.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component implementation class
// ======================================================================

#include "Svc/Ccsds/CfdpManager/CfdpManager.hpp"

#include "cf_cfdp.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CfdpManager ::CfdpManager(const char* const compName) : CfdpManagerComponentBase(compName)
{
    // TODO Call engine init here or another init function?
    // May need a mem allocator
}

CfdpManager ::~CfdpManager() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void CfdpManager ::run1Hz_handler(FwIndexType portNum, U32 context) 
{
    // The timer logic built into the CFDP engine requires it to be driven at 1 Hz
    CF_CFDP_CycleEngine();
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void CfdpManager ::TODO_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Ccsds
}  // namespace Svc
