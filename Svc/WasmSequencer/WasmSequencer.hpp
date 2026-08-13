// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Fw/DataStructures/FifoQueue.hpp"
#include "Fw/Types/LinearBufferTemplate.hpp"
#include "Fw/Types/StringTemplate.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/Seq/SeqArgsSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"
#include "config/FppConstantsAc.hpp"
#include "config/FwSizeTypeAliasAc.h"
#include "config/WasmSequencerConfig.hpp"

namespace Svc {

class WasmSequencer final : public WasmSequencerComponentBase {
    friend class WasmSequencerTester;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct WasmSequencer object
    WasmSequencer(const char* const compName  //!< The component name
    );

    //! Destroy WasmSequencer object
    ~WasmSequencer();

    //! WasmSequencer owns a raw spacewasm_t* handle and a slot in the
    //! process-wide global-allocator registry; copying would double-free both.
    WasmSequencer(const WasmSequencer&) = delete;
    WasmSequencer& operator=(const WasmSequencer&) = delete;

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
    //! Command response input
    void cmdResponseIn_handler(FwIndexType portNum,             //!< The port number
                               FwOpcodeType opCode,             //!< Command Op Code
                               U32 cmdSeq,                      //!< Command Sequence
                               const Fw::CmdResponse& response  //!< The command response argument
                               ) override;

    //! Handler implementation for writeTelemetry
    //!
    //! Port to periodically write telemetry channels (optional)
    void writeTelemetry_handler(FwIndexType portNum,  //!< The port number
                                U32 context           //!< The call order
                                ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for serialReply
    //!
    //! Reply port for [serialOut]. This reply is subject to timeout if configured.
    //! Sequences that send async serial messages will block until this reply is received
    //! on the corresponding port number
    void serialReply_handler(FwIndexType portNum,          //!< The port number
                             Fw::LinearBufferBase& buffer  //!< The serialization buffer
                             ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command RUN
    //!
    //! Run a Wasm module main function on it's own in the interpreter
    //! This command is short-hand for:
    //! 1. LOAD_NAME [fileName] ""
    //! 2. INVOKE "" "main"
    //! 3. CONTINUE
    //!
    //! If $block == Svc.BlockState.BLOCK this command will wait for completion.
    void RUN_cmdHandler(
        FwOpcodeType opCode,               //!< The opcode
        U32 cmdSeq,                        //!< The command sequence number
        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
        const Svc::BlockState& block,      //!< Block until sequence has finished running
                                           //!< Binary arguments to pass to the sequence
        const Svc::SeqArgs& seqArgs        //!< Optional arguments to execute the sequence with
                                           //!< Depending on the sequence being loaded these arguments may differ
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
    //! Invoke a main function from a loaded module
    void INVOKE_cmdHandler(FwOpcodeType opCode,             //!< The opcode
                           U32 cmdSeq,                      //!< The command sequence number
                           const Fw::CmdStringArg& module,  //!< Name of the module to invoke a function from
                           const Svc::BlockState& block,    //!< Block until sequence has finished running
                           const Svc::SeqArgs& seqArgs      //!< Arguments to invalid sequence entrypoint with
                           ) override;

    //! Handler implementation for command WAIT
    //!
    //! Wait for the interpreter to finish and return it's result as a CmdResponse
    void WAIT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;

    //! Handler implementation for command CANCEL
    //!
    //! Cancels a running or validated sequence. After running CANCEL, the sequencer should return to IDLE
    //! This completely clears the store.
    //! Cancelling during LOADING will trigger a fail response in the reader and
    //! return to IDLE mode.
    void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq            //!< The command sequence number
                           ) override;

    //! Handler implementation for command PAUSE
    //!
    //! Pauses the execution of the sequencer, just before it is about to dispatch the next directive,
    //! until unpaused by the CONTINUE command, or stepped by the STEP command. This command is only valid
    //! substates of the RUNNING state that are not RUNNING.PAUSED.
    void PAUSE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command CONTINUE
    //!
    //! Resume the sequence from a paused state
    void CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                             U32 cmdSeq            //!< The command sequence number
                             ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action loadCmd_OK of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! responds to any command waiting on load with OK
    void Svc_WasmSequencer_ControllerStateMachine_action_loadCmd_OK(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action loadCmd_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! responds to any command waiting on load with EXECUTION_ERROR
    void Svc_WasmSequencer_ControllerStateMachine_action_loadCmd_ERROR(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action runCmd_OK of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! responds to any command waiting on load with OK
    void Svc_WasmSequencer_ControllerStateMachine_action_runCmd_OK(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action runCmd_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! responds to any command waiting on load with EXECUTION_ERROR
    void Svc_WasmSequencer_ControllerStateMachine_action_runCmd_ERROR(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action load of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Load a pending module request
    void Svc_WasmSequencer_ControllerStateMachine_action_load(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleLoad& value                //!< The value
        ) override;

    //! Implementation for action reportModuleLoadFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event to denote failed module load
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleLoadFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_Status& value                    //!< The value
        ) override;

    //! Implementation for action invokeStart of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Invoke the start function on a loaded module
    void Svc_WasmSequencer_ControllerStateMachine_action_invokeStart(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleIdx& value                 //!< The value
        ) override;

    //! Implementation for action invokeMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Invoke the main function on a module given its index
    void Svc_WasmSequencer_ControllerStateMachine_action_invokeMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleIdx& value                 //!< The value
        ) override;

    //! Implementation for action reportModuleInvalidMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting that a given module has no [valid] main function
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleInvalidMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleIdx& value                 //!< The value
        ) override;

    //! Implementation for action reportModuleMainInvokeFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting why the invocation of a module main failed
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainInvokeFailed(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportModuleStartInvokeFailed of state machine
    //! Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting why the invocation of a module start failed
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartInvokeFailed(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resetStore of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Create a new store with N_MODULES (parameter) modules
    void Svc_WasmSequencer_ControllerStateMachine_action_resetStore(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action runEngine of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Send a signal to the engine state machine to begin running
    void Svc_WasmSequencer_ControllerStateMachine_action_runEngine(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportModuleSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module succeeded during execution. Increment telemetry counters
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleSucceeded(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportModuleFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module failed during execution. Increment telemetry counters
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleFailed(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action signalEntered of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! generic signal raised
    void Svc_WasmSequencer_EngineStateMachine_action_signalEntered(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action spin of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! spins the interpreter loop, executing up to a bounded number of instructions
    void Svc_WasmSequencer_EngineStateMachine_action_spin(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reset of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! resets the engine's state (clears operand stack, pc, fp, sp)
    void Svc_WasmSequencer_EngineStateMachine_action_reset(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_UNKNOWN of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_UNKNOWN(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_INTERPRETER_FINISHED of state machine
    //! Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_INTERPRETER_FINISHED(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_INTERPRETER_TRAP of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_INTERPRETER_TRAP(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_REPLY_TIMEOUT of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_REPLY_TIMEOUT(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_HOST_FAILURE of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_HOST_FAILURE(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_TIMER_INCOMPARABLE of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_TIMER_INCOMPARABLE(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_UNEXPECTED_REPLY of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_UNEXPECTED_REPLY(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitReason_CANCEL of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitReason_CANCEL(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setExitCode of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setExitCode(
        SmId smId,                                            //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal,  //!< The signal
        I32 value                                             //!< The value
        ) override;

    //! Implementation for action setTrapReason of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setTrapReason(
        SmId smId,                                            //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_TrapReason& value            //!< The value
        ) override;

    //! Implementation for action updateHostFailureReason of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_updateHostFailureReason(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action finish of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! Send a signal back to the controller state machine that we have finished executing
    //! The response codes are stored in m_exitReason, m_exitCode, m_tlmLastTrapReason
    void Svc_WasmSequencer_EngineStateMachine_action_finish(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportPaused of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! reports that execution was paused at a breakpoint
    void Svc_WasmSequencer_EngineStateMachine_action_reportPaused(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action pendPause of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! sets the pause flag to true
    void Svc_WasmSequencer_EngineStateMachine_action_pendPause(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearPause of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! sets the pause flag to false
    void Svc_WasmSequencer_EngineStateMachine_action_clearPause(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action dispatchPendingHostFunction of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! dispatch a host function port call
    void Svc_WasmSequencer_EngineStateMachine_action_dispatchPendingHostFunction(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearPendingHostFunction of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! clears the pending host function port call
    void Svc_WasmSequencer_EngineStateMachine_action_clearPendingHostFunction(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resume of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! spacewasm_engine_resume
    void Svc_WasmSequencer_EngineStateMachine_action_resume(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resumeI32 of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! spacewasm_engine_resume_some(I32(value))
    void Svc_WasmSequencer_EngineStateMachine_action_resumeI32(
        SmId smId,                                            //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal,  //!< The signal
        I32 value                                             //!< The value
        ) override;

    //! Implementation for action checkSleepTimers of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! A periodic check on the pending timer to see if we can wake up
    void Svc_WasmSequencer_EngineStateMachine_action_checkSleepTimers(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action checkTimeout of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! A periodic check on any host function to guard against timeouts
    void Svc_WasmSequencer_EngineStateMachine_action_checkTimeout(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
        ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard moduleHasStart of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if this module has a start function
    bool Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasStart(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleIdx& value                 //!< The value
    ) const override;

    //! Implementation for guard moduleHasValidMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if this module has a valid main function ([] -> i32)
    bool Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasValidMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleIdx& value                 //!< The value
    ) const override;

    //! Implementation for guard invokeSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if invokeStatus == SPACEWASM_OK. This flag is set as a result of invokeStart/invokeMain
    bool Svc_WasmSequencer_ControllerStateMachine_guard_invokeSucceeded(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard interpreterSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    bool Svc_WasmSequencer_ControllerStateMachine_guard_interpreterSucceeded(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingPause of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! return true if execution should pause before spinning the next statement
    bool Svc_WasmSequencer_EngineStateMachine_guard_pendingPause(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingHostFunction of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! a host function is waiting to be processed
    bool Svc_WasmSequencer_EngineStateMachine_guard_pendingHostFunction(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingHostFunctionIsSleep of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! the pending host function is a sleep (therefore we need to check the sleep timers)
    bool Svc_WasmSequencer_EngineStateMachine_guard_pendingHostFunctionIsSleep(
        SmId smId,                                           //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal  //!< The signal
    ) const override;

  private:
    /// The global allocator callbacks
    U8* globalAlloc(U32 size, U32 align);
    void globalDealloc(const U8* ptr);

    /// The Wasm guest allocator callbacks
    U8* guestAlloc(U32 size, U32 align);
    void guestDealloc(const U8* ptr, U32 size);

    //! Fill `m_readBuf` with the next chunk of the module file being loaded.
    spacewasm_read_result_t readModuleChunk(const U8** outBuf, size_t* outLen);

    //! Create a fresh interpreter store with the given module capacity,
    //! destroying any existing store first.
    Fw::Success createStore();

    //! Destroy the current interpreter store, if any, releasing its memory.
    void destroyStore();

    //! Take control of the spacewasm global allocator on this WasmSequence
    void takeAllocatorLock();

    //! Release control of the spacewasm global allocator on this WasmSequence
    void releaseAllocatorLock();

    //! Map a spacewasm_trap_t onto the TrapReason event enum.
    static Svc::WasmSequencer_TrapReason::T mapTrapReason(spacewasm_trap_t trap);

    //! Record the SeqName telemetry for a load. Uses moduleName when non-empty
    //! (LOAD_NAME); otherwise derives it from the file's basename with any ".wasm"
    //! suffix stripped (RUN / LOAD).
    void setSequenceName(const Fw::StringBase& filePath, const Fw::StringBase& moduleName);

    //! Static pool backing the process-wide spacewasm global page allocator.
    alignas(16) U8 m_memory_pool[Svc::WasmSequencerConfig::DYNAMIC_MEMORY_SIZE];

    //! Which pages of `m_memory_pool` are currently handed out (bit i == page i).
    U32 m_page_used_mask;

    //! Static pool backing the per-load guest linear-memory allocator; a simple
    //! bump allocator (guest modules are compiled with memory.grow disabled).
    alignas(16) U8 m_guest_pool[Svc::WasmSequencerConfig::GUEST_MEMORY_SIZE];

    //! Current bump offset into `m_guest_pool`.
    FwSizeType m_guest_offset;

    //! Currently stored sequence arguments
    Svc::SeqArgs m_args;

    //! Buffer handed to the streaming loader, filled from `m_loadFile`.
    U8 m_readBuf[256];

    //! Opaque handle to the spacewasm engine, or null.
    spacewasm_t* m_wasm;

    //! Pending command waiting for a response
    struct PendingCmd {
        FwOpcodeType opCode;
        U32 cmdSeq;

        PendingCmd() = default;
        PendingCmd(FwOpcodeType opCode_, U32 cmdSeq_) : opCode(opCode_), cmdSeq(cmdSeq_) {}
    };

    //! Commands pending interpreter finish
    Fw::FifoQueue<PendingCmd, 8> m_pendingFinishCmds;

    PendingCmd m_pendingLoadCmd;
    bool m_hasPendingLoadCmd;

    //! Pending timer from sleep host function
    Fw::Time m_pendingTimer;
    bool m_hasPendingTimer;

    //! Wall-clock time at which the current blocking async host function
    //! (COMMAND / ASYNC_PORT) began awaiting its reply. Used to enforce
    //! STATEMENT_TIMEOUT_SECS. Only meaningful while awaiting one of those.
    Fw::Time m_statementStart;
    bool m_hasStatementStart;

    //! The last module index that was invoked
    WasmSequencer_ModuleIdx m_invokedModule;

    //! Flag indicating a function invocation failed
    spacewasm_status_t m_invokeStatus;

    //! Flag indicating interpreter is waiting to be paused
    bool m_pendingPause;

    //! Currently loading file handle
    Os::File* m_loadFile;

    //! Sequences successfully completed
    U64 m_tlmSequencesSucceeded;

    //! Sequences that failed to validate or execute
    U64 m_tlmSequencesFailed;

    //! Sequences that were cancelled
    U64 m_tlmSequencesCancelled;

    //! Commands dispatched total
    U64 m_tlmCommandsDispatched;

    //! Number of commands that failed
    U64 m_tlmCommandsFailed;

    //! Monotonic counter of sequences started (bumped whenever the interpreter
    //! begins spinning a freshly-invoked program). The low 16 bits form the
    //! high half of the command context (cmdUid) so we can detect a command
    //! response that arrives late, after its originating sequence has ended.
    U32 m_sequencesStarted;

    //! Compose the command context (cmdUid) sent to the command dispatcher from
    //! the current sequence counter and the m_tlmCommandsDispatched counter. The
    //! low 16 bits of the latter form the low half of the cmdUid, letting us
    //! detect a response for a different instance of the same opcode.
    U32 makeCmdUid() const;

    //! Reason the current program exited.
    //! By default this is INTERPRETER but can be overriden from host functions
    WasmSequencer_ExitReason m_exitReason;

    //! Currently stored exit code for non-INTERPRETER exits
    I32 m_exitCode;

    //! Host function that caused the failure
    WasmSequencer_HostFunction m_failedHostFunction;

    //! Reason last sequence trapped
    WasmSequencer_TrapReason m_tlmLastTrapReason;

    //! Currently running sequence name
    Fw::StringTemplate<FileNameStringSize> m_tlmSequenceName;

    //! Buffer to hold the serial output port invocation invoked by the guest
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> m_serialPortBuffer;

    struct PendingHostFunction {
        PendingHostFunction() = default;
        bool isPending() const { return kind != WasmSequencer_HostFunction::NONE; }
        void clear() { kind = WasmSequencer_HostFunction::NONE; }

        WasmSequencer_HostFunction kind{WasmSequencer_HostFunction::NONE};

        // Handle that holds the Wasm guest memory pointer
        spacewasm_caller_t* caller{nullptr};

        // Port/channel/param id, or (for EVENT) the raw guest-requested
        // severity id as passed by the guest (may be out of range).
        U64 id{0};

        // The first pointer/length in one of the host commands
        U32 ptr1{0};
        U32 len1{0};

        // The second pointer/length in one of the host commands
        U32 ptr2{0};
        U32 len2{0};

        // The absolute/relative time for [ar]sleep
        U64 time_us{0};
    };

    PendingHostFunction m_pendingHostFunction;

    //! Helper function for checking the signature of a modules main function
    spacewasm_status_t validateModuleMain(WasmSequencer_ModuleIdx moduleIdx) const;

    void hostFprimeV1(spacewasm_host_t*);

    /// FPrime Wasm Interface Host Functions
    spacewasm_hostcall_result_t wasmExit(struct spacewasm_caller_t* caller,
                                         const struct spacewasm_value_t* params,
                                         size_t n_params,
                                         struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmPanic(struct spacewasm_caller_t* caller,
                                          const struct spacewasm_value_t* params,
                                          size_t n_params,
                                          struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmArgs(struct spacewasm_caller_t* caller,
                                         const struct spacewasm_value_t* params,
                                         size_t n_params,
                                         struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmTime(struct spacewasm_caller_t* caller,
                                         const struct spacewasm_value_t* params,
                                         size_t n_params,
                                         struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmReadTelemetry(struct spacewasm_caller_t* caller,
                                                  const struct spacewasm_value_t* params,
                                                  size_t n_params,
                                                  struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmReadParameter(struct spacewasm_caller_t* caller,
                                                  const struct spacewasm_value_t* params,
                                                  size_t n_params,
                                                  struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmCommand(struct spacewasm_caller_t* caller,
                                            const struct spacewasm_value_t* params,
                                            size_t n_params,
                                            struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmEvent(struct spacewasm_caller_t* caller,
                                          const struct spacewasm_value_t* params,
                                          size_t n_params,
                                          struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmRsleep(struct spacewasm_caller_t* caller,
                                           const struct spacewasm_value_t* params,
                                           size_t n_params,
                                           struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmAsleep(struct spacewasm_caller_t* caller,
                                           const struct spacewasm_value_t* params,
                                           size_t n_params,
                                           struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmSerialSync(struct spacewasm_caller_t* caller,
                                               const struct spacewasm_value_t* params,
                                               size_t n_params,
                                               struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmSerialAsync(struct spacewasm_caller_t* caller,
                                                const struct spacewasm_value_t* params,
                                                size_t n_params,
                                                struct spacewasm_value_t* out_result);

    // A global static lock. This is needed to allow the global allocator in spacewasm
    // to not require to pass context to fine grained context to allocations.
    // Read more about this in the SDD.
    static Os::Mutex* getGlobalAllocatorLock();
};

}  // namespace Svc

#endif
