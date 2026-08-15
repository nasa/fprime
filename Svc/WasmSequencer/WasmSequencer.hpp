// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/DataStructures/FifoQueue.hpp"
#include "Fw/Types/FileNameString.hpp"
#include "Fw/Types/LinearBufferTemplate.hpp"
#include "Fw/Types/StringTemplate.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/Seq/SeqArgsSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_RequestContextSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_StatusEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"
#include "config/FppConstantsAc.hpp"
#include "config/FwChanIdTypeAliasAc.h"
#include "config/FwPrmIdTypeAliasAc.h"
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

    //! Handler implementation for seqCancelIn
    //!
    //! port for requesting to cancel the currently running sequence
    void seqCancelIn_handler(FwIndexType portNum  //!< The port number
                             ) override;

    //! Handler implementation for seqRunIn
    //!
    //! port for requests to run sequences
    void seqRunIn_handler(FwIndexType portNum,             //!< The port number
                          const Fw::StringBase& filename,  //!< The sequence file
                          const Svc::SeqArgs& args         //!< Sequence arguments
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
    //! Pauses the execution of the sequencer, just before it is about to start spinning.
    //! This simply pends a pause flag that will be take before the sequence engine starts
    //! up again.
    //!
    //! This command completes immediately
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

    //! Implementation for action respond_noblock_OK of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to $block == NO_BLOCK requests with OK
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_noblock_OK(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with EXECUTION_ERROR
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_ERROR(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_NOT_ALLOWED of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with EXECUTION_ERROR
    //! Emit an event to say why we are rejecting this request in the current state
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_NOT_ALLOWED(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_block_OK of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to $block == BLOCK requests with OK
    //! Responds to any pending WAIT requests with OK
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_block_OK(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action respond_block_ERROR of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to $block == BLOCK requests with ERROR.
    //! Responds to any pending WAIT requests with ERROR
    void Svc_WasmSequencer_ControllerStateMachine_action_respond_block_ERROR(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action load of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! report that a RUN has begun on seqStartOut, and mark a run active so its
    //! terminal outcome (runCmd_OK / runCmd_ERROR) reports on seqDoneOut
    //! Load a pending module request
    void Svc_WasmSequencer_ControllerStateMachine_action_load(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleLoadFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event to denote failed module load
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleLoadFailed(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action invokeStart of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Invoke the start function on a loaded module
    void Svc_WasmSequencer_ControllerStateMachine_action_invokeStart(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action invokeMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Invoke the main function for module in context
    void Svc_WasmSequencer_ControllerStateMachine_action_invokeMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleInvalidMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting that a given module has no [valid] main function
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleInvalidMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleMainInvokeFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting why the invocation of a module main failed
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainInvokeFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleStartInvokeFailed of state machine
    //! Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting why the invocation of a module start failed
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartInvokeFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module succeeded during execution. Increment telemetry counters.
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleSucceeded(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleStartFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module failed during execution of start. Increment telemetry counters.
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action reportModuleFailed of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Emit an event noting a module failed during execution. Increment telemetry counters.
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleFailed(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action resetStore of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Create a new store with N_MODULES (parameter) modules
    void Svc_WasmSequencer_ControllerStateMachine_action_resetStore(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action setContext of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Set the current executing context
    void Svc_WasmSequencer_ControllerStateMachine_action_setContext(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
        ) override;

    //! Implementation for action clearContext of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Clear the current executing context
    void Svc_WasmSequencer_ControllerStateMachine_action_clearContext(
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

    //! Implementation for action clearExitStatus of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_clearExitStatus(
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

    //! Implementation for action setLastHostFunction of state machine Svc_WasmSequencer_EngineStateMachine
    void Svc_WasmSequencer_EngineStateMachine_action_setLastHostFunction(
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
        const Svc::WasmSequencer_RequestContext& value            //!< The value
    ) const override;

    //! Implementation for guard moduleHasValidMain of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if this module has a valid main function ([] -> i32)
    bool Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasValidMain(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
    ) const override;

    //! Implementation for guard invokeSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! return true if invokeStatus == SPACEWASM_OK. This flag is set as a result of invokeStart/invokeMain
    bool Svc_WasmSequencer_ControllerStateMachine_guard_invokeSucceeded(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
    ) const override;

    //! Implementation for guard interpreterSucceeded of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Check if the last engine execution finished executing successfully
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

    //! Buffer handed to the streaming loader, filled from `m_loadFile`.
    U8 m_readBuf[256];

    //! Opaque handle to the spacewasm engine, or null.
    spacewasm_t* m_wasm;

    //! Pending command waiting for a response
    struct WaitingCmd {
        FwOpcodeType opCode;
        U32 cmdSeq;

        WaitingCmd() = default;
        WaitingCmd(FwOpcodeType opCode_, U32 cmdSeq_) : opCode(opCode_), cmdSeq(cmdSeq_) {}
    };

    //! WAIT commands waiting for sequence completion
    Fw::FifoQueue<WaitingCmd, 8> m_waiting;

    struct ModuleLoad {
        Fw::FileNameString fileName;
        Fw::StringTemplate<16> moduleName;
    };

    ModuleLoad m_pendingLoad{};
    bool m_hasPendingLoad = false;
    WasmSequencer_Status m_loadFailureStatus = WasmSequencer_Status::OK;

    //! Currently stored sequence arguments
    Svc::SeqArgs m_args;

    WasmSequencer_RequestContext m_executingContext;
    bool m_hasExectingContext;

    //! Pending timer from sleep host function
    Fw::Time m_pendingTimer;
    bool m_hasPendingTimer;

    //! Wall-clock time at which the current blocking async host function
    //! (COMMAND / ASYNC_PORT) began awaiting its reply. Used to enforce
    //! STATEMENT_TIMEOUT_SECS. Only meaningful while awaiting one of those.
    Fw::Time m_statementStart;
    bool m_hasStatementStart;

    //! Flag indicating a function invocation failed
    spacewasm_status_t m_invokeStatus;

    //! Flag indicating interpreter is waiting to be paused
    bool m_pendingPause;

    //! Currently loading file handle
    Os::File* m_loadFile;

    //! Backing state for the periodically-written telemetry channels (see
    //! writeTelemetry_handler). Grouped so the counters read as one unit.
    struct Telemetry {
        //! Sequences successfully completed
        U64 sequencesSucceeded{0};

        //! Sequences that failed to validate or execute
        U64 sequencesFailed{0};

        //! Sequences that were cancelled
        U64 sequencesCancelled{0};

        //! Commands dispatched total
        U64 commandsDispatched{0};

        //! Number of commands that failed
        U64 commandsFailed{0};

        //! Currently running sequence name
        Fw::StringTemplate<FileNameStringSize> sequenceName{""};
    };

    Telemetry m_tlm;

    //! Monotonic counter of sequences started (bumped whenever the interpreter
    //! begins spinning a freshly-invoked program). The low 16 bits form the
    //! high half of the command context (cmdUid) so we can detect a command
    //! response that arrives late, after its originating sequence has ended.
    U32 m_sequencesStarted;

    //! Compose the command context (cmdUid) sent to the command dispatcher from
    //! the current sequence counter and the m_tlm.commandsDispatched counter. The
    //! low 16 bits of the latter form the low half of the cmdUid, letting us
    //! detect a response for a different instance of the same opcode.
    U32 makeCmdUid() const;

    //! Why the last sequence ended. Set as the program runs and reported
    //! together in SequenceFailed (see the controller reportModuleFailed action).
    struct ExitStatus {
        //! Reason the current program exited.
        //! By default this is INTERPRETER but can be overriden from host functions
        WasmSequencer_ExitReason reason{WasmSequencer_ExitReason::UNKNOWN};

        //! Currently stored exit code for non-INTERPRETER exits
        I32 code{0};

        //! Last run host function that could have caused the error
        WasmSequencer_HostFunction lastHostFunction{WasmSequencer_HostFunction::NONE};

        //! Reason last sequence trapped
        WasmSequencer_TrapReason lastTrapReason{WasmSequencer_TrapReason::NONE};
    };

    ExitStatus m_exit;

    //! Buffer to hold the serial output port invocation invoked by the guest
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> m_serialPortBuffer;

    //! A host function call the guest requested that is pending dispatch by the
    //! engine state machine (see dispatchPendingHostFunction). `kind` selects
    //! which arm of the `u` union carries the call's arguments.
    struct PendingHostFunction {
        PendingHostFunction() = default;
        bool isPending() const { return kind != WasmSequencer_HostFunction::NONE; }
        void clear() { kind = WasmSequencer_HostFunction::NONE; }

        WasmSequencer_HostFunction kind{WasmSequencer_HostFunction::NONE};

        // Handle that holds the Wasm guest memory pointer
        spacewasm_caller_t* caller{nullptr};

        //! Per-kind call arguments. Only the arm matching `kind` is live.
        union Args {
            // COMMAND: encoded command payload in guest memory
            struct {
                U32 ptr;
                U32 len;
            } command;

            // TELEMETRY: channel id plus where to write the serialized time and value
            struct {
                FwChanIdType chanId;
                U32 timePtr;
                U32 timeLen;
                U32 valuePtr;
                U32 valueLen;
            } telemetry;

            // PARAMETER: parameter id plus where to write the serialized value
            struct {
                FwPrmIdType prmId;
                U32 ptr;
                U32 len;
            } parameter;

            // EVENT: raw guest-requested severity (may be out of range) plus the message
            struct {
                U32 rawSeverity;
                U32 msgPtr;
                U32 msgLen;
            } event;

            // RSLEEP / ASLEEP: absolute/relative sleep duration
            struct {
                U64 us;
            } sleep;

            // ARGS: where to write the stored sequence arguments
            struct {
                U32 ptr;
                U32 len;
            } args;

            // TIME: where to write the serialized current time
            struct {
                U32 ptr;
                U32 len;
            } time;

            // SYNC_PORT: serial output port index plus payload in guest memory
            struct {
                U32 index;
                U32 ptr;
                U32 len;
            } syncPort;

            // ASYNC_PORT: like SYNC_PORT plus where to write the reply
            struct {
                U32 index;
                U32 ptr;
                U32 len;
                U32 returnPtr;
                U32 returnLen;
            } asyncPort;

            Args() : command{0, 0} {}
        } u;
    };

    PendingHostFunction m_pendingHostFunction;

    //! Helper function for checking the signature of a modules main function
    spacewasm_status_t validateModuleMain(WasmSequencer_ModuleIdx moduleIdx) const;

    //! Resolve and invoke the "main" export of the given module, recording the
    //! result in m_invokeStatus. Shared by the invokeMain / invokeMainPending
    //! state-machine actions.
    void invokeMainOnModule(WasmSequencer_ModuleIdx moduleIdx);

    //! Respond to a request with certain reply
    void respondToRequest(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response);

    //! Send response to all waiting requests
    void respondToWaiting(const Fw::CmdResponse& response);

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
