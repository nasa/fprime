// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Fw/DataStructures/FifoQueue.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "config/FwSizeTypeAliasAc.h"
#include "config/WasmSequencerConfig.hpp"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"

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

    //! Handler for input port writeTelemetry
    void writeTelemetry_handler(FwIndexType portNum,  //!< The port number
                                U32 context           //!< The call order
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
    //! If $block == Svc.BlockState.BLOCK this command will wait for complemention.
    void RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                        U32 cmdSeq,                        //!< The command sequence number
                        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                        const Svc::BlockState& block       //!< Return command status when complete or not
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
    //! Invoke a main function from a loaded module
    void INVOKE_cmdHandler(FwOpcodeType opCode,             //!< The opcode
                           U32 cmdSeq,                      //!< The command sequence number
                           const Fw::CmdStringArg& module,  //!< Name of the module to invoke a function from
                           const Svc::BlockState& block) override;

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
    void CONTINUE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                             U32 cmdSeq            //!< The command sequence number
                             ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action signalEntered of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Raises the "entered" signal
    void Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action resetStore of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Create a new store with N_MODULES (parameter) modules
    void Svc_WasmSequencer_SequencerStateMachine_action_resetStore(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action invokeMainOfLastModule of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Invoke the main function of last module loaded into the store
    void Svc_WasmSequencer_SequencerStateMachine_action_invokeMainOfLastModule(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action invokeMain of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Invoke the main function on a module given its index
    void Svc_WasmSequencer_SequencerStateMachine_action_invokeMain(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleIdx& value                //!< The value
        ) override;

    //! Implementation for action pendRun of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Set the Wasm module to load and execute
    //! Sets pendingInvoke
    void Svc_WasmSequencer_SequencerStateMachine_action_pendRun(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_SequenceExecutionArgs& value    //!< The value
        ) override;

    //! Implementation for action pendLoad of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Set the Wasm module to load but not execute
    void Svc_WasmSequencer_SequencerStateMachine_action_pendLoad(
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const Svc::WasmSequencer_ModuleLoad& value               //!< The value
        ) override;

    //! Implementation for action reportLoadFailure of state machine Svc_WasmSequencer_SequencerStateMachine
    void Svc_WasmSequencer_SequencerStateMachine_action_reportLoadFailure(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action reportInvokeFailure of state machine Svc_WasmSequencer_SequencerStateMachine
    void Svc_WasmSequencer_SequencerStateMachine_action_reportInvokeFailure(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action load of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Load a pending module request
    void Svc_WasmSequencer_SequencerStateMachine_action_load(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendLoadCmdResponse_OK of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to any command waiting on load with OK
    void Svc_WasmSequencer_SequencerStateMachine_action_sendLoadCmdResponse_OK(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendLoadCmdResponse_EXECUTION_ERROR of state machine
    //! Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to any command waiting on load with EXECUTION_ERROR
    void Svc_WasmSequencer_SequencerStateMachine_action_sendLoadCmdResponse_EXECUTION_ERROR(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action invokeStartOfLastModule of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! Invoke the start function on a loaded module
    void Svc_WasmSequencer_SequencerStateMachine_action_invokeStartOfLastModule(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendRunCmdResponse_OK of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to all commands waiting for finish with OK
    void Svc_WasmSequencer_SequencerStateMachine_action_sendRunCmdResponse_OK(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action sendRunCmdResponse_EXECUTION_ERROR of state machine
    //! Svc_WasmSequencer_SequencerStateMachine
    //!
    //! responds to all commands waiting for finish with EXECUTION_ERROR
    void Svc_WasmSequencer_SequencerStateMachine_action_sendRunCmdResponse_EXECUTION_ERROR(
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
        SmId smId,                                               //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,  //!< The signal
        const WasmSequencer_TrapReason& trapReason) override;

    //! Implementation for action report_seqPaused of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! reports that execution was paused at a breakpoint
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqPaused(
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

    //! Implementation for action pendPause of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! sets the pause flag to true
    void Svc_WasmSequencer_SequencerStateMachine_action_pendPause(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action clearPause of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! sets the pause flag to false
    void Svc_WasmSequencer_SequencerStateMachine_action_clearPause(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action dispatchPendingHostFunction of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! dispatch a host function port call
    void Svc_WasmSequencer_SequencerStateMachine_action_dispatchPendingHostFunction(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

    //! Implementation for action dispatchPendingHostFunction of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! dispatch a host function port call
    void Svc_WasmSequencer_SequencerStateMachine_action_clearPendingHostFunction(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
        ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard pendingRun of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if the module is pending to execute after loading
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingRun(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard invokeFailed of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if an invoke succeeded and the interpreter can now run
    bool Svc_WasmSequencer_SequencerStateMachine_guard_invokeFailed(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingPause of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if execution should pause before spinning the next statement
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingPause(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingHostFunction of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! a host function is waiting to be processed
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingHostFunction(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard pendingTimer of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if there is a pending sleep timer
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingTimer(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard moduleLoadSucceeded of state machine Svc_WasmSequencer_SequencerStateMachine
    //!
    //! return true if the last module load succeeded
    bool Svc_WasmSequencer_SequencerStateMachine_guard_moduleLoadSucceeded(
        SmId smId,                                              //!< The state machine id
        Svc_WasmSequencer_SequencerStateMachine::Signal signal  //!< The signal
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

    //! Emit a guest event at the requested severity id (see FprimeEventSeverity
    //! in fprime.h); unknown severities fall back to ACTIVITY_HI.
    void emitGuestEvent(U32 severity, const Fw::LogStringArg& msg);

    //! Map an F´ command response onto the FprimeCmdResponse guest enum
    //! (see fprime.h), returned to the guest on resume.
    static I32 mapCmdResponse(const Fw::CmdResponse& response);

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

    //! Index of the most-recently-loaded module within the store.
    U32 m_moduleIndex;

    //! Pending command waiting for a response
    struct PendingCmd {
        FwOpcodeType opCode;
        U32 cmdSeq;

        PendingCmd() = default;
        PendingCmd(FwOpcodeType opCode, U32 cmdSeq) : opCode(opCode), cmdSeq(cmdSeq) {}
    };

    //! Commands pending interpreter finish
    Fw::FifoQueue<PendingCmd, 8> m_pendingFinishCmds;

    PendingCmd m_pendingLoadCmd;
    bool m_hasPendingLoadCmd;

    //! Pending module load (path + optional name) for the `load` action.
    Svc::WasmSequencer_ModuleLoad m_pendingLoad;
    bool m_hasPendingLoad;

    //! Pending timer from sleep host function
    Fw::Time m_pendingTimer;
    bool m_hasPendingTimer;

    //! Break-before-next-statement flag, toggled by BREAK/CONTINUE.
    bool m_breakBeforeNextLine;

    //! Flag indicating a function invocation failed
    spacewasm_status_t m_invokeStatus;

    //! Flag indicating module load failure/success
    spacewasm_status_t m_loadStatus;

    //! Flag indicating module is pending execution of main
    bool m_pendingRun;

    //! Flag indicating interpreter is waiting to be paused
    bool m_pendingPause;

    //! Currently loading file handle
    Os::File* m_loadFile;

    struct PendingHostFunction {
        PendingHostFunction() = default;
        bool isPending() const { return kind != WasmSequencer_HostFunction::NONE; }
        void clear() { kind = WasmSequencer_HostFunction::NONE; }

        WasmSequencer_HostFunction kind;

        // TODO(tumbar) We should be able to do memory ops on `spacewasm_t`
        spacewasm_caller_t* caller;

        FwChanIdType chan_id;
        FwPrmIdType prm_id;
        Fw::LogSeverity severity;

        // The first pointer/length in one of the host commands
        U32 ptr1;
        U32 len1;

        // The second pointer/length in one of the host commands
        U32 ptr2;
        U32 len2;

        // The absolute/relative time for [ar]sleep
        U64 time_us;
    };

    PendingHostFunction m_pendingHostFunction;

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

    // A global static lock. This is needed to allow the global allocator in spacewasm
    // to not require to pass context to fine grained context to allocations.
    // Read more about this in the SDD.
    static Os::Mutex* getGlobalAllocatorLock();
};

}  // namespace Svc

#endif
