// ======================================================================
// \title  CfdpManager.cpp
// \author Brian Campuzano
// \brief  cpp file for CfdpManager component implementation class
// ======================================================================

#include <Fw/Com/ComPacket.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpManager.hpp>
#include <Svc/Ccsds/CfdpManager/Channel.hpp>
#include <Svc/Ccsds/CfdpManager/Engine.hpp>
#include <new>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CfdpManager ::CfdpManager(const char* const compName) : CfdpManagerComponentBase(compName), m_engine(nullptr) {}

CfdpManager ::~CfdpManager() {
    // Clean up the queue resources allocated during initialization
    this->deinit();

    // If cleanup() was not called, clean up manually
    if (this->m_engine != nullptr) {
        this->cleanup();
    }
}

void CfdpManager ::configure(Fw::MemAllocator& allocator, FwEnumStoreType memId) {
    // Allocate and initialize the CFDP engine
    FwSizeType engineSize = sizeof(Engine);
    this->m_engine = static_cast<Engine*>(allocator.allocate(memId, engineSize));
    FW_ASSERT(this->m_engine != nullptr);
    (void)new (this->m_engine) Engine(this);
    this->m_engine->init();

    // Store allocator for cleanup
    this->m_allocator = &allocator;
    this->m_allocatorId = memId;

    // Initialize telemetry counters to zero
    for (U8 i = 0; i < Cfdp::NumChannels; i++) {
        this->m_channelTelemetry[i] = Cfdp::ChannelTelemetry();
    }
}

void CfdpManager ::cleanup() {
    // Only try to deallocate if both pointers are non-null
    if ((this->m_allocator != nullptr) && (this->m_engine != nullptr)) {
        // Manually call destructor since we used placement new
        this->m_engine->~Engine();
        // Deallocate the memory
        this->m_allocator->deallocate(this->m_allocatorId, this->m_engine);
        this->m_engine = nullptr;
    }
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void CfdpManager ::run1Hz_handler(FwIndexType portNum, U32 context) {
    // The timer logic built into the CFDP engine requires it to be driven at 1 Hz
    FW_ASSERT(this->m_engine != nullptr);
    this->m_engine->cycle();

    // Emit telemetry once per second
    this->tlmWrite_ChannelTelemetry(this->m_channelTelemetry);
}

void CfdpManager ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // dataReturnIn is the allocated buffer coming back from the dataOut call
    // Port mapping is the same from bufferAllocate -> dataOut -> dataReturnIn -> bufferDeallocate
    FW_ASSERT(portNum < Cfdp::NumChannels, portNum, Cfdp::NumChannels);
    this->bufferDeallocate_out(portNum, fwBuffer);
}

void CfdpManager ::dataIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // There is a direct mapping between port number and channel index
    FW_ASSERT(portNum < Cfdp::NumChannels, portNum, Cfdp::NumChannels);
    FW_ASSERT(portNum >= 0, portNum);

    // TODO JMP Is there a more efficient way of doing this? Look into receivePdu()
    // Strip FW_PACKET_FILE descriptor (first 2 bytes) from buffer
    // FprimeRouter sends the entire Space Packet data field, which includes the packet type descriptor
    if (fwBuffer.getSize() < sizeof(FwPacketDescriptorType)) {
        // Buffer too small - silently ignore
        this->dataInReturn_out(portNum, fwBuffer);
        return;
    }

    // Read and verify packet type descriptor
    FwPacketDescriptorType packetType = 0;
    Fw::SerializeStatus status = fwBuffer.getDeserializer().deserializeTo(packetType);
    if (status != Fw::FW_SERIALIZE_OK || packetType != Fw::ComPacketType::FW_PACKET_FILE) {
        // Invalid packet type - silently ignore (consistent with FileUplink behavior)
        this->dataInReturn_out(portNum, fwBuffer);
        return;
    }

    // Create a new buffer view that skips the descriptor
    // The deserializer advanced past the 2-byte descriptor, but Engine::receivePdu
    // calls getData() which returns the raw pointer from byte 0. We need to create
    // a buffer that starts after the descriptor.
    const FwSizeType descriptorSize = sizeof(FwPacketDescriptorType);
    Fw::Buffer pduBuffer(fwBuffer.getData() + descriptorSize, fwBuffer.getSize() - descriptorSize,
                         fwBuffer.getContext());

    // Pass the adjusted buffer to the engine
    FW_ASSERT(this->m_engine != nullptr);
    this->m_engine->receivePdu(static_cast<U8>(portNum), pduBuffer);

    // Return buffer
    this->dataInReturn_out(portNum, fwBuffer);
}

Svc::SendFileResponse CfdpManager ::fileIn_handler(FwIndexType portNum,
                                                   const Fw::StringBase& sourceFileName,
                                                   const Fw::StringBase& destFileName,
                                                   U32 offset,
                                                   U32 length) {
    Svc::SendFileResponse response;
    FW_ASSERT(this->m_engine != nullptr);

    // CFDP engine does not support partial file retransmit at this time
    // Offset and length must be 0 to send the entire file
    if (offset > 0 || length > 0) {
        response.set_status(Svc::SendFileStatus::STATUS_INVALID);
        this->log_WARNING_LO_UnsupportedSendFileArguments(offset, length);
    } else {
        // Get parameters for fileIn port-initiated transfers
        Fw::ParamValid valid;
        U8 channelId = this->paramGet_FileInDefaultChannel(valid);
        FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
                  static_cast<FwAssertArgType>(valid.e));

        EntityId destEid = this->paramGet_FileInDefaultDestEntityId(valid);
        FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
                  static_cast<FwAssertArgType>(valid.e));

        Class::T cfdpClass = this->paramGet_FileInDefaultClass(valid);
        FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
                  static_cast<FwAssertArgType>(valid.e));

        Keep::T keep = this->paramGet_FileInDefaultKeep(valid);
        FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
                  static_cast<FwAssertArgType>(valid.e));

        U8 priority = this->paramGet_FileInDefaultPriority(valid);
        FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
                  static_cast<FwAssertArgType>(valid.e));

        // Attempt to initiate the file transfer (mark as port-initiated)
        Status::T status = this->m_engine->txFile(sourceFileName, destFileName, cfdpClass, keep, channelId, priority,
                                                  destEid, INIT_BY_PORT);

        // Map CFDP status to SendFileStatus
        if (status == Status::SUCCESS) {
            response.set_status(Svc::SendFileStatus::STATUS_OK);
            this->log_ACTIVITY_LO_SendFileInitiated(sourceFileName);
        } else {
            response.set_status(Svc::SendFileStatus::STATUS_ERROR);
            this->log_WARNING_LO_SendFileInitiateFail(sourceFileName);
        }
    }

    // Set context to portNum so we can identify this transaction later
    response.set_context(static_cast<U32>(portNum));

    return response;
}

void CfdpManager ::pingIn_handler(FwIndexType portNum, U32 key) {
    // send ping response
    this->pingOut_out(0, key);
}

// ----------------------------------------------------------------------
// Port calls that are invoked by the CFDP engine
// These functions are analogous to the functions in cf_cfdp_sbintf.*
// However these functions were not directly migrated due to the
// architectural differences between F' and cFE
// ----------------------------------------------------------------------

Status::T CfdpManager ::getPduBuffer(Fw::Buffer& buffer, Channel& channel, FwSizeType size) {
    Status::T status = Status::ERROR;
    FwIndexType portNum;

    // There is a direct mapping between channel index and port number
    portNum = static_cast<FwIndexType>(channel.getChannelId());

    // Check if we have reached the maximum number of output PDUs for this cycle
    U32 max_pdus = getMaxOutgoingPdusPerCycleParam(channel.getChannelId());
    if (channel.getOutgoingCounter() >= max_pdus) {
        status = Status::SEND_PDU_NO_BUF_AVAIL_ERROR;
    } else {
        buffer = this->bufferAllocate_out(portNum, size);
        // Check the allocation was successful based on size
        if (buffer.getSize() == size) {
            channel.incrementOutgoingCounter();
            status = Status::SUCCESS;
        } else {
            this->log_WARNING_LO_BuffersExhausted();
            status = Status::SEND_PDU_NO_BUF_AVAIL_ERROR;
        }
    }
    return status;
}

void CfdpManager ::returnPduBuffer(Channel& channel, Fw::Buffer& pduBuffer) {
    FwIndexType portNum;

    // There is a direct mapping between channel index and port number
    portNum = static_cast<FwIndexType>(channel.getChannelId());

    // Was unable to successfully populate the PDU buffer, return it
    this->bufferDeallocate_out(portNum, pduBuffer);
}

void CfdpManager ::sendPduBuffer(Channel& channel, Fw::Buffer& pduBuffer) {
    FwIndexType portNum;

    // There is a direct mapping between channel index and port number
    portNum = static_cast<FwIndexType>(channel.getChannelId());

    // ComQueue expects buffers to start with a 2-byte packet descriptor (APID)
    // The PDU data has already been serialized at offset PACKET_DESCRIPTOR_SIZE,
    // so we just need to write the descriptor at the beginning

    U8* bufferData = pduBuffer.getData();

    // Write FW_PACKET_FILE descriptor at the beginning (big-endian U16)
    const FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_FILE);
    bufferData[0] = static_cast<U8>((descriptor >> 8) & 0xFF);  // High byte
    bufferData[1] = static_cast<U8>(descriptor & 0xFF);         // Low byte

    // Send buffer with descriptor
    this->dataOut_out(portNum, pduBuffer);
}

void CfdpManager::sendFileComplete(Svc::SendFileStatus::T status) {
    Svc::SendFileResponse response;
    response.set_status(status);
    response.set_context(0);

    this->fileDoneOut_out(0, response);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void CfdpManager ::SendFile_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       U8 channelId,
                                       EntityId destId,
                                       Class cfdpClass,
                                       Keep keep,
                                       U8 priority,
                                       const Fw::CmdStringArg& sourceFileName,
                                       const Fw::CmdStringArg& destFileName) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    // Check channel index is in range
    rspStatus = this->checkCommandChannelIndex(channelId);
    FW_ASSERT(this->m_engine != nullptr);

    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS ==
         this->m_engine->txFile(sourceFileName, destFileName, cfdpClass.e, keep.e, channelId, priority, destId))) {
        this->log_ACTIVITY_LO_SendFileInitiated(sourceFileName);
        rspStatus = Fw::CmdResponse::OK;
    } else {
        // Engine emits specific failure reason EVR (e.g., MaxTxTransactionsReached)
        rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::PlaybackDirectory_cmdHandler(FwOpcodeType opCode,
                                                U32 cmdSeq,
                                                U8 channelId,
                                                EntityId destId,
                                                Class cfdpClass,
                                                Keep keep,
                                                U8 priority,
                                                const Fw::CmdStringArg& sourceDirectory,
                                                const Fw::CmdStringArg& destDirectory) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);
    // Check channel index is in range
    rspStatus = this->checkCommandChannelIndex(channelId);
    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS == this->m_engine->playbackDir(sourceDirectory.toChar(), destDirectory.toChar(), cfdpClass.e,
                                                        keep.e, channelId, priority, destId))) {
        this->log_ACTIVITY_LO_PlaybackInitiated(sourceDirectory);
    } else {
        // Engine emits specific failure reason EVR (e.g., PlaybackDirOpenFailed, PlaybackDirSlotUnavailable)
        rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::PollDirectory_cmdHandler(FwOpcodeType opCode,
                                            U32 cmdSeq,
                                            U8 channelId,
                                            U8 pollId,
                                            EntityId destId,
                                            Class cfdpClass,
                                            U8 priority,
                                            U32 interval,
                                            const Fw::CmdStringArg& sourceDirectory,
                                            const Fw::CmdStringArg& destDirectory) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);
    // Check channel index and poll index are in range
    rspStatus = this->checkCommandChannelIndex(channelId);
    if (rspStatus == Fw::CmdResponse::OK) {
        rspStatus = this->checkCommandChannelPollIndex(pollId);
    }

    if ((rspStatus == Fw::CmdResponse::OK) &&
        (Status::SUCCESS == this->m_engine->startPollDir(channelId, pollId, sourceDirectory, destDirectory, cfdpClass.e,
                                                         priority, destId, interval))) {
        this->log_ACTIVITY_LO_PollDirInitiated(sourceDirectory);
    } else {
        // Failure EVR was already emitted
        rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::StopPollDirectory_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, U8 pollId) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);
    // Check channel index and poll index are in range
    rspStatus = this->checkCommandChannelIndex(channelId);
    if (rspStatus == Fw::CmdResponse::OK) {
        rspStatus = this->checkCommandChannelPollIndex(pollId);
    }

    if ((rspStatus == Fw::CmdResponse::OK) && (Status::SUCCESS == this->m_engine->stopPollDir(channelId, pollId))) {
        this->log_ACTIVITY_LO_PollDirStopped(channelId, pollId);
    }
    // Failure EVR was already emitted
    // Not failing the command if the stop request failed
    // This allows operators to reinforce state prior to calling PollDirectory

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::SetChannelFlow_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId, Flow flowState) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);
    // Check channel index is in range
    rspStatus = checkCommandChannelIndex(channelId);
    if (rspStatus == Fw::CmdResponse::OK) {
        this->m_engine->setChannelFlowState(channelId, flowState);
        this->log_ACTIVITY_LO_SetFlowState(channelId, flowState);
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::SuspendResumeTransaction_cmdHandler(FwOpcodeType opCode,
                                                       U32 cmdSeq,
                                                       U8 channelId,
                                                       TransactionSeq transactionSeq,
                                                       EntityId entityId,
                                                       SuspendResume action) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);

    rspStatus = checkCommandChannelIndex(channelId);

    if (rspStatus == Fw::CmdResponse::OK) {
        Status::T status = this->m_engine->setSuspendResumeTransaction(channelId, transactionSeq, entityId, action);
        if (status == Status::SUCCESS) {
            if (action == SuspendResume::SUSPEND) {
                log_ACTIVITY_LO_TransactionSuspended(transactionSeq, entityId);
            } else {
                log_ACTIVITY_LO_TransactionResumed(transactionSeq, entityId);
            }
        } else {
            log_WARNING_LO_TransactionNotFound(transactionSeq, entityId);
            rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
        }
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::CancelTransaction_cmdHandler(FwOpcodeType opCode,
                                                U32 cmdSeq,
                                                U8 channelId,
                                                TransactionSeq transactionSeq,
                                                EntityId entityId) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);

    rspStatus = checkCommandChannelIndex(channelId);

    if (rspStatus == Fw::CmdResponse::OK) {
        Status::T status = this->m_engine->cancelTransactionBySeq(channelId, transactionSeq, entityId);
        if (status == Status::SUCCESS) {
            log_ACTIVITY_HI_TransactionCanceled(transactionSeq, entityId);
        } else {
            log_WARNING_LO_TransactionNotFound(transactionSeq, entityId);
            rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
        }
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::AbandonTransaction_cmdHandler(FwOpcodeType opCode,
                                                 U32 cmdSeq,
                                                 U8 channelId,
                                                 TransactionSeq transactionSeq,
                                                 EntityId entityId) {
    Fw::CmdResponse::T rspStatus = Fw::CmdResponse::OK;

    FW_ASSERT(this->m_engine != nullptr);

    rspStatus = checkCommandChannelIndex(channelId);

    if (rspStatus == Fw::CmdResponse::OK) {
        Status::T status = this->m_engine->abandonTransaction(channelId, transactionSeq, entityId);
        if (status == Status::SUCCESS) {
            log_ACTIVITY_HI_TransactionAbandoned(transactionSeq, entityId);
        } else {
            log_WARNING_LO_TransactionNotFound(transactionSeq, entityId);
            rspStatus = Fw::CmdResponse::EXECUTION_ERROR;
        }
    }

    this->cmdResponse_out(opCode, cmdSeq, rspStatus);
}

void CfdpManager ::ResetCounters_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U8 channelId) {
    // 0xFF means reset all channels
    if (channelId == 0xFF) {
        for (U8 i = 0; i < Cfdp::NumChannels; i++) {
            this->m_channelTelemetry[i] = Cfdp::ChannelTelemetry();
        }
        this->log_ACTIVITY_HI_ResetCounters(0xFF);
    }
    // Otherwise reset specific channel
    else if (channelId < Cfdp::NumChannels) {
        this->m_channelTelemetry[channelId] = Cfdp::ChannelTelemetry();
        this->log_ACTIVITY_HI_ResetCounters(channelId);
    } else {
        // Invalid channel ID
        this->log_WARNING_LO_InvalidChannel(channelId, Cfdp::NumChannels - 1);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    // Emit updated telemetry
    this->tlmWrite_ChannelTelemetry(this->m_channelTelemetry);

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Private command helper functions
// ----------------------------------------------------------------------

Fw::CmdResponse::T CfdpManager ::checkCommandChannelIndex(U8 channelIndex) {
    if (channelIndex >= Cfdp::NumChannels) {
        this->log_WARNING_LO_InvalidChannel(channelIndex, Cfdp::NumChannels);
        return Fw::CmdResponse::VALIDATION_ERROR;
    } else {
        return Fw::CmdResponse::OK;
    }
}

Fw::CmdResponse::T CfdpManager ::checkCommandChannelPollIndex(U8 pollIndex) {
    if (pollIndex >= CFDP_MAX_POLLING_DIR_PER_CHAN) {
        this->log_WARNING_LO_InvalidChannelPoll(pollIndex, CFDP_MAX_POLLING_DIR_PER_CHAN);
        return Fw::CmdResponse::VALIDATION_ERROR;
    } else {
        return Fw::CmdResponse::OK;
    }
}

// ----------------------------------------------------------------------
// Parameter helpers used by the CFDP engine
// ----------------------------------------------------------------------

EntityId CfdpManager::getLocalEidParam(void) {
    Fw::ParamValid valid;

    // Check for coding errors as all CFDP parameters must have a default
    EntityId localEid = this->paramGet_LocalEid(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return localEid;
}

U32 CfdpManager::getOutgoingFileChunkSizeParam(void) {
    Fw::ParamValid valid;

    // Check for coding errors as all CFDP parameters must have a default
    U32 chunkSize = this->paramGet_OutgoingFileChunkSize(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return chunkSize;
}
U32 CfdpManager::getRxCrcCalcBytesPerCycleParam(void) {
    Fw::ParamValid valid;

    // Check for coding errors as all CFDP parameters must have a default
    U32 rxSize = this->paramGet_RxCrcCalcBytesPerCycle(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    return rxSize;
}

Fw::String CfdpManager::getTmpDirParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_tmp_dir();
}

Fw::String CfdpManager::getFailDirParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_fail_dir();
}

U8 CfdpManager::getAckLimitParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_ack_limit();
}

U8 CfdpManager::getNackLimitParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_nack_limit();
}

U32 CfdpManager::getAckTimerParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_ack_timer();
}

U32 CfdpManager::getInactivityTimerParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_inactivity_timer();
}

Fw::Enabled CfdpManager::getDequeueEnabledParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_dequeue_enabled();
}

Fw::String CfdpManager::getMoveDirParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

    // Check for coding errors as all CFDP parameters must have a default
    // Get the array first
    ChannelArrayParams paramArray = paramGet_ChannelConfig(valid);
    FW_ASSERT(valid != Fw::ParamValid::INVALID && valid != Fw::ParamValid::UNINIT,
              static_cast<FwAssertArgType>(valid.e));

    // Now get individual parameter
    return paramArray[channelIndex].get_move_dir();
}

U32 CfdpManager ::getMaxOutgoingPdusPerCycleParam(U8 channelIndex) {
    Fw::ParamValid valid;

    FW_ASSERT(channelIndex < Cfdp::NumChannels, channelIndex, Cfdp::NumChannels);

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
