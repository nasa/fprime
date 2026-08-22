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
#include "Fw/Types/StringBase.hpp"
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
#include "Utils/Types/CircularBuffer.hpp"
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
    //! Port to periodically drive sleep-wake and host-function-timeout checks
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

    //! Handler implementation for serialIn
    //!
    //! Port for receiving serial messages from other components.
    //! When the serial in queue is configured as unified, all port indexes will share the same queue
    //! When the serial in queue is configured as split, each port index will have it's own queue
    void serialIn_handler(FwIndexType portNum,          //!< The port number
                          Fw::LinearBufferBase& buffer  //!< The serialization buffer
                          ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command RUN
    //!
    //! Run a Wasm module main function on it's own in the interpreter
    //! This command first resets the store (discarding any modules previously
    //! staged with LOAD), then is short-hand for:
    //! 1. LOAD [fileName] ""
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
    //! Loads and validates a WebAssembly module into the store under the given name.
    //! Naming the module lets its exports be referenced by other modules and lets
    //! INVOKE, GLOBAL_GET and GLOBAL_SET address it. Use an empty name for a single,
    //! standalone module.
    void LOAD_cmdHandler(
        FwOpcodeType opCode,               //!< The opcode
        U32 cmdSeq,                        //!< The command sequence number
        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
        const Fw::CmdStringArg& name       //!< WebAssembly module name (empty for a single unnamed module)
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

    //! Handler implementation for command GLOBAL_SET_I32
    //!
    //! Set a global variable to a given i32 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of i32 type
    void GLOBAL_SET_I32_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   I32 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_SET_I64
    //!
    //! Set a global variable to a given i64 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of i64 type
    void GLOBAL_SET_I64_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   I64 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_SET_F32
    //!
    //! Set a global variable to a given f32 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of f32 type
    void GLOBAL_SET_F32_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   F32 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_SET_F64
    //!
    //! Set a global variable to a given f64 value.
    //! Command fails if the module is not found, global is not exported, mutable, or of f64 type
    void GLOBAL_SET_F64_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                                   U32 cmdSeq,                          //!< The command sequence number
                                   const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                                   const Fw::CmdStringArg& name,        //!< Name of the global
                                   F64 value                            //!< Value to set global to
                                   ) override;

    //! Handler implementation for command GLOBAL_GET
    //!
    //! Get the current value of a global variable and emit an event
    //! Command fails if the module is not found, global is not exported, mutable, or of f64 type
    void GLOBAL_GET_cmdHandler(FwOpcodeType opCode,                 //!< The opcode
                               U32 cmdSeq,                          //!< The command sequence number
                               const Fw::CmdStringArg& moduleName,  //!< Name of the module to set global for
                               const Fw::CmdStringArg& name         //!< Name of the global
                               ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action processInvoke of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Action to save the invoke arguments and emit an 'invoked' signal
    void Svc_WasmSequencer_ControllerStateMachine_action_processInvoke(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_InvokeRequest& value             //!< The value
        ) override;

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

    //! Implementation for action respondInvoke_BUSY of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with EXECUTION_ERROR
    //! Emit an event to say why we are rejecting this request in the current state
    void Svc_WasmSequencer_ControllerStateMachine_action_respondInvoke_BUSY(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_InvokeRequest& value             //!< The value
        ) override;

    //! Implementation for action respondLoad_BUSY of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Responds to request with EXECUTION_ERROR
    //! Emit an event to say why we are rejecting this request in the current state
    void Svc_WasmSequencer_ControllerStateMachine_action_respondLoad_BUSY(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_LoadRequest& value               //!< The value
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
    //! Load a module into the store.
    //! Emits `loadSucceded` or `loadFailed` depending on result
    void Svc_WasmSequencer_ControllerStateMachine_action_load(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_LoadRequest& value               //!< The value
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

    //! Implementation for action reportModuleStarted of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Reports that a module started running it's main function
    void Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStarted(
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

    //! Implementation for action runEngine of state machine Svc_WasmSequencer_ControllerStateMachine
    //!
    //! Send a signal to the engine state machine to begin running
    //! The engine will asynchronously reply with the `engineFinished` signal
    void Svc_WasmSequencer_ControllerStateMachine_action_runEngine(
        SmId smId,                                                //!< The state machine id
        Svc_WasmSequencer_ControllerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value            //!< The value
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

    //! Implementation for action setContext of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! Set the current executing context
    void Svc_WasmSequencer_EngineStateMachine_action_setContext(
        SmId smId,                                            //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_RequestContext& value        //!< The value
        ) override;

    //! Implementation for action clearContext of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! Clear the current executing context
    void Svc_WasmSequencer_EngineStateMachine_action_clearContext(
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

    //! Implementation for action dequeueSerialAndResume of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! Dequeue a serial message into the host guest memory and resume the interpreter
    void Svc_WasmSequencer_EngineStateMachine_action_dequeueSerialAndResume(
        SmId smId,                                            //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal,  //!< The signal
        const FwIndexType& value                              //!< The value
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
    //! return true if execution should pause before spinning the interpreter again
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

    //! Implementation for guard blockingSerialIn of state machine Svc_WasmSequencer_EngineStateMachine
    //!
    //! Check if we are currently blocking on a serial_recv() for a given serial port index
    bool Svc_WasmSequencer_EngineStateMachine_guard_blockingSerialIn(
        SmId smId,                                            //!< The state machine id
        Svc_WasmSequencer_EngineStateMachine::Signal signal,  //!< The signal
        const FwIndexType& value                              //!< The value
    ) const override;

  private:
    /// The global allocator callbacks
    U8* globalAlloc(U32 size, U32 align);
    void globalDealloc(const U8* ptr);

    /// C-callback trampolines for the spacewasm global-allocator registry. Static
    /// so they can be used as plain function pointers (no lambdas, per CPP-7);
    /// each forwards to the owning instance carried in `userdata`.
    static U8* globalAllocThunk(void* userdata, size_t size, size_t align);
    static void globalDeallocThunk(void* userdata, U8* ptr, size_t size, size_t align);

    /// The Wasm guest allocator callbacks
    U8* guestAlloc(U32 size, U32 align);
    void guestDealloc(const U8* ptr, U32 size);

    //! Fill `m_readBuf` with the next chunk of the module file being loaded.
    spacewasm_read_result_t readModuleChunk(const U8** outBuf, size_t* outLen);

    //! Create a fresh interpreter store with the given module capacity,
    //! destroying any existing store first.
    void createStore();

    //! Destroy the current interpreter store, if any, releasing its memory.
    void destroyStore();

    //! Take control of the spacewasm global allocator on this WasmSequence
    void takeAllocatorLock();

    //! Release control of the spacewasm global allocator on this WasmSequence
    void releaseAllocatorLock();

    //! Map a spacewasm_trap_t onto the TrapReason event enum.
    static Svc::WasmSequencer_TrapReason::T mapTrapReason(spacewasm_trap_t trap);

    //! Record the SeqName telemetry for a load. Uses moduleName when non-empty;
    //! otherwise derives it from the file's basename with any ".wasm" suffix
    //! stripped (an empty-name RUN / LOAD).
    void setSequenceName(const Fw::StringBase& filePath, const Fw::StringBase& moduleName);

    //! Static pool backing the process-wide spacewasm global page allocator.
    alignas(16) U8 m_memory_pool[Svc::WasmSequencerConfig::DYNAMIC_MEMORY_SIZE]{};

    //! Which pages of `m_memory_pool` are currently handed out (bit i == page i).
    U32 m_page_used_mask;

    //! Static pool backing the per-load guest linear-memory allocator; a simple
    //! bump allocator (guest modules are compiled with memory.grow disabled).
    alignas(16) U8 m_guest_pool[Svc::WasmSequencerConfig::GUEST_MEMORY_SIZE]{};

    //! Current bump offset into `m_guest_pool`.
    FwSizeType m_guest_offset;

    //! Buffer handed to the streaming loader, filled from `m_loadFile`.
    U8 m_readBuf[Svc::WasmSequencerConfig::LOAD_READ_CHUNK_SIZE]{};

    //! Opaque handle to the spacewasm engine, or null.
    spacewasm_t* m_wasm;

    //! Pending command waiting for a response
    struct WaitingCmd {
        FwOpcodeType opCode;
        U32 cmdSeq;

        WaitingCmd() : opCode(0), cmdSeq(0) {}
        WaitingCmd(FwOpcodeType opCode_, U32 cmdSeq_) : opCode(opCode_), cmdSeq(cmdSeq_) {}
    };

    //! WAIT commands waiting for sequence completion
    Fw::FifoQueue<WaitingCmd, 8> m_waiting;

    //! Status stored by the `load` action indicated failure reason
    WasmSequencer_Status m_loadFailureStatus = WasmSequencer_Status::OK;

    //! Currently stored sequence arguments
    Svc::SeqArgs m_args;

    //! File path of the last module load
    Fw::FileNameString m_lastLoadFileName;

    WasmSequencer_RequestContext m_executingContext;
    bool m_hasExectingContext;

    //! Pending timer from sleep host function
    Fw::Time m_pendingTimer;
    bool m_hasPendingTimer;

    //! Wall-clock time at which the current blocking async host function
    //! (COMMAND / ASYNC_PORT) began awaiting its reply. Used to enforce
    //! HOST_FUNCTION_TIMEOUT_SECS. Only meaningful while awaiting one of those.
    Fw::Time m_hostFunctionStart;
    bool m_hasHostFunctionStart;

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

    //! Why the last sequence ended. Set as the program runs and classified into
    //! the appropriate completion event (see reportSequenceFailure in the controller).
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

    //! Serial in buffer data
    U8 m_serialInQueueData[NUM_SERIALIN_INPUT_PORTS][Svc::WasmSequencerConfig::SERIAL_IN_QUEUE_SIZE];

    //! Queues (or queue) that handle inputs on the serial input port
    Types::CircularBuffer m_serialInQueue[NUM_SERIALIN_INPUT_PORTS];

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

            // SERIAL_OUT: serial output port index plus payload in guest memory
            struct {
                U32 index;
                U32 ptr;
                U32 len;
            } serialOut;

            // SERIAL_RECV: Read a message from the serialIn port queue given a port index
            struct {
                U32 index;
                U32 dataPtr;
                U32 dataSize;
                U32 actualSizePtr;
                Svc::BlockState::T blockingType;
            } serialRecv;

            Args() : command{0, 0} {}
        } u;
    };

    PendingHostFunction m_pendingHostFunction;

    //! Helper function for checking the signature of a modules main function
    spacewasm_status_t validateModuleMain(WasmSequencer_ModuleIdx moduleIdx) const;

    //! Report an engine-completion failure as the appropriate distinct event
    //! (SequenceExited / SequencePanic / SequenceTrapped / SequenceHostFailure,
    //! or SequenceCancelled) and bump the matching telemetry counter. `phase`
    //! records whether the failure occurred running the module's start function
    //! or its main entrypoint.
    void reportSequenceFailure(WasmSequencer_ModuleIdx moduleIdx, WasmSequencer_SequencePhase phase);

    //! Check if there are any pending flags before trying to fill the pending state
    Fw::Success checkPendingFlags();

    //! Respond to a request with certain reply
    void respondToRequest(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response);

    //! Send response to all waiting requests
    void respondToWaiting(const Fw::CmdResponse& response);

    //! Report that a RUN finished on seqDoneOut (if connected) with the given
    //! response. Only emits for RUN-sourced requests, matching the RUN-gated
    //! seqStartOut in reportModuleStarted, so the seqStart/seqDone pair stays
    //! balanced. A no-op for INVOKE/LOAD, so it is safe to call from every
    //! terminal responder.
    void reportSeqDone(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response);

    //! Set a global to a value given the name of the module, global export name and value
    spacewasm_status_t setGlobal(const Fw::StringBase& moduleName, const Fw::StringBase& name, spacewasm_value_t value);

    //! Get the value of a global variable given its module name and global name
    spacewasm_status_t getGlobal(const Fw::StringBase& moduleName,
                                 const Fw::StringBase& name,
                                 spacewasm_value_t* value);

    //! Set up the fprime host interface
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

    spacewasm_hostcall_result_t wasmSerialOut(struct spacewasm_caller_t* caller,
                                              const struct spacewasm_value_t* params,
                                              size_t n_params,
                                              struct spacewasm_value_t* out_result);

    spacewasm_hostcall_result_t wasmSerialRecv(struct spacewasm_caller_t* caller,
                                               const struct spacewasm_value_t* params,
                                               size_t n_params,
                                               struct spacewasm_value_t* out_result);

    //! Validate a guest serial-port request (shared by the sync and async variants).
    //! Emits the appropriate warning event and returns false when the port index is
    //! out of range/unconnected or the payload length exceeds the configured maximum.
    bool validateSerialPortOutput(WasmSequencer_HostFunction::T kind, I32 index, U32 len);
    bool validateSerialPortRecv(WasmSequencer_HostFunction::T kind, I32 index, U32 len);

    // A global static lock. This is needed to allow the global allocator in spacewasm
    // to not require to pass context to fine grained context to allocations.
    // Read more about this in the SDD.
    static Os::Mutex* getGlobalAllocatorLock();
};

}  // namespace Svc

#endif
