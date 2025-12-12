// ======================================================================
// \title  CfdpManager.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component implementation class
// ======================================================================

#include "Svc/Ccsds/CfdpManager/CfdpManager.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CfdpManager ::CfdpManager(const char* const compName) : CfdpManagerComponentBase(compName) {}

CfdpManager ::~CfdpManager() {}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void CfdpManager ::TODO_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Ccsds
}  // namespace Svc
