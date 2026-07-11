// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Fw/Types/Assert.hpp"
#include "Svc/WasmSequencer/WasmSequencerFFI.hpp"
#include "Svc/WasmSequencer/fprime_spacewasm/fprime_spacewasm.h"
#include "config/WasmSequencerConfig.hpp"

namespace Svc {

namespace {
//! The single active WasmSequencer instance. The `fprime_spacewasm` crate
//! installs a process-global allocator whose pages are served from that
//! instance's page pool, so at most one instance may exist at a time.
WasmSequencer* g_activeSequencer = nullptr;
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName)
    : WasmSequencerComponentBase(compName), m_store(nullptr), m_maxModules(0) {
    // The Rust global allocator draws pages from this instance; only one may be active.
    FW_ASSERT(g_activeSequencer == nullptr);
    for (U32 i = 0; i < SVC_WASMSEQUENCER_PAGE_COUNT; i++) {
        this->m_pageUsed[i] = false;
    }
    g_activeSequencer = this;
}

WasmSequencer ::~WasmSequencer() {
    this->destroyStore();
    g_activeSequencer = nullptr;
}

// ----------------------------------------------------------------------
// Interpreter store and page-backed allocator
// ----------------------------------------------------------------------

U8* WasmSequencer ::allocPage(AllocResult* result, U32 size) {
    // The Rust PageAllocator only ever requests whole pages; anything larger
    // than a page cannot be served from this pool.
    if (size > SVC_WASMSEQUENCER_PAGE_SIZE) {
        *result = AllocResult::AllocationFailed;
        return nullptr;
    }

    for (U32 slot = 0; slot < SVC_WASMSEQUENCER_PAGE_COUNT; slot++) {
        if (!this->m_pageUsed[slot]) {
            this->m_pageUsed[slot] = true;
            *result = AllocResult::Ok;
            return &this->m_pages[slot * SVC_WASMSEQUENCER_PAGE_SIZE];
        }
    }

    // No free pages remain in the pool.
    *result = AllocResult::OutOfMemory;
    return nullptr;
}

void WasmSequencer ::deallocPage(U8* ptr, U32 size) {
    (void)size;
    if (ptr == nullptr) {
        return;
    }

    // Map the pointer back to its page slot and mark it free.
    const FwSizeType offset = static_cast<FwSizeType>(ptr - this->m_pages);
    const U32 slot = static_cast<U32>(offset / SVC_WASMSEQUENCER_PAGE_SIZE);
    FW_ASSERT(slot < SVC_WASMSEQUENCER_PAGE_COUNT, static_cast<FwAssertArgType>(slot));
    FW_ASSERT(this->m_pageUsed[slot]);
    this->m_pageUsed[slot] = false;
}

void WasmSequencer ::createStore(U16 moduleCount) {
    this->destroyStore();
    const U32 status = fprime_spacewasm_store_new(moduleCount, &this->m_store);
    if (status != 0) {
        // Allocation failed; leave the store null.
        this->m_store = nullptr;
    }
}

void WasmSequencer ::destroyStore() {
    if (this->m_store != nullptr) {
        fprime_spacewasm_store_delete(this->m_store);
        this->m_store = nullptr;
    }
}

extern "C" {
//! Page allocation hook imported by the `fprime_spacewasm` Rust crate. Routes to
//! the active WasmSequencer instance's page pool.
U8* svc_wasmsequencer_alloc_page(AllocResult* result, U32 size) {
    if (g_activeSequencer == nullptr) {
        *result = AllocResult::AllocationFailed;
        return nullptr;
    }
    return g_activeSequencer->allocPage(result, size);
}

//! Page deallocation hook imported by the `fprime_spacewasm` Rust crate.
void svc_wasmsequencer_dealloc_page(U8* ptr, U32 size) {
    if (g_activeSequencer != nullptr) {
        g_activeSequencer->deallocPage(ptr, size);
    }
}
}  // extern "C"

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::checkTimers_handler(FwIndexType portNum, U32 context) {
    auto now = this->getTime();
}

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    // TODO
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void WasmSequencer ::RUN_cmdHandler(FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    Svc::BlockState block) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::LOAD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& fileName) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::LOAD_NAME_cmdHandler(FwOpcodeType opCode,
                                          U32 cmdSeq,
                                          const Fw::CmdStringArg& fileName,
                                          const Fw::CmdStringArg& name) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Fw::CmdStringArg& functionName) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CLEAR_STORE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::REINITIALIZE_STORE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U16 module_count) {
    this->m_maxModules = module_count;
    this->createStore(module_count);
    this->cmdResponse_out(opCode, cmdSeq,
                          (this->m_store != nullptr) ? Fw::CmdResponse::OK : Fw::CmdResponse::EXECUTION_ERROR);
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::BREAK_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::TRACE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invoke(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_setLoad(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleLoad& value) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // Recreate a fresh store with the same module capacity, clearing all
    // loaded modules, tables and memories.
    this->createStore(this->m_maxModules);
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_reportValidationFailure(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_VALIDATION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pushModuleToStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqTrap(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqReadError(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqBroken(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_OK(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_set_sleepTime(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_setBreakBeforeNextLine(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearBreakBeforeNextLine(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingInvoke(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_hasStartFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_shouldBreak(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
}

}  // namespace Svc
