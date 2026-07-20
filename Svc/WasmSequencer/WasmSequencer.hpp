// ======================================================================
// \title  WasmSequencer.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component implementation class
// ======================================================================

#ifndef Svc_WasmSequencer_HPP
#define Svc_WasmSequencer_HPP

#include "Fw/Types/SuccessEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"
#include "config/WasmSequencerConfig.hpp"
#include "spacewasm.h"

extern "C" {
uint8_t* wasmSeqGlobalAlloc(void* userdata, std::size_t size, std::size_t align);
void wasmSeqGlobalDealloc(void* userdata, uint8_t* ptr, std::size_t size, std::size_t align);
uint8_t* wasmSeqGuestAlloc(void* userdata, std::size_t size, std::size_t align);
uint8_t* wasmSeqGuestRealloc(void* userdata, uint8_t* ptr, std::size_t oldSize, std::size_t newSize, std::size_t align);
void wasmSeqGuestDealloc(void* userdata, uint8_t* ptr, std::size_t size, std::size_t align);
spacewasm_read_result_t wasmSeqReadModule(void* userdata, const uint8_t** outBuf, std::size_t* outLen);
}

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
    void RUN_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                        U32 cmdSeq,                        //!< The command sequence number
                        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
                        Svc::BlockState block              //!< Return command status when complete or not
                        ) override;

    //! Handler implementation for command WAIT
    void WAIT_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;

    //! Handler implementation for command LOAD
    void LOAD_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                         U32 cmdSeq,                       //!< The command sequence number
                         const Fw::CmdStringArg& fileName  //!< The name of the sequence file
                         ) override;

    //! Handler implementation for command LOAD_NAME
    void LOAD_NAME_cmdHandler(
        FwOpcodeType opCode,               //!< The opcode
        U32 cmdSeq,                        //!< The command sequence number
        const Fw::CmdStringArg& fileName,  //!< The name of the sequence file
        const Fw::CmdStringArg& name  //!< WebAssembly module name, must not conflict with previously loaded modules
        ) override;

    //! Handler implementation for command INVOKE
    void INVOKE_cmdHandler(FwOpcodeType opCode,                  //!< The opcode
                           U32 cmdSeq,                           //!< The command sequence number
                           const Fw::CmdStringArg& module,       //!< Name of the module to invoke a function from
                           const Fw::CmdStringArg& functionName  //!< Name of the function to be invoked
                           ) override;

    //! Handler implementation for command CANCEL
    void CANCEL_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                           U32 cmdSeq            //!< The command sequence number
                           ) override;

    //! Handler implementation for command BREAK
    void BREAK_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                          U32 cmdSeq            //!< The command sequence number
                          ) override;

    //! Handler implementation for command TRACE
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

    //! simply raises the "entered" signal
    void Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! Create a new store with N_MODULES modules
    void Svc_WasmSequencer_SequencerStateMachine_action_createStore(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! Clears the Wasm store and deallocates everything
    void Svc_WasmSequencer_SequencerStateMachine_action_clearStore(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! sets up a function invocation on the interpreter from the pending FunctionInvoke member var
    void Svc_WasmSequencer_SequencerStateMachine_action_invoke(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! Set the Wasm module to load and execute; sets pendingLoad and pendingInvoke
    void Svc_WasmSequencer_SequencerStateMachine_action_pendRun(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,
        const Svc::WasmSequencer_SequenceExecutionArgs& value) override;

    //! Set the Wasm module to load but not execute; sets pendingLoad
    void Svc_WasmSequencer_SequencerStateMachine_action_pendLoad(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,
        const Svc::WasmSequencer_ModuleLoad& value) override;

    //! Records the freshly loaded/validated module handle
    void Svc_WasmSequencer_SequencerStateMachine_action_appendModuleToStore(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,
        const Svc::WasmSequencer_Module& value) override;

    //! Reports the module load failure as an event
    void Svc_WasmSequencer_SequencerStateMachine_action_reportLoadFailure(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal,
        const Svc::WasmSequencer_LoadFailure& value) override;

    //! Load a pending module request
    void Svc_WasmSequencer_SequencerStateMachine_action_load(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! Invoke the start function on a loaded module
    void Svc_WasmSequencer_SequencerStateMachine_action_invokeStart(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! spins the interpreter loop, executing up to a bounded number of instructions
    void Svc_WasmSequencer_SequencerStateMachine_action_spin(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! reports that the interpreter ran to completion
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqSucceeded(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! reports that the sequence was cancelled
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqCancelled(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! reports that the sequence failed to execute successfully
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqFailed(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! reports that the interpreter trapped, with the trap reason
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqTrap(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! reports that the interpreter failed to read an instruction from memory
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqReadError(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! reports that execution was paused at a breakpoint
    void Svc_WasmSequencer_SequencerStateMachine_action_report_seqBroken(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! responds to the calling command with OK
    void Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_OK(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! responds to the calling command with EXECUTION_ERROR
    void Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! stores the requested wake time for the SLEEPING state
    void Svc_WasmSequencer_SequencerStateMachine_action_set_sleepTime(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! checks if the current statement has timed out
    void Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! checks if the sequencer should wake from sleep
    void Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! sets the "break before next line" flag to true
    void Svc_WasmSequencer_SequencerStateMachine_action_setBreakBeforeNextLine(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

    //! sets the "break before next line" flag to false
    void Svc_WasmSequencer_SequencerStateMachine_action_clearBreakBeforeNextLine(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! return true if there is a function invocation pending to be set up
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingInvoke(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) const override;

    //! return true if there is a module load pending
    bool Svc_WasmSequencer_SequencerStateMachine_guard_pendingLoad(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) const override;

    //! return true if the freshly loaded module declared a Wasm start function
    bool Svc_WasmSequencer_SequencerStateMachine_guard_lastModuleHasStart(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) const override;

    //! return true if execution should break before spinning the next statement
    bool Svc_WasmSequencer_SequencerStateMachine_guard_shouldBreak(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) const override;

    //! return true if the interpreter is currently executing a module's start function
    bool Svc_WasmSequencer_SequencerStateMachine_guard_executingStart(
        SmId smId,
        Svc_WasmSequencer_SequencerStateMachine::Signal signal) const override;

  private:
    // ----------------------------------------------------------------------
    // Interpreter store and page-backed allocators
    //
    // The `spacewasm_c_api` crate installs a process-wide page allocator as its
    // global (Rust) allocator via spacewasm_set_global_allocator. Each page it
    // requests is served from `m_memory_pool` below, so the interpreter's store
    // and compiled bytecode live entirely within this component's memory. Guest
    // linear memory is served separately from `m_guest_pool` via a per-load
    // spacewasm_allocator_t.
    // ----------------------------------------------------------------------

    //! The C-linkage trampolines forward into these private instance methods.
    //! Function names are parenthesized so the return type + `::name` is not
    //! parsed as a qualified-id.
    friend uint8_t*(::wasmSeqGlobalAlloc)(void* userdata, std::size_t size, std::size_t align);
    friend void(::wasmSeqGlobalDealloc)(void* userdata, uint8_t* ptr, std::size_t size, std::size_t align);
    friend uint8_t*(::wasmSeqGuestAlloc)(void* userdata, std::size_t size, std::size_t align);
    friend uint8_t*(::wasmSeqGuestRealloc)(void* userdata,
                                           uint8_t* ptr,
                                           std::size_t oldSize,
                                           std::size_t newSize,
                                           std::size_t align);
    friend void(::wasmSeqGuestDealloc)(void* userdata, uint8_t* ptr, std::size_t size, std::size_t align);
    friend spacewasm_read_result_t(::wasmSeqReadModule)(void* userdata, const uint8_t** outBuf, std::size_t* outLen);

    //! Hand out a fixed-size page from `m_memory_pool` for the Rust allocator.
    U8* allocPage(U32 size, U32 align);
    //! Return a page previously handed out by allocPage back to the pool.
    void deallocPage(U8* ptr);

    //! Serve `size` bytes for a guest linear memory from `m_guest_pool`.
    U8* guestAlloc(U32 size, U32 align);
    //! Grow/shrink a guest linear-memory allocation.
    U8* guestRealloc(U8* ptr, U32 oldSize, U32 newSize, U32 align);
    //! Release a guest linear-memory allocation.
    void guestDealloc(U8* ptr, U32 size);

    //! Fill `m_readBuf` with the next chunk of the module file being loaded.
    spacewasm_read_result_t readModuleChunk(const U8** outBuf, std::size_t* outLen);

    //! Create a fresh interpreter store with the given module capacity,
    //! destroying any existing store first.
    Fw::Success createStore(U16 moduleCount  //!< Maximum number of modules
    );

    //! Destroy the current interpreter store, if any, releasing its memory.
    void destroyStore();

    //! Map a spacewasm store-allocation status onto the AllocError event enum.
    static Svc::WasmSequencer_AllocError::T mapAllocError(spacewasm_status_t status);

    //! Map a spacewasm_trap_t onto the TrapReason event enum.
    static Svc::WasmSequencer_TrapReason::T mapTrapReason(spacewasm_trap_t trap);

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

    //! Opaque handle to the spacewasm store, or null.
    spacewasm_store_t* m_store;

    //! Module capacity of the current/last store, remembered so CLEAR_STORE can
    //! recreate a store with the same capacity.
    U16 m_maxModules;

    //! Index of the most-recently-loaded module within the store.
    U32 m_moduleIndex;

    //! True while the interpreter is running a module's Wasm start function.
    bool m_executingStart;

    //! Opcode/cmdSeq of the command that started the current run, saved so a
    //! deferred (blocking) response can be sent once the SM reaches a terminal
    //! state.
    FwOpcodeType m_savedOpCode;
    U32 m_savedCmdSeq;

    //! Whether the current run should defer its command response to a terminal
    //! SM transition (true) or was already answered (false).
    bool m_shouldRespond;

    //! Pending module load (path + optional name) for the `load` action.
    Svc::WasmSequencer_ModuleLoad m_pendingLoad;
    bool m_hasPendingLoad;

    //! Pending function invocation for the `invoke` action.
    Svc::WasmSequencer_FunctionInvoke m_pendingInvoke;
    bool m_hasPendingInvoke;

    //! Whether the most-recently-loaded module declared a start function.
    bool m_lastModuleHasStart;

    //! Break-before-next-statement flag, toggled by BREAK/CONTINUE.
    bool m_breakBeforeNextLine;

    //! The open module file for the streaming loader, valid only during `load`.
    //! Held as a void* so the header does not depend on Os/File.hpp.
    void* m_loadFile;

    //! Most recent trap reason, stashed so the payload-less report_seqTrap
    //! action can render it.
    spacewasm_trap_t m_lastTrap;
};

}  // namespace Svc

#endif
