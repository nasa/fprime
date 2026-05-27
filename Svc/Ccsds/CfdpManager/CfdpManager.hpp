// ======================================================================
// \title  CfdpManager.hpp
// \author Brian Campuzano
// \brief  hpp file for CfdpManager component implementation class
// ======================================================================

#ifndef CCSDS_CFDPMANAGER_HPP
#define CCSDS_CFDPMANAGER_HPP

#include <Svc/Ccsds/CfdpManager/CfdpManagerComponentAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/StatusEnumAc.hpp>
#include <Fw/Types/MemAllocator.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// Forward declarations
class Engine;
class Channel;
class Transaction;

class CfdpManager final : public CfdpManagerComponentBase {
  friend class CfdpManagerTester;
  // Give access to protected functions for EVRs and Telemetry
  friend class Engine;
  friend class Channel;
  friend class Transaction;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct CfdpManager object
    CfdpManager(const char* const compName  //!< The component name
    );

    //! Destroy CfdpManager object
    ~CfdpManager();

    //! Configure CFDP engine
    //!
    //! Initializes the CFDP engine and allocates all memory resources needed
    //! for CFDP operations including transactions, chunks, and histories.
    //! Must be called once after construction and before any CFDP operations.
    //!
    //! \param allocator Memory allocator to use for Engine allocation
    //! \param memId Allocator ID for deallocation
    void configure(Fw::MemAllocator& allocator, FwEnumStoreType memId = 0);

    //! Cleanup CFDP engine and deallocate resources
    void cleanup();

  public:
  // ----------------------------------------------------------------------
  // Constants
  // ----------------------------------------------------------------------

  //! Size of packet descriptor prepended to PDUs for ComQueue
  static constexpr FwSizeType PACKET_DESCRIPTOR_SIZE = sizeof(FwPacketDescriptorType);

  // ----------------------------------------------------------------------
  // Port calls that are invoked by the CFDP engine
  // These functions are analogous to the functions in cf_cfdp_sbintf.*
  // However these functions are not direct ports due to the architectural
  // differences between F' and cFE
  // ----------------------------------------------------------------------

  //! Get a buffer for constructing an outgoing CFDP PDU
  //!
  //! Allocates a buffer from the downstream component for building a PDU.
  //! Checks against the maximum number of PDUs allowed per cycle.
  //! Equivalent to CF_CFDP_MsgOutGet in cFS.
  //!
  //! \param buffer [out] Buffer object to be populated with allocated memory
  //! \param channel [in] Channel to allocate buffer for
  //! \param size [in] Size of buffer needed in bytes
  //! \return Status::SUCCESS if buffer allocated, Status::SEND_PDU_NO_BUF_AVAIL_ERROR otherwise
  Status::T getPduBuffer(Fw::Buffer& buffer, Channel& channel,
                             FwSizeType size);

  //! Return an unused PDU buffer
  //!
  //! Deallocates a buffer that was obtained but not sent (e.g., due to error).
  //!
  //! \param channel [in] Channel that owns the buffer
  //! \param pduBuffer [in] Buffer to return/deallocate
  void returnPduBuffer(Channel& channel, Fw::Buffer& pduBuffer);

  //! Send a PDU buffer via output port
  //!
  //! Transmits a fully constructed PDU buffer via the dataOut port.
  //!
  //! \param channel [in] Channel to send on
  //! \param pduBuffer [in] Buffer containing the PDU to send
  void sendPduBuffer(Channel& channel, Fw::Buffer& pduBuffer);

  //! Send file completion notification for port-initiated transfers
  //!
  //! Invokes the fileDoneOut output port with the transaction status.
  //!
  //! \param status Transaction completion status
  void sendFileComplete(Svc::SendFileStatus::T status);

  // ----------------------------------------------------------------
  // Telemetry helper methods (public for Engine/Transaction access)
  // ----------------------------------------------------------------

  //! Increment receive error counter
  void incrementRecvErrors(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_recvErrors(m_channelTelemetry[chanId].get_recvErrors() + 1);
  }

  //! Increment receive dropped counter
  void incrementRecvDropped(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_recvDropped(m_channelTelemetry[chanId].get_recvDropped() + 1);
  }

  //! Increment receive spurious counter
  void incrementRecvSpurious(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_recvSpurious(m_channelTelemetry[chanId].get_recvSpurious() + 1);
  }

  //! Add to received file data bytes
  void addRecvFileDataBytes(U8 chanId, U32 bytes) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_recvFileDataBytes(m_channelTelemetry[chanId].get_recvFileDataBytes() + bytes);
  }

  //! Add to received NAK segment requests
  void addRecvNakSegmentRequests(U8 chanId, U32 count) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_recvNakSegmentRequests(m_channelTelemetry[chanId].get_recvNakSegmentRequests() + count);
  }

  //! Increment received PDU counter
  void incrementRecvPdu(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_recvPdu(m_channelTelemetry[chanId].get_recvPdu() + 1);
  }

  //! Add to sent NAK segment requests
  void addSentNakSegmentRequests(U8 chanId, U32 count) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_sentNakSegmentRequests(m_channelTelemetry[chanId].get_sentNakSegmentRequests() + count);
  }

  //! Add sent file data bytes
  void addSentFileDataBytes(U8 chanId, U32 bytes) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_sentFileDataBytes(m_channelTelemetry[chanId].get_sentFileDataBytes() + bytes);
  }

  //! Increment sent PDU counter
  void incrementSentPdu(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_sentPdu(m_channelTelemetry[chanId].get_sentPdu() + 1);
  }

  //! Increment fault ACK limit counter
  void incrementFaultAckLimit(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultAckLimit(m_channelTelemetry[chanId].get_faultAckLimit() + 1);
  }

  //! Increment fault NAK limit counter
  void incrementFaultNakLimit(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultNakLimit(m_channelTelemetry[chanId].get_faultNakLimit() + 1);
  }

  //! Increment fault inactivity timer counter
  void incrementFaultInactivityTimer(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultInactivityTimer(m_channelTelemetry[chanId].get_faultInactivityTimer() + 1);
  }

  //! Increment fault CRC mismatch counter
  void incrementFaultCrcMismatch(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultCrcMismatch(m_channelTelemetry[chanId].get_faultCrcMismatch() + 1);
  }

  //! Increment fault file size mismatch counter
  void incrementFaultFileSizeMismatch(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultFileSizeMismatch(m_channelTelemetry[chanId].get_faultFileSizeMismatch() + 1);
  }

  //! Increment fault file open counter
  void incrementFaultFileOpen(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultFileOpen(m_channelTelemetry[chanId].get_faultFileOpen() + 1);
  }

  //! Increment fault file read counter
  void incrementFaultFileRead(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultFileRead(m_channelTelemetry[chanId].get_faultFileRead() + 1);
  }

  //! Increment fault file write counter
  void incrementFaultFileWrite(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultFileWrite(m_channelTelemetry[chanId].get_faultFileWrite() + 1);
  }

  //! Increment fault file seek counter
  void incrementFaultFileSeek(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultFileSeek(m_channelTelemetry[chanId].get_faultFileSeek() + 1);
  }

  //! Increment fault file rename counter
  void incrementFaultFileRename(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultFileRename(m_channelTelemetry[chanId].get_faultFileRename() + 1);
  }

  //! Increment fault directory read counter
  void incrementFaultDirectoryRead(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    m_channelTelemetry[chanId].set_faultDirectoryRead(m_channelTelemetry[chanId].get_faultDirectoryRead() + 1);
  }

  //! Get reference to channel telemetry for queue depth updates
  Cfdp::ChannelTelemetry& getChannelTelemetryRef(U8 chanId) {
    FW_ASSERT(chanId < Cfdp::NumChannels);
    return m_channelTelemetry[chanId];
  }

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for run1Hz
    //!
    //! Run port which must be invoked at 1 Hz in order to satisfy CFDP timer logic
    void run1Hz_handler(FwIndexType portNum,  //!< The port number
                        U32 context           //!< The call order
    ) override;

    //! Handler for input port dataReturnIn
    void dataReturnIn_handler(
            FwIndexType portNum, //!< The port number
            Fw::Buffer& fwBuffer
    ) override;

    //! Handler for input port dataIn
    void dataIn_handler(FwIndexType portNum, //!< The port number
                        Fw::Buffer& fwBuffer //!< The buffer
    ) override;

    //! Handler for input port fileIn
    Svc::SendFileResponse fileIn_handler(
        FwIndexType portNum, //!< The port number
        const Fw::StringBase& sourceFileName, //!< Path of file to send
        const Fw::StringBase& destFileName, //!< Path to store file at destination
        U32 offset, //!< Byte offset to start reading from
        U32 length //!< Number of bytes to read (0 = entire file)
    ) override;

    //! Handler for input port pingIn
    void pingIn_handler(
        FwIndexType portNum, //!< The port number
        U32 key //!< Value to return to pinger
    ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler for command SendFile
    //!
    //! Command to start a CFDP file transaction
    void SendFile_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID for the file transaction
        EntityId destId, //!< Destination entity id
        Class cfdpClass, //!< CFDP class for the file transfer
        Keep keep, //!< Whether or not to keep or delete the file upon completion
        U8 priority, //!< Priority: 0=highest priority
        const Fw::CmdStringArg& sourceFileName, //!< The name of the on-board file to send
        const Fw::CmdStringArg& destFileName //!< The name of the destination file on the ground
    ) override;

    //! Handler for command PlaybackDirectory
    //!
    //! Command to start a directory playback
    void PlaybackDirectory_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID for the file transaction(s)
        EntityId destId, //!< Destination entity id
        Class cfdpClass, //!< CFDP class for the file transfer(s)
        Keep keep, //!< Whether or not to keep or delete the file(s) upon completion
        U8 priority, //!< Priority: 0=highest priority
        const Fw::CmdStringArg& sourceDirectory, //!< The name of the on-board directory to send
        const Fw::CmdStringArg& destDirectory //!< The name of the destination directory on the ground
    ) override;

    //! Handler for command PollDirectory
    //!
    //! Command to start a directory poll
    void PollDirectory_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID for the file transaction(s)
        U8 pollId, //!< Channel poll ID for the file transaction(s)
        EntityId destId, //!< Destination entity id
        Class cfdpClass, //!< CFDP class for the file transfer(s)
        U8 priority, //!< Priority: 0=highest priority
        U32 interval, //!< Interval to poll the directory in seconds
        const Fw::CmdStringArg& sourceDirectory, //!< The name of the on-board directory to send
        const Fw::CmdStringArg& destDirectory //!< The name of the destination directory on the ground
    ) override;

    //! Handler for command StopPollDirectory
    //!
    //! Command to stop a directory poll
    void StopPollDirectory_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID to stop
        U8 pollId //!< Channel poll ID to stop
    ) override;

    //! Handler for command SetChannelFlow
    //!
    //! Command to set channel's flow status
    void SetChannelFlow_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID to set
        Flow freeze //!< Flow state to set
    ) override;

    //! Handler for command SuspendResumeTransaction
    //!
    //! Command to suspend or resume a transaction
    void SuspendResumeTransaction_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID for the transaction
        TransactionSeq transactionSeq, //!< Transaction sequence number
        EntityId entityId, //!< Entity ID of the transaction
        SuspendResume action //!< Action to take: SUSPEND or RESUME
    ) override;

    //! Handler for command CancelTransaction
    //!
    //! Command to cancel a transaction with graceful close-out
    void CancelTransaction_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID for the transaction
        TransactionSeq transactionSeq, //!< Transaction sequence number
        EntityId entityId //!< Entity ID of the transaction
    ) override;

    //! Handler for command AbandonTransaction
    //!
    //! Command to abandon a transaction immediately
    void AbandonTransaction_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId, //!< Channel ID for the transaction
        TransactionSeq transactionSeq, //!< Transaction sequence number
        EntityId entityId //!< Entity ID of the transaction
    ) override;

    //! Handler for command ResetCounters
    //!
    //! Command to reset telemetry counters
    void ResetCounters_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U8 channelId //!< Channel ID to reset (0xFF for all channels)
    ) override;

  private:
    // ----------------------------------------------------------------------
    // Private command helper functions
    // ----------------------------------------------------------------------

    //! Checks if the requested channel index is valid, and emits an EVR if not
    Fw::CmdResponse::T checkCommandChannelIndex(U8 channelIndex //!< The channel index to check
    );

    //! Checks if the requested channel poll index is valid, and emits an EVR if not
    Fw::CmdResponse::T checkCommandChannelPollIndex(U8 pollIndex //!< The poll index to check
    );

  public:
  // ----------------------------------------------------------------------
  // Parameter helpers used by the CFDP engine
  // ----------------------------------------------------------------------

  //! Get the local entity ID parameter
  //!
  //! \return The local CFDP entity ID
  EntityId getLocalEidParam(void);

  //! Get the outgoing file chunk size parameter
  //!
  //! \return Maximum size in bytes for file data segments in outgoing PDUs
  U32 getOutgoingFileChunkSizeParam(void);

  //! Get the RX CRC calculation bytes per scheduler cycle parameter
  //!
  //! \return Number of bytes to process per cycle when calculating received file CRC
  U32 getRxCrcCalcBytesPerCycleParam(void);

  //! Get the temporary directory parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Path to temporary directory for in-progress file transfers
  Fw::String getTmpDirParam(U8 channelIndex);

  //! Get the failure directory parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Path to directory where failed transfers are moved
  Fw::String getFailDirParam(U8 channelIndex);

  //! Get the ACK limit parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Maximum number of times to retry sending an ACK PDU
  U8 getAckLimitParam(U8 channelIndex);

  //! Get the NAK limit parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Maximum number of times to retry sending a NAK PDU
  U8 getNackLimitParam(U8 channelIndex);

  //! Get the ACK timer parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return ACK timeout value in seconds
  U32 getAckTimerParam(U8 channelIndex);

  //! Get the inactivity timer parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Inactivity timeout value in seconds
  U32 getInactivityTimerParam(U8 channelIndex);

  //! Get the dequeue enabled parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Whether the channel is enabled for dequeuing transactions
  Fw::Enabled getDequeueEnabledParam(U8 channelIndex);

  //! Get the move directory parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Path to directory where completed transfers are moved
  Fw::String getMoveDirParam(U8 channelIndex);

  //! Get the maximum outgoing PDUs per cycle parameter for a channel
  //!
  //! \param channelIndex [in] Index of the channel
  //! \return Maximum number of PDUs that can be sent per engine cycle
  U32 getMaxOutgoingPdusPerCycleParam(U8 channelIndex);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    // CFDP Engine - owns all protocol state and operations
    Engine* m_engine;

    //! Telemetry array for all CFDP channels
    Cfdp::ChannelTelemetryArray m_channelTelemetry;

    //! Stored for cleanup
    FwEnumStoreType m_allocatorId = 0;
    Fw::MemAllocator* m_allocator = nullptr;

};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDPMANAGER_HPP
