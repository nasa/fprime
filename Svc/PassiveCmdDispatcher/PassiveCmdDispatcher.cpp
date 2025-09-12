// ======================================================================
// \title  PassiveCmdDispatcher.cpp
// \author root
// \brief  cpp file for PassiveCmdDispatcher component implementation class
// ======================================================================

#include "Svc/PassiveCmdDispatcher/PassiveCmdDispatcher.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

PassiveCmdDispatcher::PassiveCmdDispatcher(const char* const compName) : PassiveCmdDispatcherComponentBase(compName) {}

PassiveCmdDispatcher::~PassiveCmdDispatcher() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void PassiveCmdDispatcher::compCmdReg_handler(FwIndexType portNum, FwOpcodeType opCode) {
    // TODO
}

void PassiveCmdDispatcher::compCmdStat_handler(FwIndexType portNum,
                                               FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdResponse& response) {
    // TODO
}

void PassiveCmdDispatcher::seqCmdBuff_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void PassiveCmdDispatcher::CMD_NO_OP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveCmdDispatcher::CMD_CLEAR_TRACKING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
