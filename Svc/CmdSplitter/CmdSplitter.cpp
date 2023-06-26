// ======================================================================
// \title  CmdSplitter.cpp
// \author watney
// \brief  cpp file for CmdSplitter component implementation class
// ======================================================================

#include <FpConfig.hpp>
#include <Fw/Cmd/CmdPacket.hpp>
#include <Svc/CmdSplitter/CmdSplitter.hpp>
#include <config/CmdSplitterCfg.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

CmdSplitter ::CmdSplitter(const char* const compName) : CmdSplitterComponentBase(compName) {}

CmdSplitter ::~CmdSplitter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void CmdSplitter ::CmdBuff_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    Fw::CmdPacket cmdPkt;
    Fw::SerializeStatus stat = cmdPkt.deserialize(data);

    if (stat != Fw::FW_SERIALIZE_OK) {
        // Let the local command dispatcher deal with it
        this->LocalCmd_out(0, data, context);
    } else {
        // Check if local or remote
        if (cmdPkt.getOpCode() < CMD_SPLITTER_REMOTE_OPCODE_BASE) {
            this->LocalCmd_out(0, data, context);
        } else {
            this->RemoteCmd_out(0, data, context);
        }
    }
}

void CmdSplitter ::seqCmdStatus_handler(const NATIVE_INT_TYPE portNum,
                                        FwOpcodeType opCode,
                                        U32 cmdSeq,
                                        const Fw::CmdResponse& response) {
    // Forward the command status
    this->forwardSeqCmdStatus_out(portNum, opCode, cmdSeq, response);
}

}  // end namespace Svc
