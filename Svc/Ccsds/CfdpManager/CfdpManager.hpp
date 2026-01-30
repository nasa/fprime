// ======================================================================
// \title  CfdpManager.hpp
// \author campuzan
// \brief  hpp file for CfdpManager component implementation class
// ======================================================================

#ifndef CCSDS_CFDPMANAGER_HPP
#define CCSDS_CFDPMANAGER_HPP

#include <Svc/Ccsds/CfdpManager/CfdpManagerComponentAc.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpLogicalPdu.hpp>
#include <Svc/Ccsds/CfdpManager/CfdpCodec.hpp>
#include <Svc/Ccsds/CfdpManager/Types/StatusEnumAc.hpp>

namespace Svc {
namespace Ccsds {

// Forward declarations
class CfdpEngine;
class CfdpChannel;

class CfdpManager final : public CfdpManagerComponentBase {
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
    void configure(void);

  public:
  // ----------------------------------------------------------------------
  // Port calls that are invoked by the CFDP engine
  // These functions are analogous to the functions in cf_cfdp_sbintf.*
  // However these functions are not direct ports due to the architectural
  // differences between F' and cFE
  // ----------------------------------------------------------------------

  // Equivelent of CF_CFDP_MsgOutGet
  Cfdp::Status::T getPduBuffer(Fw::Buffer& buffer, CfdpChannel& channel,
                             FwSizeType size);
  // Not sure there is an equivelent
  void returnPduBuffer(CfdpChannel& channel, Fw::Buffer& pduBuffer);
  // Equivelent of CF_CFDP_Send
  void sendPduBuffer(CfdpChannel& channel, Fw::Buffer& pduBuffer);

  public:
  // ----------------------------------------------------------------------
  // Parameter helpers used by the CFDP engine
  // ----------------------------------------------------------------------
  CfdpEntityId getLocalEidParam(void);
  U32 getOutgoingFileChunkSizeParam(void);
  U32 getRxCrcCalcBytesPerWakeupParam(void);
  Fw::String getTmpDirParam(void);
  Fw::String getFailDirParam(void);
  U8 getAckLimitParam(U8 channelIndex);
  U8 getNackLimitParam(U8 channelIndex);
  U32 getAckTimerParam(U8 channelIndex);
  U32 getInactivityTimerParam(U8 channelIndex);
  Fw::Enabled getDequeueEnabledParam(U8 channelIndex);
  Fw::String getMoveDirParam(U8 channelIndex);
  U32 getMaxOutgoingPdusPerCycleParam(U8 channelIndex);

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
        CfdpEntityId destId, //!< Destination entity id
        Cfdp::Class cfdpClass, //!< CFDP class for the file transfer
        Cfdp::Keep keep, //!< Whether or not to keep or delete the file upon completion
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
        CfdpEntityId destId, //!< Destination entity id
        Cfdp::Class cfdpClass, //!< CFDP class for the file transfer(s)
        Cfdp::Keep keep, //!< Whether or not to keep or delete the file(s) upon completion
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
        CfdpEntityId destId, //!< Destination entity id
        Cfdp::Class cfdpClass, //!< CFDP class for the file transfer(s)
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
        Cfdp::Flow freeze //!< Flow state to set
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

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    // CFDP Engine - owns all protocol state and operations
    CfdpEngine* m_engine;

    friend class CfdpManagerTester;

};

}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDPMANAGER_HPP
