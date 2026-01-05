// ======================================================================
// \title  CfdpManager.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component implementation class
// ======================================================================

#include "Svc/Ccsds/CfdpManager/CfdpManager.hpp"

#include "CfdpEngine.hpp"
#include "CfdpEngine.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CfdpManager ::CfdpManager(const char* const compName) : CfdpManagerComponentBase(compName)
{
    // TODO Call engine init here or another init function?
    // May need a mem allocator

    // Temporary buffer pool for prototyping
    CF_Logical_PduBuffer_t* pduPtr = NULL;
    for(U32 i = 0; i < CFDP_MANAGER_NUM_BUFFERS; i++)
    {
        memset(&this->pduBuffers[i], 0, sizeof(CfdpPduBuffer));
        this->pduBuffers[i].inUse = false;

        pduPtr = reinterpret_cast<CF_Logical_PduBuffer_t*>(this->pduBuffers[i].data);
        FW_ASSERT(pduPtr != NULL);
        pduPtr->index = CFDP_MANAGER_NUM_BUFFERS;
        pduPtr = NULL;
    }
}

CfdpManager ::~CfdpManager() {}

void CfdpManager ::configure(void)
{
    // May need a mem allocator
    CF_CFDP_InitEngine(*this);
}

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
    FW_ASSERT(portNum < CF_NUM_CHANNELS, portNum, CF_NUM_CHANNELS);
    this->bufferDeallocate_out(portNum, data);
}

void CfdpManager ::dataIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer)
{
  CF_Channel_t* channel = NULL;
  CF_Logical_PduBuffer_t pdu;
  CF_DecoderState_t decoder;

  // There is a direct mapping between port number and channel index
  // Get the channel based on the port number
  FW_ASSERT(portNum < CF_NUM_CHANNELS, portNum, CF_NUM_CHANNELS);
  FW_ASSERT(portNum >= 0, portNum);
  channel = &cfdpEngine.channels[portNum];

  // This input port handler replicates the receive behavior in CF_CFDP_ReceiveMessage in cf_cfdp_sbintf.c
  pdu.pdec = &decoder;
  CF_CFDP_DecodeStart(pdu.pdec, fwBuffer.getData(), &pdu, fwBuffer.getSize());

  // Identify and dispatch this PDU
  CF_CFDP_ReceivePdu(channel, &pdu);
  
  // Return buffer
  this->dataInReturn_out(portNum, fwBuffer);
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
// However these functions were not directly migrated due to the
// architectural differences between F' and cFE
// ----------------------------------------------------------------------

CfdpStatus::T CfdpManager ::getPduBuffer(CF_Logical_PduBuffer_t* pduPtr, U8* msgPtr, U8 channelNum, FwSizeType size)
{
    // FwIndexType portNum;
    
    // // There is a direct mapping between channel number and port number
    // FW_ASSERT(channelNum < CF_NUM_CHANNELS, channelNum, CF_NUM_CHANNELS);
    // portNum = static_cast<FwIndexType>(channelNum);

    // return this->bufferAllocate_out(portNum, size);

    // For now, just pull a buffer from the preallocated pool
    CfdpStatus::T status = CfdpStatus::ERROR;

    FW_ASSERT(pduPtr == NULL);
    FW_ASSERT(msgPtr == NULL);

    // TODO Add output throtteling and guards here
    // CF implemented this in CF_CFDP_MsgOutGet()

    for(U32 i = 0; i < CFDP_MANAGER_NUM_BUFFERS; i++)
    {
        if(this->pduBuffers[i].inUse == false)
        {
            this->pduBuffers[i].inUse = true;
            pduPtr = &this->pduBuffers[i].pdu;
            pduPtr->index = i;
            msgPtr = this->pduBuffers[i].data;
            status = CfdpStatus::SUCCESS;
            break;
        }
    }

    // Check if we were unable to allocate a buffer
    if(status != CfdpStatus::SUCCESS)
    {
        this->log_WARNING_LO_CfdpBuffersExuasted();
    }
    return status;
}

// TODO call this from reset
// Check for other escape routes
void CfdpManager ::returnPduBuffer(U8 channelNum, CF_Logical_PduBuffer_t * pdu)
{
    // FwIndexType portNum;
    
    FW_ASSERT(pdu != NULL);
    // // There is a direct mapping between channel number and port number
    // FW_ASSERT(channelNum < CF_NUM_CHANNELS, channelNum, CF_NUM_CHANNELS);
    // portNum = static_cast<FwIndexType>(channelNum);

    // // Was unable to succesfully populate the PDU buffer, return it
    // this->bufferDeallocate_out(portNum, buffer);

    // Return to buffer pool for now
    this->returnBufferHelper(pdu);
}

void CfdpManager ::sendPduBuffer(U8 channelNum, CF_Logical_PduBuffer_t * pdu, const U8* msgPtr)
{
    FwIndexType portNum;
    FwSizeType msgSize;
    Fw::SerializeStatus status;
    
    FW_ASSERT(pdu != NULL);
    FW_ASSERT(msgPtr != NULL);
    // There is a direct mapping between channel number and port number
    FW_ASSERT(channelNum < CF_NUM_CHANNELS, channelNum, CF_NUM_CHANNELS);
    portNum = static_cast<FwIndexType>(channelNum);
    
    // TODO it would be more efficient to allocate a buffer in CF_CFDP_ConstructPduHeader()
    // However for the proof of concept I am just going to copy the data here
    // Just want the PDU header and data
    msgSize = pdu->pdu_header.header_encoded_length + pdu->pdu_header.data_encoded_length;
    Fw::Buffer buffer = this->bufferAllocate_out(portNum, msgSize);

    auto serializer = buffer.getSerializer();
    status = serializer.serializeFrom(msgPtr, msgSize, Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Full send
    this->dataOut_out(portNum, buffer);
    
    // Mark interal buffer as available
    this->returnBufferHelper(pdu);
}

  // ----------------------------------------------------------------------
  // Parameter helpers used by the CFDP engine
  // ----------------------------------------------------------------------

  CfdpEntityId CfdpManager:: getLocalEidParam(void)
  {
    Fw::ParamValid valid;
    
    // Check for coding errors as all CFDP parameters must have a default
    CfdpEntityId localEid = this->paramGet_LocalEid(valid);
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
    
    FW_ASSERT(channelIndex < CF_NUM_CHANNELS, channelIndex, CF_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    CfdpChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_ack_limit();
  }
  
  U8 CfdpManager:: getNackLimitParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CF_NUM_CHANNELS, channelIndex, CF_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    CfdpChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_nack_limit();
  }
  
  U32 CfdpManager:: getAckTimerParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CF_NUM_CHANNELS, channelIndex, CF_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    CfdpChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_ack_timer();
  }

  U32 CfdpManager:: getInactivityTimerParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CF_NUM_CHANNELS, channelIndex, CF_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    CfdpChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_inactivity_timer();
  }

  Fw::Enabled CfdpManager:: getDequeueEnabledParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CF_NUM_CHANNELS, channelIndex, CF_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    CfdpChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_dequeue_enabled();
  }

  Fw::String CfdpManager:: getMoveDirParam(U8 channelIndex)
  {
    Fw::ParamValid valid;
    
    FW_ASSERT(channelIndex < CF_NUM_CHANNELS, channelIndex, CF_NUM_CHANNELS);
    
    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    CfdpChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_move_dir();
  }

// ----------------------------------------------------------------------
// Buffer helpers
// ----------------------------------------------------------------------
void CfdpManager ::returnBufferHelper(CF_Logical_PduBuffer_t * pdu)
{
    U32 index = pdu->index;
    CfdpPduBuffer* bufferPtr;

    FW_ASSERT(pdu != NULL);
    FW_ASSERT(index < CFDP_MANAGER_NUM_BUFFERS, index, CFDP_MANAGER_NUM_BUFFERS);
    bufferPtr = &this->pduBuffers[index];

    bufferPtr->inUse = false;
    memset(&bufferPtr->data, 0, CF_MAX_PDU_SIZE);
    memset(&bufferPtr->pdu, 0, sizeof(CF_Logical_PduBuffer_t));
    bufferPtr->pdu.index = CFDP_MANAGER_NUM_BUFFERS;
}

}  // namespace Ccsds
}  // namespace Svc

