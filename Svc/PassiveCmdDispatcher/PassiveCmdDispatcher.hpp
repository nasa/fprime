// ======================================================================
// \title  PassiveCmdDispatcher.hpp
// \author root
// \brief  hpp file for PassiveCmdDispatcher component implementation class
// ======================================================================

#ifndef Svc_PassiveCmdDispatcher_HPP
#define Svc_PassiveCmdDispatcher_HPP

#include <Svc/PassiveCmdDispatcher/PassiveCmdDispatcherComponentAc.hpp>
#include <config/CommandDispatcherImplCfg.hpp>

namespace Svc {

class PassiveCmdDispatcher final : public PassiveCmdDispatcherComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct PassiveCmdDispatcher object
    PassiveCmdDispatcher(const char* const compName  //!< The component name
    );

    //! Destroy PassiveCmdDispatcher object
    ~PassiveCmdDispatcher();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for compCmdReg
    //!
    //! Command registration input ports. Size must match the dispatch output ports.
    void compCmdReg_handler(FwIndexType portNum,  //!< The port number
                            FwOpcodeType opCode   //!< Command Op Code
                            ) override;

    //! Handler implementation for compCmdStat
    //!
    //! Input command status ports
    void compCmdStat_handler(FwIndexType portNum,             //!< The port number
                             FwOpcodeType opCode,             //!< Command Op Code
                             U32 cmdSeq,                      //!< Command Sequence
                             const Fw::CmdResponse& response  //!< The command response argument
                             ) override;

    //! Handler implementation for seqCmdBuff
    //!
    //! Command buffer input port for sequencers or other sources of command buffers
    void seqCmdBuff_handler(FwIndexType portNum,  //!< The port number
                            Fw::ComBuffer& data,  //!< Buffer containing packet data
                            U32 context           //!< Call context value; meaning chosen by user
                            ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command CMD_NO_OP
    //!
    //! No-op command
    void CMD_NO_OP_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                              U32 cmdSeq            //!< The command sequence number
                              ) override;

    //! Handler implementation for command CMD_CLEAR_TRACKING
    //!
    //! Clear command tracking info to recover from components that are not returning status
    void CMD_CLEAR_TRACKING_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                       U32 cmdSeq            //!< The command sequence number
                                       ) override;

    struct DispatchEntry {
        bool used;            //!< if entry has been used yet
        FwOpcodeType opcode;  //!< opcode of entry
        FwIndexType port;     //!< which port the entry invokes
    };

    struct SequenceTracker {
        bool used;               //!< if this slot is used
        U32 seq;                 //!< command sequence number
        FwOpcodeType opCode;     //!< opcode being tracked
        U32 context;             //!< context passed by user
        FwIndexType callerPort;  //!< port command source port
    };

    //!< Current command sequence number
    U32 m_seq;
    //! Dispatch entry table: maps incoming opcodes to the port connected to the component that
    //! implements the command
    DispatchEntry m_entryTable[CMD_DISPATCHER_DISPATCH_TABLE_SIZE];
    //! Sequence tracker table: tracks commands that are being executed but are not yet complete
    // FIXME: is this necessary since we are synchronous?
    SequenceTracker m_sequenceTracker[CMD_DISPATCHER_SEQUENCER_TABLE_SIZE];
};

}  // namespace Svc

#endif
