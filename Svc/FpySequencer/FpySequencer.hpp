// ======================================================================
// \title  FpySequencer.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer component implementation class
// ======================================================================

#ifndef FpySequencer_HPP
#define FpySequencer_HPP

#include "Fw/Types/MemAllocator.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Fw/Types/WaitEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/FpySequencer/DirectiveIdEnumAc.hpp"
#include "Svc/FpySequencer/FooterSerializableAc.hpp"
#include "Svc/FpySequencer/FpySequencerComponentAc.hpp"
#include "Svc/FpySequencer/FpySequencer_GoalStateEnumAc.hpp"
#include "Svc/FpySequencer/HeaderSerializableAc.hpp"
#include "Svc/FpySequencer/SequenceSerializableAc.hpp"
#include "Svc/FpySequencer/StatementSerializableAc.hpp"
#include "config/FppConstantsAc.hpp"

static_assert(Svc::Fpy::MAX_SEQUENCE_ARG_COUNT <= std::numeric_limits<U8>::max(),
              "Sequence arg count must be below U8 max");
static_assert(Svc::Fpy::NUM_REGISTERS <= std::numeric_limits<U8>::max(),
              "Register count must be below U8 max");
static_assert(Svc::Fpy::MAX_SEQUENCE_STATEMENT_COUNT <= std::numeric_limits<U16>::max(),
              "Sequence statement count must be below U16 max");
static_assert(Svc::Fpy::MAX_SERIALIZABLE_REGISTER_SIZE <= std::numeric_limits<FwSizeType>::max(),
              "Local variable buffer size must be below FwSizeType max");
static_assert(Svc::Fpy::MAX_SERIALIZABLE_REGISTER_SIZE >= FW_TLM_BUFFER_MAX_SIZE,
              "Local variable buffer size must be greater than FW_TLM_BUFFER_MAX_SIZE");
static_assert(Svc::Fpy::MAX_SERIALIZABLE_REGISTER_SIZE >= FW_PARAM_BUFFER_MAX_SIZE,
              "Local variable buffer size must be greater than FW_PARAM_BUFFER_MAX_SIZE");

namespace Svc {

using Signal = FpySequencer_SequencerStateMachineStateMachineBase::Signal;
using State = FpySequencer_SequencerStateMachineStateMachineBase::State;
using DirectiveError = FpySequencer_DirectiveErrorCode;

class FpySequencer : public FpySequencerComponentBase {

  friend class FpySequencerTester;

  public:
    union DirectiveUnion {
        FpySequencer_WaitRelDirective waitRel;
        FpySequencer_WaitAbsDirective waitAbs;
        FpySequencer_SetSerRegDirective setSerReg;
        FpySequencer_GotoDirective gotoDirective;
        FpySequencer_IfDirective ifDirective;
        FpySequencer_NoOpDirective noOp;
        FpySequencer_GetTlmDirective getTlm;
        FpySequencer_GetPrmDirective getPrm;
        FpySequencer_CmdDirective cmd;
        FpySequencer_DeserSerRegDirective deserSerReg;
        FpySequencer_SetRegDirective setReg;
        FpySequencer_BinaryCmpDirective binaryCmp;
        FpySequencer_NotDirective notDirective;
        FpySequencer_ExitDirective exit;

        DirectiveUnion() {}
        ~DirectiveUnion() {}
    };

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

    private:

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

    //! Handler for command DEBUG_SET_BREAKPOINT
    //!
    //! Sets the debugging breakpoint which will pause the execution of the sequencer
    //! until unpaused by the DEBUG_CONTINUE command. Will pause just before dispatching
    //! the specified statement. This command is valid in all states. Debug settings are
    //! cleared after a sequence ends execution.
    void DEBUG_SET_BREAKPOINT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                         U32 cmdSeq,           //!< The command sequence number
                                         U32 stmtIdx,          //!< The statement index to pause execution before.
                                         bool breakOnce        //!< Whether or not to break only once at this breakpoint
                                         ) override;

    //! Handler for command DEBUG_BREAK
    //!
    //! Pauses the execution of the sequencer once, just before it is about to dispatch the next statement,
    //! until unpaused by the DEBUG_CONTINUE command. This command is only valid in the RUNNING state.
    //! Debug settings are cleared after a sequence ends execution.
    void DEBUG_BREAK_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq,           //!< The command sequence number
                                bool breakOnce        //!< Whether or not to break only once at this breakpoint
                                ) override;

    //! Handler for command DEBUG_CONTINUE
    //!
    //! Continues the execution of the sequence after it has been paused by a debug break. This command
    //! is only valid in the RUNNING.DEBUG_BROKEN state.
    void DEBUG_CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                   U32 cmdSeq            //!< The command sequence number
                                   ) override;

    //! Handler for command DEBUG_CLEAR_BREAKPOINT
    //!
    //! Clears the debugging breakpoint, but does not continue executing the sequence. This command
    //! is valid in all states. This happens automatically when a sequence ends execution.
    void DEBUG_CLEAR_BREAKPOINT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
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

    //! Implementation for action clearSequenceFile of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! clears all variables related to the loading/validating of the sequence file
    void Svc_FpySequencer_SequencerStateMachine_action_clearSequenceFile(
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

    //! Implementation for action checkStatementTimeout of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! checks if the current statement has timed out
    void Svc_FpySequencer_SequencerStateMachine_action_checkStatementTimeout(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action incrementSequenceCounter of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! increments the m_sequencesStarted counter
    void Svc_FpySequencer_SequencerStateMachine_action_incrementSequenceCounter(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearDebugBreakpoint of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! clears the debug breakpoint, allowing execution of the sequence to continue
    void Svc_FpySequencer_SequencerStateMachine_action_clearDebugBreakpoint(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_debugBroken of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a debug breakpoint was hit
    void Svc_FpySequencer_SequencerStateMachine_action_report_debugBroken(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setDebugBreakpoint of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! sets the debug breakpoint to the provided args
    void Svc_FpySequencer_SequencerStateMachine_action_setDebugBreakpoint(
        SmId smId,                                              //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::FpySequencer_DebugBreakpointArgs& value      //!< The value
        ) override;

    //! Implementation for action report_seqFailed of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! called when a sequence failed to execute successfully
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqFailed(
        SmId smId, //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal //!< The signal
    ) override;

    //! Implementation for action report_seqStarted of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! reports that a sequence was started
    void Svc_FpySequencer_SequencerStateMachine_action_report_seqStarted(
        SmId smId, //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal //!< The signal
    ) override;

    protected:

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

    //! Implementation for guard shouldDebugBreak of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! return true if should debug break at this point in execution, before dispatching
    //! next stmt
    bool Svc_FpySequencer_SequencerStateMachine_guard_shouldDebugBreak(
        SmId smId,                                             //!< The state machine id
        Svc_FpySequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard debugBreakOnce of state machine Svc_FpySequencer_SequencerStateMachine
    //!
    //! return true if this debug breakpoint should only happen once
    bool Svc_FpySequencer_SequencerStateMachine_guard_debugBreakOnce(
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

    //! Handler for input port seqRunIn
    void seqRunIn_handler(FwIndexType portNum,
                          const Fw::StringBase& filename
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
    void directive_waitAbs_internalInterfaceHandler(const FpySequencer_WaitAbsDirective& directive) override;

    //! Internal interface handler for directive_waitRel
    void directive_waitRel_internalInterfaceHandler(const FpySequencer_WaitRelDirective& directive) override;

    //! Internal interface handler for directive_setSerReg
    void directive_setSerReg_internalInterfaceHandler(const FpySequencer_SetSerRegDirective& directive) override;

    //! Internal interface handler for directive_goto
    void directive_goto_internalInterfaceHandler(const Svc::FpySequencer_GotoDirective& directive) override;

    //! Internal interface handler for directive_if
    void directive_if_internalInterfaceHandler(const Svc::FpySequencer_IfDirective& directive) override;

    //! Internal interface handler for directive_noOp
    void directive_noOp_internalInterfaceHandler(const Svc::FpySequencer_NoOpDirective& directive) override;

    //! Internal interface handler for directive_getTlm
    void directive_getTlm_internalInterfaceHandler(const Svc::FpySequencer_GetTlmDirective& directive) override;

    //! Internal interface handler for directive_getPrm
    void directive_getPrm_internalInterfaceHandler(const Svc::FpySequencer_GetPrmDirective& directive) override;

    //! Internal interface handler for directive_cmd
    void directive_cmd_internalInterfaceHandler(const Svc::FpySequencer_CmdDirective& directive) override;

    //! Internal interface handler for directive_deserSerReg
    void directive_deserSerReg_internalInterfaceHandler(const Svc::FpySequencer_DeserSerRegDirective& directive) override;

    //! Internal interface handler for directive_setReg
    void directive_setReg_internalInterfaceHandler(const Svc::FpySequencer_SetRegDirective& directive) override;

    //! Internal interface handler for directive_binaryCmp
    void directive_binaryCmp_internalInterfaceHandler(const Svc::FpySequencer_BinaryCmpDirective& directive) override;

    //! Internal interface handler for directive_not
    void directive_not_internalInterfaceHandler(const Svc::FpySequencer_NotDirective& directive) override;

    //! Internal interface handler for directive_exit
    void directive_exit_internalInterfaceHandler(const Svc::FpySequencer_ExitDirective& directive) override;

    void parametersLoaded() override;
    void parameterUpdated(FwPrmIdType id) override;

  public:
    void allocateBuffer(FwEnumStoreType identifier, Fw::MemAllocator& allocator, FwSizeType bytes);

    void deallocateBuffer(Fw::MemAllocator& allocator);
    private :

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

    // the total number of sequences this sequencer has started since construction
    U64 m_sequencesStarted;
    // the total number of statements this sequencer has dispatched, successfully or
    // otherwise, since construction
    U64 m_statementsDispatched;

    // the runtime state of the sequence. encapsulates all state
    // needed to run the sequence.
    // this is distinct from the state of the sequencer. the
    // sequencer and all its state is really just a shell to load
    // and execute this runtime.
    struct Runtime {
        // the index of the next statement to be executed
        U32 nextStatementIndex = 0;

        // the opcode of the statement that is currently executing
        U8 currentStatementOpcode = Fpy::DirectiveId::INVALID;
        // the opcode of the command that we are currently awaiting, or 0 if we are executing a directive
        FwOpcodeType currentCmdOpcode = 0;
        // the time we dispatched the statement that is currently executing
        Fw::Time currentStatementDispatchTime = Fw::Time();

        // the absolute time we should wait for until returning
        // a statement response
        Fw::Time wakeupTime = Fw::Time();

        // all the serializable registers in the sequence
        struct SerializableReg {
            // the value buffer of the serReg
            U8 value[Fpy::MAX_SERIALIZABLE_REGISTER_SIZE] = {};
            // the size of the data in the serReg buf
            FwSizeType valueSize = 0;
        } serRegs[Fpy::NUM_SERIALIZABLE_REGISTERS] = {};

        // all the regs in the sequence. regs are 8 byte
        // values of unspecified type
        I64 regs[Fpy::NUM_REGISTERS] = {0};

    } m_runtime;

    // the state of the debugger. debugger is separate from runtime
    // because it can be set up before running the sequence.
    struct Debug {
        // whether or not to break at the debug breakpoint index
        bool breakOnBreakpoint = false;
        // whether or not to remove the breakpoint after breaking on it
        bool breakOnlyOnceOnBreakpoint = false;
        // the statement index at which to break, before dispatching
        U32 breakpointIndex = 0;
    } m_debug;

    struct Telemetry {
        // the number of statements that failed to execute
        U64 statementsFailed = 0;

        // the number of sequences successfully completed
        U64 sequencesSucceeded = 0;

        // the number of sequences that failed to validate or execute
        U64 sequencesFailed = 0;

        // the number of sequences that have been cancelled
        U64 sequencesCancelled = 0;

        // the error code of the last directive that ran
        DirectiveError lastDirectiveError = DirectiveError::NO_ERROR;
    } m_tlm;

    // ----------------------------------------------------------------------
    // Validation state
    // ----------------------------------------------------------------------

    static void updateCrc(U32& crc,              //!< The CRC to update
                          const U8* buffer,      //!< The buffer
                          FwSizeType bufferSize  //!< The buffer size
    );

    // loads the sequence in memory, and does header/crc/integrity checks.
    // return success if sequence is valid
    Fw::Success validate();
    // reads and validates the header from the m_sequenceBuffer
    // return success if header is valid
    Fw::Success readHeader();
    // reads and validates the body from the m_sequenceBuffer
    // return success if body is valid
    Fw::Success readBody();
    // reads and validates the footer from the m_sequenceBuffer
    // return success if footer is valid
    Fw::Success readFooter();

    // reads some bytes from the open file into the m_sequenceBuffer.
    // updates the CRC by default, but can be turned off if the contents
    // aren't included in CRC.
    // return success if successful
    Fw::Success readBytes(Os::File& file, FwSizeType readLen, bool updateCrc = true);

    // ----------------------------------------------------------------------
    // Run state
    // ----------------------------------------------------------------------

    // dispatches the next statement
    Signal dispatchStatement();

    // deserializes a directive from bytes into the Fpy type
    // returns success if able to deserialize, and returns the Fpy type object
    // as a reference, in a union of all the possible directive type objects
    Fw::Success deserializeDirective(const Fpy::Statement& stmt, DirectiveUnion& deserializedDirective);

    // dispatches a deserialized sequencer directive to the right handler.
    void dispatchDirective(const DirectiveUnion& directive, const Fpy::DirectiveId& id);

    // checks whether the currently executing statement timed out
    Signal checkStatementTimeout();

    // checks whether the sequencer should wake from sleeping
    Signal checkShouldWake();

    // return true if state is a substate of RUNNING
    bool isRunningState(State state);

    // return a struct containing debug telemetry, or defaults if not in debug break
    FpySequencer_DebugTelemetry getDebugTelemetry();

    // ----------------------------------------------------------------------
    // Directives
    // ----------------------------------------------------------------------

    // sends a signal based on a signal id
    void sendSignal(Signal signal);

    // helper function to get a reference to a register from an index
    // saves a bunch of typing
    I64& reg(U8 idx);

    // we split these functions up into the internalInterfaceInvoke and these custom member funcs
    // so that we can unit test them easier
    Signal waitRel_directiveHandler(const FpySequencer_WaitRelDirective& directive, DirectiveError& error);
    Signal waitAbs_directiveHandler(const FpySequencer_WaitAbsDirective& directive, DirectiveError& error);
    Signal setSerReg_directiveHandler(const FpySequencer_SetSerRegDirective& directive, DirectiveError& error);
    Signal goto_directiveHandler(const FpySequencer_GotoDirective& directive, DirectiveError& error);
    Signal if_directiveHandler(const FpySequencer_IfDirective& directive, DirectiveError& error);
    Signal noOp_directiveHandler(const FpySequencer_NoOpDirective& directive, DirectiveError& error);
    Signal getTlm_directiveHandler(const FpySequencer_GetTlmDirective& directive, DirectiveError& error);
    Signal getPrm_directiveHandler(const FpySequencer_GetPrmDirective& directive, DirectiveError& error);
    Signal cmd_directiveHandler(const FpySequencer_CmdDirective& directive, DirectiveError& error);
    Signal deserSerReg_directiveHandler(const FpySequencer_DeserSerRegDirective& directive, DirectiveError& error);
    Signal setReg_directiveHandler(const FpySequencer_SetRegDirective& directive, DirectiveError& error);

    Signal binaryCmp_directiveHandler(const FpySequencer_BinaryCmpDirective& directive, DirectiveError& error);
    I64 binaryCmp_or(I64 lhs, I64 rhs);
    I64 binaryCmp_and(I64 lhs, I64 rhs);
    I64 binaryCmp_ieq(I64 lhs, I64 rhs);
    I64 binaryCmp_ine(I64 lhs, I64 rhs);
    I64 binaryCmp_ult(I64 lhs, I64 rhs);
    I64 binaryCmp_ule(I64 lhs, I64 rhs);
    I64 binaryCmp_ugt(I64 lhs, I64 rhs);
    I64 binaryCmp_uge(I64 lhs, I64 rhs);
    I64 binaryCmp_slt(I64 lhs, I64 rhs);
    I64 binaryCmp_sle(I64 lhs, I64 rhs);
    I64 binaryCmp_sgt(I64 lhs, I64 rhs);
    I64 binaryCmp_sge(I64 lhs, I64 rhs);
    I64 binaryCmp_feq(I64 lhs, I64 rhs);
    I64 binaryCmp_fne(I64 lhs, I64 rhs);
    I64 binaryCmp_flt(I64 lhs, I64 rhs);
    I64 binaryCmp_fle(I64 lhs, I64 rhs);
    I64 binaryCmp_fgt(I64 lhs, I64 rhs);
    I64 binaryCmp_fge(I64 lhs, I64 rhs);

    Signal not_directiveHandler(const FpySequencer_NotDirective& directive, DirectiveError& error);
    Signal exit_directiveHandler(const FpySequencer_ExitDirective& directive, DirectiveError& error);
};

}  // namespace Svc

#endif
