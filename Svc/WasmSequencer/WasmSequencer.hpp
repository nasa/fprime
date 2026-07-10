// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"

namespace Svc {

class WasmSequencer final : public WasmSequencerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct WasmSequencer object
    WasmSequencer(const char* const compName  //!< The component name
    );

    //! Destroy WasmSequencer object
    ~WasmSequencer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for checkTimers
    //!
    //! Port to periodically drive sleep-wake and statement-timeout checks
    void checkTimers_handler(FwIndexType portNum,  //!< The port number
                             U32 context           //!< The call order
                             ) override;

    //! Handler implementation for cmdResponseIn
    //!
    //! Response
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opCode,             //!< Command Op Code
                               U32 cmdSeq,                      //!< Command Sequence
                               const Fw::CmdResponse& response  //!< The command response argument
                               ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command RUN
    //!
    //! Run a Wasm module main function on it's own in the interpreter
    //! This command is short-hand for:
    //! 1. CLEAR_STORE
    //! 2. LOAD_NAME [fileName] ""
    //! 3. INVOKE "" "main"
    //! 4. CONTINUE
    //!
    //! If $block == Svc.BlockState.BLOCK this command will wait for complemention.
    void RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                        U32 cmdSeq,                        //!< The command sequence number
                        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                        Svc::BlockState block              //!< Return command status when complete or not
                        ) override;

    //! Handler implementation for command WAIT
    //!
    //! Wait for the interpreter to finish and return it's result as a CmdResponse
    void WAIT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;

    //! Handler implementation for command LOAD
    //!
    //! Loads and validates a WebAssembly module into the store.
    //! This command loads the module with a empty name meaning only a single module may be loaded.
    //! To allow multiple modules, use the `LOAD_NAME` command instead.
    void LOAD_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                         U32 cmdSeq,                       //!< The command sequence number
                         const Fw::CmdStringArg& fileName  //!< The name of the sequence file
                         ) override;

    //! Handler implementation for command LOAD_NAME
    //!
    //! Load and validate a WebAssembly module into the store. This module is given a name so that
    //! it's exports can be referenced by other modules.
    void LOAD_NAME_cmdHandler(
        FwOpcodeType opCode,               //!< The opcode
        U32 cmdSeq,                        //!< The command sequence number
        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
        const Fw::CmdStringArg& name  //!< WebAssembly module name, must not conflict with previously loaded modules
        ) override;

    //! Handler implementation for command INVOKE
    //!
    //! Invoke a function from a loaded module
    void INVOKE_cmdHandler(FwOpcodeType opCode,                  //!< The opcode
                           U32 cmdSeq,                           //!< The command sequence number
                           const Fw::CmdStringArg& module,       //!< Name of the module to invoke a function from
                           const Fw::CmdStringArg& functionName  //!< Name of the function to be invoked
                           ) override;

    //! Handler implementation for command CLEAR_STORE
    //!
    //! Clear the WebAssembly store of all modules, tables, and memories to start fresh.
    void CLEAR_STORE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq            //!< The command sequence number
                                ) override;

    //! Handler implementation for command REINITIALIZE_STORE
    //!
    //! Reinitialize the Wasm store. This clears the current store and reallocates the internal memory
    //! for holding modules. A maximum of 256 modules is supported.
    void REINITIALIZE_STORE_cmdHandler(
        FwOpcodeType opCode,  //!< The opcode
        U32 cmdSeq,           //!< The command sequence number
        U16 module_count      //!< Maximum number of modules that can be loaded into the store
        ) override;

    //! Handler implementation for command CANCEL
    //!
    //! Cancels a running or validated sequence. After running CANCEL, the sequencer
    //! should return to IDLE
    void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq            //!< The command sequence number
                           ) override;

    //! Handler implementation for command BREAK
    //!
    //! Pauses the execution of the sequencer, just before it is about to dispatch the next directive,
    //! until unpaused by the CONTINUE command, or stepped by the STEP command. This command is only valid
    //! substates of the RUNNING state that are not RUNNING.PAUSED.
    void BREAK_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command TRACE
    //!
    //! Dump a stack trace as an event
    void TRACE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command CONTINUE
    void CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                             U32 cmdSeq            //!< The command sequence number
                             ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action signalEntered of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! simply raises the "entered" signal
    void Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action invoke of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! sets up a function invocation on the interpreter state from the pending FunctionInvoke member var
    void Svc_WasmSequencer_SequencerStateMachine_action_invoke(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setLoad of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! stores the module load request for the LOADING state
    void Svc_WasmSequencer_SequencerStateMachine_action_setLoad(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleLoad& value               //!< The value
        ) override;

    //! Implementation for action load of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! streams, decodes and validates the module into the store
    void Svc_WasmSequencer_SequencerStateMachine_action_load(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearStore of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! clears the interpreter store of all modules, tables and memories
    void Svc_WasmSequencer_SequencerStateMachine_action_clearStore(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportValidationFailure of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports the module validation failure as an event
    void Svc_WasmSequencer_SequencerStateMachine_action_reportValidationFailure(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendCmdResponse_VALIDATION_ERROR of state machine
    //! Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to the calling command with VALIDATION_ERROR
    void Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_VALIDATION_ERROR(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action pushModuleToStore of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! pushes the freshly loaded/validated module into the store
    void Svc_WasmSequencer_SequencerStateMachine_action_pushModuleToStore(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action spin of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! spins the interpreter loop, executing up to a bounded number of instructions
    void Svc_WasmSequencer_SequencerStateMachine_action_spin(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqSucceeded of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that the interpreter ran to completion
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqSucceeded(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqCancelled of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that the sequence was cancelled
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqCancelled(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqFailed of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that the sequence failed to execute successfully
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqFailed(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqTrap of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that the interpreter trapped, with the trap reason
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqTrap(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqReadError of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that the interpreter failed to read an instruction from memory
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqReadError(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action report_seqBroken of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that execution was paused at a breakpoint
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqBroken(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendCmdResponse_OK of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to the calling command with OK
    void Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_OK(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendCmdResponse_EXECUTION_ERROR of state machine
    //! Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to the calling command with EXECUTION_ERROR
    void Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action set_sleepTime of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! stores the requested wake time for the SLEEPING state
    void Svc_WasmSequencer_SequencerStateMachine_action_set_sleepTime(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action checkStatementTimeout of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! checks if the current statement has timed out
    void Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action checkShouldWake of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! checks if the sequencer should wake from sleep
    void Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setBreakBeforeNextLine of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! sets the "break before next line" flag to true
    void Svc_WasmSequencer_SequencerStateMachine_action_setBreakBeforeNextLine(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearBreakBeforeNextLine of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! sets the "break before next line" flag to false
    void Svc_WasmSequencer_SequencerStateMachine_action_clearBreakBeforeNextLine(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard pendingInvoke of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if there is a function invocation pending to be set up on entry to IDLE.
    //! the pending FunctionInvoke is held in a component member var
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingInvoke(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard hasStartFunction of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if the freshly loaded module declared a Wasm start function
    bool Svc_WasmSequencer_SequencerStateMachine_guard_hasStartFunction(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard shouldBreak of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if execution should break before spinning the next statement
    bool Svc_WasmSequencer_SequencerStateMachine_guard_shouldBreak(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;
};

}  // namespace Svc

#endif
