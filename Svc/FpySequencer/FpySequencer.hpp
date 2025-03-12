// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer component implementation class
// ======================================================================

#ifndef FpySequencer_HPP
#define FpySequencer_HPP

#include "FppConstantsAc.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Fw/Types/WaitEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/FpySequencer/DirectiveIdEnumAc.hpp"
#include "Svc/FpySequencer/FooterSerializableAc.hpp"
#include "Svc/FpySequencer/FpySequencerComponentAc.hpp"
#include "Svc/FpySequencer/FpySequencer_GoalStateEnumAc.hpp"
#include "Svc/FpySequencer/HeaderSerializableAc.hpp"
#include "Svc/FpySequencer/SequenceSerializableAc.hpp"
#include "Svc/FpySequencer/StatementSerializableAc.hpp"

namespace Svc {

class FpySequencer : public FpySequencerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object FpySequencer
    //!
    FpySequencer(const char* const compName /*!< The component name*/
    );

    //! Destroy object FpySequencer
    //!
    ~FpySequencer();

    PRIVATE :

        //! Handler for command RUN
        //!
        //! Loads, validates and runs a sequence
        void
        RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                       U32 cmdSeq,                        //!< The command sequence number
                       const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                       FpySequencer_BlockState block      //!< Return command status when complete or not
                       ) override;

    //! Handler for command VALIDATE
    //!
    //! Loads and validates a sequence
    void VALIDATE_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                             U32 cmdSeq,                       //!< The command sequence number
                             const Fw::CmdStringArg& fileName  //!< The name of the sequence file
                             ) override;

    //! Handler for command RUN_VALIDATED
    //!
    //! Runs a previously validated sequence
    void RUN_VALIDATED_cmdHandler(FwOpcodeType opCode,           //!< The opcode
                                  U32 cmdSeq,                    //!< The command sequence number
                                  FpySequencer_BlockState block  //!< Return command status when complete or not
                                  ) override;

    //! Handler for command CANCEL
    //!
    //! Cancels a running or validated sequence
    void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq            //!< The command sequence number
                           ) override;

    // ----------------------------------------------------------------------
    // Functions to implement for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action signalEntered of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! simply raises the "entered" signal
    void Svc_FpySequencer_SequencerStateMachine_action_signalEntered(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setSequenceFilePath of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the current sequence file path member var
    void Svc_FpySequencer_SequencerStateMachine_action_setSequenceFilePath(
        SmId smId,                                              //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
        ) override;

    //! Implementation for action setSequenceBlockState of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the block state of the sequence to be run
    void Svc_FpySequencer_SequencerStateMachine_action_setSequenceBlockState(
        SmId smId,                                              //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_SequenceExecutionArgs& value    //!< The value
        ) override;

    //! Implementation for action validate of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! performs all steps necessary for sequence validation, and raises a signal result_success or result_failure
    void Svc_FpySequencer_SequencerStateMachine_action_validate(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqSucceeded of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence succeeded
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqSucceeded(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqCancelled of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence was cancelled
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqCancelled(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqFailed of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence failed
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_invalidCmd of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! warns that the user cmd was invalid
    void Svc_FpySequencer_SequencerStateMachine_action_report_invalidCmd(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_invalidSeq of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! warns that the sequence failed validation
    void Svc_FpySequencer_SequencerStateMachine_action_report_invalidSeq(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setGoalState_RUNNING of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the goal state to RUNNING
    void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_RUNNING(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setGoalState_VALID of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the goal state to VALID
    void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_VALID(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setGoalState_IDLE of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the goal state to IDLE
    void Svc_FpySequencer_SequencerStateMachine_action_setGoalState_IDLE(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendCmdResponse_OK of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! responds to the calling command with OK
    void Svc_FpySequencer_SequencerStateMachine_action_sendCmdResponse_OK(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendCmdResponse_EXECUTION_ERROR of state machine
    //! Svc_FpySequencer_SequencerStateMachine
    //!
    //! responds to the calling command with EXECUTION_ERROR
    void Svc_FpySequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action dispatchStatement of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! iterates to the next statement and dispatches it
    void Svc_FpySequencer_SequencerStateMachine_action_dispatchStatement(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action checkShouldWake of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! checks if sequencer should wake from sleep
    void Svc_FpySequencer_SequencerStateMachine_action_checkShouldWake(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resetRuntime of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! resets the sequence runtime
    void Svc_FpySequencer_SequencerStateMachine_action_resetRuntime(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setWakeupTime of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the wakeup time of the sequencer
    void Svc_FpySequencer_SequencerStateMachine_action_setWakeupTime(
        SmId smId,                                              //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Fw::Time& value                                   //!< The value
        ) override;

    //! Implementation for action checkStatementTimeout of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! checks if the current statement has timed out
    void Svc_FpySequencer_SequencerStateMachine_action_checkStatementTimeout(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqTimedOut of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that the sequence timed out
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqTimedOut(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;
    PROTECTED :

        // ----------------------------------------------------------------------
        // Functions to implement for internal state machine guards
        // ----------------------------------------------------------------------

        //! Implementation for guard goalStateIs_RUNNING of state machine Svc_FpySequencer_SequencerStateMachine
        //!
        //! return true if the goal state is RUNNING
        bool
        Svc_FpySequencer_SequencerStateMachine_guard_goalStateIs_RUNNING(
            SmId smId,                                             //!< The state machine id
            Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) const override;

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port checkTimers
    void checkTimers_handler(FwIndexType portNum,  //!< The port number
                             U32 context           //!< The call order
                             ) override;

    //! Handler for input port cmdResponseIn
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opCode,             //!< Command Op Code
                               U32 cmdSeq,                      //!< Command Sequence
                               const Fw::CmdResponse& response  //!< The command response argument
                               ) override;

    //! Handler for input port pingIn
    void pingIn_handler(FwIndexType portNum,  //!< The port number
                        U32 key               //!< Value to return to pinger
                        ) override;

    //! Handler for input port tlmWrite
    void tlmWrite_handler(FwIndexType portNum,  //!< The port number
                          U32 context           //!< The call order
                          ) override;

    //! Internal interface handler for directive_waitAbs
    void directive_waitAbs_internalInterfaceHandler(const Svc::FpySequencer_WaitAbsDirective& directive) override;

    //! Internal interface handler for directive_waitRel
    void directive_waitRel_internalInterfaceHandler(const Svc::FpySequencer_WaitRelDirective& directive) override;

    void parametersLoaded() override;
    void parameterUpdated(FwPrmIdType id) override;

  public:
    void allocateBuffer(FwEnumStoreType identifier, Fw::MemAllocator& allocator, FwSizeType bytes);

    void deallocateBuffer(Fw::MemAllocator& allocator);
    PRIVATE :

        static constexpr U32 CRC_INITIAL_VALUE = 0xFFFFFFFFU;

    // allocated at startup
    Fw::ExternalSerializeBuffer m_sequenceBuffer;
    // id of allocator that gave us m_sequenceBuffer
    FwEnumStoreType m_allocatorId;

    // assigned by the user via cmd
    Fw::String m_sequenceFilePath;
    // the sequence, loaded in memory
    Fpy::Sequence m_sequenceObj;
    // live running computation of CRC (updated as we read)
    U32 m_computedCRC;

    // whether or not the sequence we're about to run should return immediately or
    // block on completion
    FpySequencer_BlockState m_sequenceBlockState;
    // if we are to block on completion, save the opCode and cmdSeq we should
    // return
    FwOpcodeType m_savedOpCode;
    U32 m_savedCmdSeq;

    // the goal state is the state that we're trying to reach in the sequencer
    // if it's RUNNING, then we should promptly go to RUNNING once we validate the
    // sequence. if it's VALID, we should wait after VALIDATING
    FpySequencer_GoalState m_goalState;

    // the runtime state of the sequence. encapsulates all state
    // needed to run the sequence.
    // this is distinct from the state of the sequencer. the
    // sequencer and all its state is really just a shell to load
    // and execute this runtime.
    struct Runtime {
        // the index of the next statement to be executed
        U32 nextStatementIndex = 0;

        // the opcode of the statement that is currently executing
        FwOpcodeType currentStatementOpcode = Fpy::DirectiveId::INVALID;
        // the time we dispatched the statement that is currently executing
        Fw::Time currentStatementDispatchTime = Fw::Time();

        // the absolute time we should wait for until returning
        // a statement response
        Fw::Time wakeupTime = Fw::Time();
    };

    Runtime m_runtime;

    struct {
        // the number of statements successfully dispatched total
        U64 statementsDispatched = 0;

        // the number of statements that failed to execute
        U64 statementsFailed = 0;

        // the number of sequences successfully completed
        U64 sequencesSucceeded = 0;

        // the number of sequences that failed to validate or execute, or were cancelled
        U64 sequencesFailed = 0;
    } m_tlm;

    void updateComputedCRC(const U8* buffer,      //!< The buffer
                           FwSizeType bufferSize  //!< The buffer size
    );

    // loads the sequence in memory, and does header/crc/integrity checks.
    // return true if sequence is valid
    bool validate();

    // reads some bytes from the open file into the m_sequenceBuffer.
    // updates the CRC by default, but can be turned off if the contents
    // aren't included in CRC.
    // return true if successful
    bool readBytes(Os::File& file, FwSizeType readLen, bool updateCRC = true);

    // ----------------------------------------------------------------------
    // Runtime
    // ----------------------------------------------------------------------

    // dispatches the next statement
    void dispatchStatement();

    // dispatches a command out via port.
    // return true if successfully dispatched.
    bool dispatchCommand(const Fpy::Statement& stmt);

    // dispatches a sequencer directive to the right handler.
    // return true if successfully handled.
    bool dispatchDirective(const Fpy::Statement& stmt);
};

}  // namespace Svc

#endif
