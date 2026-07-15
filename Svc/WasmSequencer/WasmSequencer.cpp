// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencerFFI.hpp"
#include "Svc/WasmSequencer/WasmSequencer_AllocErrorEnumAc.hpp"
#include "Svc/WasmSequencer/fprime_spacewasm/fprime_spacewasm.h"
#include "config/FwAssertArgTypeAliasAc.h"
#include "config/WasmSequencerConfig.hpp"

namespace Svc {

namespace {
WasmSequencer* g_activeSequencer = nullptr;
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName)
    : WasmSequencerComponentBase(compName), m_store(nullptr), m_maxModules(0), m_memory_allocated(false) {
    // FIXME(tumbar) This only allows a singleton WasmSequencer
    //               We should make this thread local in a way that is cross platform?
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
    // TODO(tumbar) It's possible to have a fancier memory management system
    // The Rust PageAllocator should only ever request the entire memory pool

    FW_ASSERT(size == Svc::WasmSequencerConfig::DYNAMIC_MEMORY_SIZE, static_cast<FwAssertArgType>(size),
              Svc::WasmSequencerConfig::DYNAMIC_MEMORY_SIZE);

    // Make sure memory is not already allocated
    // This _is_ a programming error since [spacewasm::PageAllocator] has a hardcoded max-pages=1 in fprime_spacewasm
    FW_ASSERT(!this->m_memory_allocated);

    this->m_memory_allocated = true;

    // Allocation succeeded.
    *result = AllocResult::Ok;
    return this->m_memory_pool;
}

void WasmSequencer ::deallocPage(U8* ptr, U32 size) {
    (void)size;
    if (ptr == nullptr) {
        return;
    }

    // Make sure we are actually deallocating the memory
    FW_ASSERT(this->m_memory_allocated);

    // Make sure the deallocation is the expected size
    FW_ASSERT(size == Svc::WasmSequencerConfig::DYNAMIC_MEMORY_SIZE, static_cast<FwAssertArgType>(size),
              Svc::WasmSequencerConfig::DYNAMIC_MEMORY_SIZE);

    this->m_memory_allocated = false;
}

Fw::Success WasmSequencer ::createStore(U16 moduleCount) {
    this->destroyStore();
    const U32 status = fprime_spacewasm_store_new(moduleCount, &this->m_store);
    Fw::Success success;
    if (this->m_store == nullptr) {
        // Allocation failed; leave the store null.
        this->log_WARNING_HI_StoreAllocationFailed(
            moduleCount, Svc::WasmSequencer_AllocError(static_cast<Svc::WasmSequencer_AllocError::T>(status)));
        success = Fw::Success::FAILURE;
    } else {
        this->log_ACTIVITY_LO_StoreAllocationSucceeded(moduleCount);
        success = Fw::Success::SUCCESS;
    }

    return success;
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
    auto status = this->createStore(module_count);
    switch (status) {
        case Fw::Success::FAILURE:
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        case Fw::Success::SUCCESS:
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            break;
    }
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
    return false;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_hasStartFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
    return false;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_shouldBreak(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    // TODO
    return false;
}

}  // namespace Svc
