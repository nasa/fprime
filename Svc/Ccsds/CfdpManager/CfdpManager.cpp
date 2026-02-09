// ======================================================================
// \title  CfdpManager.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component implementation class
// ======================================================================

#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpEngine.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpChannel.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CfdpManager ::CfdpManager(const char* const compName) :
    CfdpManagerComponentBase(compName),
    m_engine(nullptr)
{
    // Create the CFDP engine
    this->m_engine = new CfdpEngine(this);
    FW_ASSERT(this->m_engine != nullptr);
}

CfdpManager ::~CfdpManager() {
    // Clean up the queue resources allocated during initialization
    this->deinit();

    delete this->m_engine;
    this->m_engine = nullptr;
}

void CfdpManager ::configure(void)
{
    // TODO BPC: Do we need a mem allocator here?
    this->m_engine->init();
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void CfdpManager ::run1Hz_handler(FwIndexType portNum, U32 context)
{
    // The timer logic built into the CFDP engine requires it to be driven at 1 Hz
    this->m_engine->cycle();
}

void CfdpManager ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context)
{
    // dataReturnIn is the allocated buffer coming back from the dataOut call
    // Port mapping is the same from bufferAllocate -> dataOut -> dataReturnIn -> bufferDeallocate
    FW_ASSERT(portNum < CFDP_NUM_CHANNELS, portNum, CFDP_NUM_CHANNELS);
    this->bufferDeallocate_out(portNum, data);
}

void CfdpManager ::dataIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer)
{
  // There is a direct mapping between port number and channel index
  FW_ASSERT(portNum < CFDP_NUM_CHANNELS, portNum, CFDP_NUM_CHANNELS);
  FW_ASSERT(portNum >= 0, portNum);

  // Pass buffer to the engine to deserialize
  this->m_engine->receivePdu(static_cast<U8>(portNum), fwBuffer);

  // Return buffer
  this->dataInReturn_out(portNum, fwBuffer);
}

// ----------------------------------------------------------------------
// Port calls that are invoked by the CFDP engine
// These functions are analogous to the functions in cf_cfdp_sbintf.*
// However these functions were not directly migrated due to the
// architectural differences between F' and cFE
// ----------------------------------------------------------------------

Status::T CfdpManager ::getPduBuffer(Fw::Buffer& buffer, CfdpChannel& channel,
                                           FwSizeType size)
{
    Status::T status = Status::ERROR;
    FwIndexType portNum;

    // There is a direct mapping between channel index and port number
    portNum = static_cast<FwIndexType>(channel.getChannelId());

    // Check if we have reached the maximum number of output PDUs for this cycle
    U32 max_pdus = getMaxOutgoingPdusPerCycleParam(channel.getChannelId());
    if (channel.getOutgoingCounter() >= max_pdus)
    {
        status = Status::SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        buffer = this->bufferAllocate_out(portNum, size);
        // Check the allocation was successful based on size
        if(buffer.getSize() == size)
        {
            channel.incrementOutgoingCounter();
            status = Status::SUCCESS;
        }
        else
        {
            this->log_WARNING_LO_BuffersExuasted();
            status = Status::SEND_PDU_NO_BUF_AVAIL_ERROR;
        }
    }
    return status;
}

void CfdpManager ::returnPduBuffer(CfdpChannel& channel, Fw::Buffer& pduBuffer)
{
    FwIndexType portNum;

    // There is a direct mapping between channel index and port number
    portNum = static_cast<FwIndexType>(channel.getChannelId());

    // Was unable to succesfully populate the PDU buffer, return it
    this->bufferDeallocate_out(portNum, pduBuffer);
}

void CfdpManager ::sendPduBuffer(CfdpChannel& channel, Fw::Buffer& pduBuffer)
{
    FwIndexType portNum;

    // There is a direct mapping between channel index and port number
    portNum = static_cast<FwIndexType>(channel.getChannelId());

    // Full send
    this->dataOut_out(portNum, pduBuffer);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void CfdpManager ::SendFile_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, EntityId destId,
                                       Class cfdpClass, Keep keep, U8 priority,
                                       const Fw::CmdStringArg& sourceFileName,
                                       const Fw::CmdStringArg& destFileName)
{
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    // Check channel index is in range
    rspStatus = this->checkCommandChannelIndex(channelId);

    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS == this->m_engine->txFile(sourceFileName, destFileName, cfdpClass.e, keep.e,
                                                        channelId, priority, destId)))
    {
        this->log_ACTIVITY_LO_SendFileInitiatied(sourceFileName);
        rspStatus = Fw::CmdResponse::OK;
    }
    else
    {
        // TODO BPC: Was failure reason already emitted?
        // Do we need this EVR?
        this->log_WARNING_LO_SendFileInitiateFail(sourceFileName);
        rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::PlaybackDirectory_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, EntityId destId,
                                                Class cfdpClass, Keep keep, U8 priority,
                                                const Fw::CmdStringArg& sourceDirectory,
                                                const Fw::CmdStringArg& destDirectory)
{
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    // Check channel index is in range
    rspStatus = this->checkCommandChannelIndex(channelId);

    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS == this->m_engine->playbackDir(sourceDirectory.toChar(), destDirectory.toChar(), cfdpClass.e,
                                                             keep.e, channelId, priority, destId)))
    {
        this->log_ACTIVITY_LO_PlaybackInitiatied(sourceDirectory);
    }
    else
    {
        // TODO BPC: Was failure reason already emitted?
        // Do we need this EVR?
        this->log_WARNING_LO_PlaybackInitiateFail(sourceDirectory);
        rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::PollDirectory_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, U8 pollId,
                                            EntityId destId, Class cfdpClass, U8 priority,
                                            U32 interval, const Fw::CmdStringArg& sourceDirectory,
                                            const Fw::CmdStringArg& destDirectory)
{
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    // Check channel index and poll index are in range
    rspStatus = this->checkCommandChannelIndex(channelId);
    if (rspStatus == Fw::CmdResponse::OK)
    {
      rspStatus = this->checkCommandChannelPollIndex(pollId);
    }

    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS == this->m_engine->startPollDir(channelId, pollId, sourceDirectory, destDirectory,
                                                              cfdpClass.e, priority, destId, interval)))
    {
        this->log_ACTIVITY_LO_PollDirInitiatied(sourceDirectory);
    }
    else
    {
        // Failure EVR was already emitted
        rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::StopPollDirectory_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, U8 pollId)
{
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    // Check channel index and poll index are in range
    rspStatus = this->checkCommandChannelIndex(channelId);
    if (rspStatus == Fw::CmdResponse::OK)
    {
      rspStatus = this->checkCommandChannelPollIndex(pollId);
    }

    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS == this->m_engine->stopPollDir(channelId, pollId)))
    {
        this->log_ACTIVITY_LO_PollDirStopped(channelId, pollId);
    }
    // Failure EVR was already emitted
    // Not failing the command if the stop request failed
    // This allows operators to reinforce state prior to calling PollDirectory

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::SetChannelFlow_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, Flow flowState)
{
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    // Check channel index is in range
    rspStatus = checkCommandChannelIndex(channelId);
    if (rspStatus == Fw::CmdResponse::OK)
    {
        this->m_engine->setChannelFlowState(channelId, flowState);
        this->log_ACTIVITY_LO_SetFlowState(channelId, flowState);
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);

}

// ----------------------------------------------------------------------
// Private command helper functions
// ----------------------------------------------------------------------

Fw::CmdResponse::T CfdpManager ::checkCommandChannelIndex(U8 channelIndex)
{
    if(channelIndex >= CfdpManagerNumChannels)
    {
        this->log_WARNING_LO_InvalidChannel(channelIndex, CfdpManagerNumChannels);
        return Fw::CmdResponse::VALIDATION_ERROR;
    }
    else
    {
      return Fw::CmdResponse::OK;
    }
}

Fw::CmdResponse::T CfdpManager ::checkCommandChannelPollIndex(U8 pollIndex)
{
    if(pollIndex >= CFDP_MAX_POLLING_DIR_PER_CHAN)
    {
        this->log_WARNING_LO_InvalidChannelPoll(pollIndex, CFDP_MAX_POLLING_DIR_PER_CHAN);
        return Fw::CmdResponse::VALIDATION_ERROR;
    }
    else
    {
      return Fw::CmdResponse::OK;
    }
}

  // ----------------------------------------------------------------------
  // Parameter helpers used by the CFDP engine
  // ----------------------------------------------------------------------

  EntityId CfdpManager:: getLocalEidParam(void)
  {
    Fw::ParamValid valid;
    
    // Check for coding errors as all CFDP parameters must have a default
    EntityId localEid = this->paramGet_LocalEid(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return localEid;
  }

  U32 CfdpManager:: getOutgoingFileChunkSizeParam(void)
  {
    Fw::ParamValid valid;
    
    // Check for coding errors as all CFDP parameters must have a default
    U32 chunkSize = this->paramGet_OutgoingFileChunkSize(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return chunkSize;
  }
  U32 CfdpManager:: getRxCrcCalcBytesPerWakeupParam(void)
  {
    Fw::ParamValid valid;
    
    // Check for coding errors as all CFDP parameters must have a default
    U32 rxSize = this->paramGet_RxCrcCalcBytesPerWakeup(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return rxSize;
  }
  
  Fw::String CfdpManager:: getTmpDirParam(void)
  {
    Fw::ParamValid valid;
    
    // Check for coding errors as all CFDP parameters must have a default
    Fw::String tmpDir = this->paramGet_TmpDir(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return tmpDir;
  }

  Fw::String CfdpManager:: getFailDirParam(void)
  {
    Fw::ParamValid valid;
    
    // Check for coding errors as all CFDP parameters must have a default
    Fw::String failDir = this->paramGet_TmpDir(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return failDir;
  }

  U8 CfdpManager:: getAckLimitParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_ack_limit();
  }
  
  U8 CfdpManager:: getNackLimitParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_nack_limit();
  }
  
  U32 CfdpManager:: getAckTimerParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_ack_timer();
  }

  U32 CfdpManager:: getInactivityTimerParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_inactivity_timer();
  }

  Fw::Enabled CfdpManager:: getDequeueEnabledParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_dequeue_enabled();
  }

  Fw::String CfdpManager:: getMoveDirParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_move_dir();
  }

  U32 CfdpManager ::getMaxOutgoingPdusPerCycleParam(U8 channelIndex)
  {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < CFDP_NUM_CHANNELS, channelIndex, CFDP_NUM_CHANNELS);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_max_outgoing_pdus_per_cycle();
  }

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

