// ======================================================================
// \title  CmdSplitter.cpp
// \author watney
// \brief  cpp file for CmdSplitter component implementation class
// ======================================================================

#include <FpConfig.hpp>
#include <Fw/Cmd/CmdPacket.hpp>
#include <Svc/CmdSplitter/CmdSplitter.hpp>
#include <Fw/Types/Assert.hpp>
#include <FppConstantsAc.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

CmdSplitter ::CmdSplitter(const char* const compName) : CmdSplitterComponentBase(compName) {}

CmdSplitter ::~CmdSplitter() {}

void CmdSplitter ::configure(const FwOpcodeType remoteBaseOpcode) {
    this->m_remoteBase = remoteBaseOpcode;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void CmdSplitter ::CmdBuff_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    Fw::CmdPacket cmdPkt;
    Fw::SerializeStatus stat = cmdPkt.deserialize(data);

    FW_ASSERT(portNum < CmdSplitterPorts);
    if (stat != Fw::FW_SERIALIZE_OK) {
        // Let the local command dispatcher deal with it
        this->LocalCmd_out(portNum, data, context);
    } else {
        // Check if local or remote
        if (cmdPkt.getOpCode() < this->m_remoteBase) {
            this->LocalCmd_out(portNum, data, context);
        } else {
            this->RemoteCmd_out(portNum, data, context);
        }
    }
}

void CmdSplitter ::seqCmdStatus_handler(const NATIVE_INT_TYPE portNum,
                                        FwOpcodeType opCode,
                                        U32 cmdSeq,
                                        const Fw::CmdResponse& response) {
    FW_ASSERT(portNum < CmdSplitterPorts);
    // Forward the command status
    this->forwardSeqCmdStatus_out(portNum, opCode, cmdSeq, response);
}

}  // end namespace Svc
