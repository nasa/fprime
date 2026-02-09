// ======================================================================
// \title  CfdpManager.hpp
// \author campuzan
// \brief  hpp file for CfdpManager component implementation class
// ======================================================================

#ifndef CCSDS_CFDPMANAGER_HPP
#define CCSDS_CFDPMANAGER_HPP

#include <Svc/Ccsds/CfdpManager/CfdpManagerComponentAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/StatusEnumAc.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// Forward declarations
class CfdpEngine;
class CfdpChannel;
class CfdpTransaction;

class CfdpManager final : public CfdpManagerComponentBase {
  friend class CfdpManagerTester;
  // Give access to protected functions for EVRs and Telemetry
  friend class CfdpEngine;
  friend class CfdpTransaction;

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
    void configure(void);

  public:
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
  Status::T getPduBuffer(Fw::Buffer& buffer, CfdpChannel& channel,
                             FwSizeType size);

  //! Return an unused PDU buffer
  //!
  //! Deallocates a buffer that was obtained but not sent (e.g., due to error).
  //!
  //! \param channel [in] Channel that owns the buffer
  //! \param pduBuffer [in] Buffer to return/deallocate
  void returnPduBuffer(CfdpChannel& channel, Fw::Buffer& pduBuffer);

  //! Send a PDU buffer via output port
  //!
  //! Transmits a fully constructed PDU buffer via the dataOut port.
  //!
  //! \param channel [in] Channel to send on
  //! \param pduBuffer [in] Buffer containing the PDU to send
  void sendPduBuffer(CfdpChannel& channel, Fw::Buffer& pduBuffer);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for run1Hz
    //!
    //! Run port which must be invoked at 1 Hz in order to satify CFDP timer logic
    void run1Hz_handler(FwIndexType portNum,  //!< The port number
                        U32 context           //!< The call order
    ) override;

    //! Handler for input port dataReturnIn
    void dataReturnIn_handler(
            FwIndexType portNum, //!< The port number
            Fw::Buffer& data,
            const ComCfg::FrameContext& context
    ) override;

    //! Handler for input port dataIn
    void dataIn_handler(FwIndexType portNum, //!< The port number
                        Fw::Buffer& fwBuffer //!< The buffer
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

  //! Get the RX CRC calculation bytes per wakeup parameter
  //!
  //! \return Number of bytes to process per cycle when calculating received file CRC
  U32 getRxCrcCalcBytesPerWakeupParam(void);

  //! Get the temporary directory parameter
  //!
  //! \return Path to temporary directory for in-progress file transfers
  Fw::String getTmpDirParam(void);

  //! Get the failure directory parameter
  //!
  //! \return Path to directory where failed transfers are moved
  Fw::String getFailDirParam(void);

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
    CfdpEngine* m_engine;

};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDPMANAGER_HPP
