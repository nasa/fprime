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


void CfdpManager ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context)
{
    // dataReturnIn is the allocated buffer coming back from the dataOut call
    // Port mapping is the same from bufferAllocate -> dataOut -> dataReturnIn -> bufferDeallocate
    this->bufferDeallocate_out(portNum, data);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void CfdpManager ::TODO_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Port calls that are invoked by the CFDP engine
// These functions are analogous to the functions in cf_cfdp_sbintf.*
// However these functions are not direct ports due to the architectural
// differences between F' and cFE
// ----------------------------------------------------------------------

Fw::Buffer CfdpManager ::cfdpGetMessageBuffer(U8 channelNum, FwSizeType size)
{
    FwIndexType portNum;
    
    // There is a direct mapping between channel number and port number
    FW_ASSERT(channelNum < CF_NUM_CHANNELS, channelNum, CF_NUM_CHANNELS);
    portNum = static_cast<FwIndexType>(channelNum);

    Fw::Buffer buffer = this->bufferAllocate_out(portNum, size);
    return buffer;
}

}  // namespace Ccsds
}  // namespace Svc
