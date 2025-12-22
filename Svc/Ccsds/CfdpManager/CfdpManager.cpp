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

CfdpStatus::T CfdpManager ::getPduBuffer(CF_Logical_PduBuffer_t* pduPtr, U8* msgPtr, U8 channelNum, FwSizeType size)
{
    // FwIndexType portNum;
    
    // // There is a direct mapping between channel number and port number
    // FW_ASSERT(channelNum < CF_NUM_CHANNELS, channelNum, CF_NUM_CHANNELS);
    // portNum = static_cast<FwIndexType>(channelNum);

    // return this->bufferAllocate_out(portNum, size);

    // For now, just pull a buffer from the preallocated pool
    CfdpStatus::T status = CfdpStatus::CFDP_ERROR;

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
            status = CfdpStatus::CFDP_SUCCESS;
            break;
        }
    }

    // Check if we were unable to allocate a buffer
    if(status != CfdpStatus::CFDP_SUCCESS)
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

