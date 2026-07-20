// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include <cstdlib>

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/WasmSequencer/WasmSequencer_AllocErrorEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "config/FwAssertArgTypeAliasAc.h"
#include "config/WasmSequencerConfig.hpp"

namespace Svc {

namespace {
// FIXME(tumbar) This only allows a singleton WasmSequencer: spacewasm_set_global_allocator
//               installs a single process-wide backend, so only one instance may
//               register its pool at a time.
WasmSequencer* g_activeSequencer = nullptr;
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName)
    : WasmSequencerComponentBase(compName),
      m_page_used_mask(0),
      m_guest_offset(0),
      m_store(nullptr),
      m_maxModules(0),
      m_moduleIndex(0),
      m_executingStart(false),
      m_savedOpCode(0),
      m_savedCmdSeq(0),
      m_shouldRespond(false),
      m_hasPendingLoad(false),
      m_hasPendingInvoke(false),
      m_lastModuleHasStart(false),
      m_breakBeforeNextLine(false),
      m_loadFile(nullptr),
      m_lastTrap(SPACEWASM_TRAP_NONE) {
    FW_ASSERT(g_activeSequencer == nullptr);
    g_activeSequencer = this;

    // Install the process-wide global page allocator, backed by this instance's
    // page pool. The store/bytecode live entirely within m_memory_pool.
    const I32 status = spacewasm_set_global_allocator(&wasmSeqGlobalAlloc, &wasmSeqGlobalDealloc, this);
    FW_ASSERT(status == 0, status);
}

WasmSequencer ::~WasmSequencer() {
    this->destroyStore();
    g_activeSequencer = nullptr;
}

// ----------------------------------------------------------------------
// Interpreter store and page-backed allocators
// ----------------------------------------------------------------------

U8* WasmSequencer ::allocPage(U32 size, U32 align) {
    // The spacewasm PageAllocator only ever requests fixed-size pages of exactly
    // SPACEWASM_PAGE_SIZE, aligned no more than the pool's alignment.
    FW_ASSERT(size == Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE, static_cast<FwAssertArgType>(size));
    FW_ASSERT(align <= 16, static_cast<FwAssertArgType>(align));

    for (U32 page = 0; page < Svc::WasmSequencerConfig::SPACEWASM_MAX_PAGES; page++) {
        const U32 bit = static_cast<U32>(1) << page;
        if ((this->m_page_used_mask & bit) == 0) {
            this->m_page_used_mask |= bit;
            return &this->m_memory_pool[page * Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE];
        }
    }
    // Out of pages.
    return nullptr;
}

void WasmSequencer ::deallocPage(U8* ptr) {
    if (ptr == nullptr) {
        return;
    }
    const FwSizeType offset = static_cast<FwSizeType>(ptr - this->m_memory_pool);
    FW_ASSERT((offset % Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE) == 0, static_cast<FwAssertArgType>(offset));
    const U32 page = static_cast<U32>(offset / Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE);
    FW_ASSERT(page < Svc::WasmSequencerConfig::SPACEWASM_MAX_PAGES, static_cast<FwAssertArgType>(page));
    const U32 bit = static_cast<U32>(1) << page;
    FW_ASSERT((this->m_page_used_mask & bit) != 0);
    this->m_page_used_mask &= ~bit;
}

U8* WasmSequencer ::guestAlloc(U32 size, U32 align) {
    if (size == 0) {
        return nullptr;
    }
    // Round the current offset up to the requested alignment.
    const FwSizeType a = (align < 1) ? 1 : static_cast<FwSizeType>(align);
    FwSizeType start = (this->m_guest_offset + a - 1) & ~(a - 1);
    if (start + size > Svc::WasmSequencerConfig::GUEST_MEMORY_SIZE) {
        return nullptr;
    }
    this->m_guest_offset = start + size;
    return &this->m_guest_pool[start];
}

U8* WasmSequencer ::guestRealloc(U8* ptr, U32 oldSize, U32 newSize, U32 align) {
    // memory.grow is disabled at compile time, so realloc should not be hit in
    // practice. Serve it conservatively: a fresh block plus a copy.
    U8* fresh = this->guestAlloc(newSize, align);
    if (fresh != nullptr && ptr != nullptr) {
        const U32 copy = (oldSize < newSize) ? oldSize : newSize;
        for (U32 i = 0; i < copy; i++) {
            fresh[i] = ptr[i];
        }
    }
    return fresh;
}

void WasmSequencer ::guestDealloc(U8* ptr, U32 size) {
    // Bump allocator: individual frees are no-ops. The whole guest pool is reset
    // when a new store is created (destroyStore).
    (void)ptr;
    (void)size;
}

spacewasm_read_result_t WasmSequencer ::readModuleChunk(const U8** outBuf, std::size_t* outLen) {
    if (this->m_loadFile == nullptr) {
        *outLen = 0;
        return SPACEWASM_READ_ERROR;
    }

    Os::File* file = static_cast<Os::File*>(this->m_loadFile);
    FwSizeType size = static_cast<FwSizeType>(sizeof(this->m_readBuf));
    const Os::File::Status status = file->read(this->m_readBuf, size);
    if (status != Os::File::Status::OP_OK) {
        *outLen = 0;
        return SPACEWASM_READ_ERROR;
    }
    // `size` is updated in-place with the number of bytes actually read.
    *outBuf = this->m_readBuf;
    *outLen = static_cast<std::size_t>(size);
    return (size == 0) ? SPACEWASM_READ_EOF : SPACEWASM_READ_OK;
}

Fw::Success WasmSequencer ::createStore(U16 moduleCount) {
    this->destroyStore();

    spacewasm_host_t host;
    spacewasm_status_t status = spacewasm_host_new(0, &host);
    if (status != SPACEWASM_OK) {
        this->log_WARNING_HI_StoreAllocationFailed(
            moduleCount, Svc::WasmSequencer_AllocError(Svc::WasmSequencer_AllocError::AllocationFailed));
        return Fw::Success::FAILURE;
    }

    spacewasm_compiler_options_t options;
    options.allow_memory_grow = false;
    options.max_backpatch_iterations = 0;
    options.max_code_pages = Svc::WasmSequencerConfig::MAX_CODE_PAGES;

    status =
        spacewasm_store_new(&host, Svc::WasmSequencerConfig::GUEST_STACK_SIZE, moduleCount, options, &this->m_store);
    if (status != SPACEWASM_OK || this->m_store == nullptr) {
        this->m_store = nullptr;
        this->log_WARNING_HI_StoreAllocationFailed(moduleCount,
                                                   Svc::WasmSequencer_AllocError(WasmSequencer::mapAllocError(status)));
        return Fw::Success::FAILURE;
    }

    this->log_ACTIVITY_LO_StoreAllocationSucceeded(moduleCount);
    return Fw::Success::SUCCESS;
}

void WasmSequencer ::destroyStore() {
    if (this->m_store != nullptr) {
        spacewasm_store_destroy(this->m_store);
        this->m_store = nullptr;
    }
    // Reset the guest linear-memory bump allocator; all guest allocations were
    // owned by the store that just went away.
    this->m_guest_offset = 0;
}

Svc::WasmSequencer_AllocError::T WasmSequencer ::mapAllocError(spacewasm_status_t status) {
    switch (status) {
        case SPACEWASM_ERR_OUT_OF_MEMORY:
            return Svc::WasmSequencer_AllocError::OutOfMemory;
        case SPACEWASM_ERR_PAGE_TOO_SMALL:
            return Svc::WasmSequencer_AllocError::PageTooSmall;
        case SPACEWASM_ERR_ALLOC_FAILED:
        default:
            return Svc::WasmSequencer_AllocError::AllocationFailed;
    }
}

Svc::WasmSequencer_TrapReason::T WasmSequencer ::mapTrapReason(spacewasm_trap_t trap) {
    // spacewasm_trap_t values 0..14 map 1:1 onto the TrapReason enum ordinals.
    switch (trap) {
        case SPACEWASM_TRAP_UNREACHABLE:
            return Svc::WasmSequencer_TrapReason::Unreachable;
        case SPACEWASM_TRAP_HOST:
            return Svc::WasmSequencer_TrapReason::Host;
        case SPACEWASM_TRAP_DIVIDE_BY_ZERO:
            return Svc::WasmSequencer_TrapReason::DivideByZero;
        case SPACEWASM_TRAP_INVALID_TABLE_INDEX:
            return Svc::WasmSequencer_TrapReason::InvalidTableIndex;
        case SPACEWASM_TRAP_INVALID_TABLE_FUNCTION_TYPE:
            return Svc::WasmSequencer_TrapReason::InvalidTableFunctionType;
        case SPACEWASM_TRAP_UNINITIALIZED_TABLE_ELEMENT:
            return Svc::WasmSequencer_TrapReason::UninitializedTableElement;
        case SPACEWASM_TRAP_GLOBAL_GET_FAILED:
            return Svc::WasmSequencer_TrapReason::GlobalGetFailed;
        case SPACEWASM_TRAP_GLOBAL_SET_FAILED:
            return Svc::WasmSequencer_TrapReason::GlobalSetFailed;
        case SPACEWASM_TRAP_OUT_OF_MEMORY:
            return Svc::WasmSequencer_TrapReason::OutOfMemory;
        case SPACEWASM_TRAP_MEMORY_REF_NOT_UNIQUE:
            return Svc::WasmSequencer_TrapReason::MemoryRefNotUnique;
        case SPACEWASM_TRAP_MEMORY_OUT_OF_BOUNDS:
            return Svc::WasmSequencer_TrapReason::MemoryOutOfBounds;
        case SPACEWASM_TRAP_STACK_OVERFLOW:
            return Svc::WasmSequencer_TrapReason::StackOverflow;
        case SPACEWASM_TRAP_UNREPRESENTABLE_RESULT:
            return Svc::WasmSequencer_TrapReason::UnrepresentableResult;
        case SPACEWASM_TRAP_INTEGER_OVERFLOW:
            return Svc::WasmSequencer_TrapReason::IntegerOverflow;
        case SPACEWASM_TRAP_BAD_CONVERSION_TO_INTEGER:
            return Svc::WasmSequencer_TrapReason::BadConversionToInteger;
        default:
            return Svc::WasmSequencer_TrapReason::Host;
    }
}

// ----------------------------------------------------------------------
// C ABI trampolines imported by the spacewasm_c_api crate
// ----------------------------------------------------------------------

extern "C" U8* wasmSeqGlobalAlloc(void* userdata, std::size_t size, std::size_t align) {
    if (userdata == nullptr) {
        return nullptr;
    }
    return static_cast<WasmSequencer*>(userdata)->allocPage(static_cast<U32>(size), static_cast<U32>(align));
}

extern "C" void wasmSeqGlobalDealloc(void* userdata, U8* ptr, std::size_t size, std::size_t align) {
    (void)size;
    (void)align;
    if (userdata != nullptr) {
        static_cast<WasmSequencer*>(userdata)->deallocPage(ptr);
    }
}

extern "C" U8* wasmSeqGuestAlloc(void* userdata, std::size_t size, std::size_t align) {
    if (userdata == nullptr) {
        return nullptr;
    }
    return static_cast<WasmSequencer*>(userdata)->guestAlloc(static_cast<U32>(size), static_cast<U32>(align));
}

extern "C" U8* wasmSeqGuestRealloc(void* userdata,
                                   U8* ptr,
                                   std::size_t oldSize,
                                   std::size_t newSize,
                                   std::size_t align) {
    if (userdata == nullptr) {
        return nullptr;
    }
    return static_cast<WasmSequencer*>(userdata)->guestRealloc(ptr, static_cast<U32>(oldSize),
                                                               static_cast<U32>(newSize), static_cast<U32>(align));
}

extern "C" void wasmSeqGuestDealloc(void* userdata, U8* ptr, std::size_t size, std::size_t align) {
    (void)align;
    if (userdata != nullptr) {
        static_cast<WasmSequencer*>(userdata)->guestDealloc(ptr, static_cast<U32>(size));
    }
}

extern "C" spacewasm_read_result_t wasmSeqReadModule(void* userdata, const U8** outBuf, std::size_t* outLen) {
    if (userdata == nullptr) {
        *outLen = 0;
        return SPACEWASM_READ_ERROR;
    }
    return static_cast<WasmSequencer*>(userdata)->readModuleChunk(outBuf, outLen);
}

//! Panic hook the spacewasm interpreter calls on a fatal internal error. Must
//! not return. Signature matches the extern declaration in spacewasm.h exactly.
extern "C" void spacewasm_panic(const U8* filename,
                                std::size_t filename_len,
                                U32 line,
                                const U8* msg,
                                std::size_t len) {
    (void)filename;
    (void)filename_len;
    (void)line;
    (void)msg;
    (void)len;
    // TODO(tumbar) Emit a WARNING_HI event and reset the Rust state gracefully.
    FW_ASSERT(false);
    // FW_ASSERT should not return, but abort defensively to satisfy the
    // never-returns contract.
    abort();
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::checkTimers_handler(FwIndexType portNum, U32 context) {
    // Drive the sleep-wake / statement-timeout checks in the state machine.
    this->sequencer_sendSignal_checkTimersIn();
}

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    // TODO(spacewasm lacks host-call resume-with-result primitives): the
    // RUNNING.AWAITING_RESPONSE path (dispatching an F´ command from a paused
    // host function and feeding the response back to guest code) is out of
    // scope for this pass. Any response that arrives here is unexpected; nudge
    // the state machine so a stray response cannot wedge a running sequence.
    this->sequencer_sendSignal_stmtResponse_unexpected();
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void WasmSequencer ::RUN_cmdHandler(FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    Svc::BlockState block) {
    // RUN is only valid from IDLE.
    if (this->sequencer_getState() != Svc_WasmSequencer_SequencerStateMachine::State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(this->sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    if (block == Svc::BlockState::BLOCK) {
        // Defer the response until the run reaches a terminal state.
        this->m_savedOpCode = opCode;
        this->m_savedCmdSeq = cmdSeq;
    }

    this->sequencer_sendSignal_cmd_RUN(Svc::WasmSequencer_SequenceExecutionArgs(fileName, block, Svc::SeqArgs{0, 0}));

    if (block == Svc::BlockState::NO_BLOCK) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO(tumbar) WAIT should block until the current run finishes. Without
    // the AWAITING_RESPONSE machinery there is nothing to wait on yet.
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::LOAD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& fileName) {
    // LOAD uses an empty module name (single unnamed module).
    this->LOAD_NAME_cmdHandler(opCode, cmdSeq, fileName, Fw::CmdStringArg(""));
}

void WasmSequencer ::LOAD_NAME_cmdHandler(FwOpcodeType opCode,
                                          U32 cmdSeq,
                                          const Fw::CmdStringArg& fileName,
                                          const Fw::CmdStringArg& name) {
    // Loading is only valid from IDLE.
    if (this->sequencer_getState() != Svc_WasmSequencer_SequencerStateMachine::State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(this->sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // A plain load always defers its response: OK once the module reaches READY,
    // EXECUTION_ERROR on load failure.
    this->m_savedOpCode = opCode;
    this->m_savedCmdSeq = cmdSeq;

    this->sequencer_sendSignal_cmd_LOAD(Svc::WasmSequencer_ModuleLoad(fileName, name));
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Fw::CmdStringArg& functionName) {
    // INVOKE is only valid from READY.
    if (this->sequencer_getState() != Svc_WasmSequencer_SequencerStateMachine::State::READY) {
        this->log_WARNING_LO_InvalidCommand(static_cast<I32>(this->sequencer_getState()));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // Defer the response until the invoked function reaches a terminal state.
    this->m_savedOpCode = opCode;
    this->m_savedCmdSeq = cmdSeq;

    this->sequencer_sendSignal_cmd_INVOKE(Svc::WasmSequencer_FunctionInvoke(module, functionName));
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Cancel is a no-op in IDLE; otherwise it aborts back to IDLE.
    this->sequencer_sendSignal_cmd_CANCEL();
    // Cancel returns immediately and always succeeds.
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::BREAK_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->sequencer_sendSignal_cmd_BREAK();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::TRACE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO(tumbar) Dump a stack trace as an event once the interpreter exposes one.
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->sequencer_sendSignal_cmd_CONTINUE();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->sequencer_sendSignal_entered();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_createStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->createStore(this->m_maxModules);
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // Recreate a fresh store with the same module capacity, clearing all
    // loaded modules, tables and memories. Also clear pending run/load state.
    this->m_hasPendingLoad = false;
    this->m_hasPendingInvoke = false;
    this->m_lastModuleHasStart = false;
    this->m_breakBeforeNextLine = false;
    this->m_executingStart = false;
    this->createStore(this->m_maxModules);
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invoke(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_store == nullptr) {
        this->sequencer_sendSignal_interpreterTrap(Svc::WasmSequencer_TrapReason::Host);
        return;
    }

    const Fw::String functionName(this->m_pendingInvoke.get_functionName());

    // Resolve the exported function in the most-recently-loaded module.
    U32 funcIndex = 0;
    const spacewasm_status_t findStatus =
        spacewasm_store_find_export_func(this->m_store, this->m_moduleIndex, functionName.toChar(), &funcIndex);

    // This invocation is not a start function.
    this->m_executingStart = false;
    this->m_hasPendingInvoke = false;

    if (findStatus != SPACEWASM_OK) {
        this->sequencer_sendSignal_interpreterTrap(Svc::WasmSequencer_TrapReason::Host);
        return;
    }

    // No arguments are passed from the command interface.
    const spacewasm_status_t invokeStatus =
        spacewasm_store_invoke(this->m_store, this->m_moduleIndex, funcIndex, nullptr, 0);
    if (invokeStatus != SPACEWASM_OK) {
        this->sequencer_sendSignal_interpreterTrap(Svc::WasmSequencer_TrapReason::Host);
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pendRun(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_SequenceExecutionArgs& value) {
    // RUN == load the file (unnamed) then invoke its "main".
    this->m_pendingLoad = Svc::WasmSequencer_ModuleLoad(value.get_filePath(), Fw::String(""));
    this->m_hasPendingLoad = true;

    this->m_pendingInvoke = Svc::WasmSequencer_FunctionInvoke(Fw::String(""), Fw::String("main"));
    this->m_hasPendingInvoke = true;

    // Whether we defer the command response is decided by the block state saved
    // in the command handler (m_savedOpCode/Seq only set when BLOCK).
    this->m_shouldRespond = (value.get_block() == Svc::BlockState::BLOCK);
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pendLoad(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleLoad& value) {
    this->m_pendingLoad = value;
    this->m_hasPendingLoad = true;
    // A plain load has no pending invoke; it replies OK on reaching READY.
    this->m_hasPendingInvoke = false;
    this->m_shouldRespond = true;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_appendModuleToStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_Module& value) {
    // The module was already pushed into the store by spacewasm_store_load_module;
    // record whether it declared a start function for the SUCCESS choice.
    this->m_lastModuleHasStart = value.get_hasStart();
    this->m_moduleIndex = value.get_moduleIndex();
    this->m_hasPendingLoad = false;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_reportLoadFailure(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_LoadFailure& value) {
    this->log_WARNING_HI_ModuleLoadFailed(value.get_status());
    this->m_hasPendingLoad = false;
    this->m_hasPendingInvoke = false;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_store == nullptr) {
        // No store to load into; report a synthetic failure.
        this->sequencer_sendSignal_loadFailed(Svc::WasmSequencer_LoadFailure(SPACEWASM_ERR_NULL_ARG));
        return;
    }

    const Fw::String filePath(this->m_pendingLoad.get_fileName());
    const Fw::String moduleName(this->m_pendingLoad.get_moduleName());

    Os::File file;
    const Os::File::Status openStatus = file.open(filePath.toChar(), Os::File::OPEN_READ);
    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(filePath, static_cast<I32>(openStatus));
        this->sequencer_sendSignal_loadFailed(Svc::WasmSequencer_LoadFailure(SPACEWASM_ERR_STREAM));
        return;
    }

    // Expose the open file to the streaming read trampoline for the duration of
    // this decode.
    this->m_loadFile = &file;

    // A per-load guest linear-memory allocator. Backed by m_guest_pool; released
    // immediately after load (the module retains its own reference).
    spacewasm_allocator_t* alloc =
        spacewasm_allocator_new(&wasmSeqGuestAlloc, &wasmSeqGuestRealloc, &wasmSeqGuestDealloc, this);

    U32 moduleIndex = 0;
    const spacewasm_status_t status =
        spacewasm_store_load_module(this->m_store, moduleName.toChar(), &wasmSeqReadModule, this, alloc, &moduleIndex);

    spacewasm_allocator_destroy(alloc);
    this->m_loadFile = nullptr;
    file.close();

    if (status != SPACEWASM_OK) {
        this->sequencer_sendSignal_loadFailed(Svc::WasmSequencer_LoadFailure(static_cast<I32>(status)));
        return;
    }

    bool needsStart = false;
    const spacewasm_status_t startStatus = spacewasm_store_module_needs_start(this->m_store, moduleIndex, &needsStart);
    if (startStatus != SPACEWASM_OK) {
        this->sequencer_sendSignal_loadFailed(Svc::WasmSequencer_LoadFailure(static_cast<I32>(startStatus)));
        return;
    }

    this->sequencer_sendSignal_loadSucceeded(Svc::WasmSequencer_Module(static_cast<U16>(moduleIndex), needsStart));
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invokeStart(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_store == nullptr) {
        this->sequencer_sendSignal_interpreterTrap(Svc::WasmSequencer_TrapReason::Host);
        return;
    }
    // The first `spin` seeds and drives the start function via
    // spacewasm_store_run_start; just flag that we are executing it.
    this->m_executingStart = true;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_store == nullptr) {
        this->sequencer_sendSignal_interpreterTrap(Svc::WasmSequencer_TrapReason::Host);
        return;
    }

    spacewasm_trap_t trap = SPACEWASM_TRAP_NONE;
    spacewasm_run_status_t runStatus;
    if (this->m_executingStart) {
        runStatus = spacewasm_store_run_start(this->m_store, this->m_moduleIndex,
                                              Svc::WasmSequencerConfig::INSTRUCTION_FUEL, &trap);
    } else {
        runStatus = spacewasm_store_run(this->m_store, Svc::WasmSequencerConfig::INSTRUCTION_FUEL, &trap);
    }
    this->m_lastTrap = trap;

    switch (runStatus) {
        case SPACEWASM_RUN_FINISHED:
            this->sequencer_sendSignal_interpreterFinished();
            break;
        case SPACEWASM_RUN_TRAP:
            this->sequencer_sendSignal_interpreterTrap(WasmSequencer::mapTrapReason(trap));
            break;
        case SPACEWASM_RUN_PAUSE:
            this->sequencer_sendSignal_interpreterPause();
            break;
        case SPACEWASM_RUN_OUT_OF_FUEL:
            this->sequencer_sendSignal_interpreterOutOfFuel();
            break;
        case SPACEWASM_RUN_READER_ERROR:
            this->sequencer_sendSignal_interpreterReaderError();
            break;
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(runStatus));
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequenceSucceeded();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequenceCancelled();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_WARNING_HI_SequenceFailed();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqTrap(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_WARNING_HI_SequenceTrap(WasmSequencer::mapTrapReason(this->m_lastTrap));
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqReadError(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_WARNING_HI_SequenceReadError();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqBroken(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequenceBroken();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_OK(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_shouldRespond) {
        this->cmdResponse_out(this->m_savedOpCode, this->m_savedCmdSeq, Fw::CmdResponse::OK);
        this->m_shouldRespond = false;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendCmdResponse_EXECUTION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_shouldRespond) {
        this->cmdResponse_out(this->m_savedOpCode, this->m_savedCmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        this->m_shouldRespond = false;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_set_sleepTime(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO(spacewasm lacks time/sleep primitives): the SLEEPING state cannot be
    // realized against the crate as it stands.
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO(tumbar): no statement timeout to
    // check yet. Report "no timeout" so the SM does not wedge waiting on a check.
    this->sequencer_sendSignal_result_checkStatementTimeout_noTimeout();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO(tumbar) nothing sleeps yet. Report
    // "keep sleeping" so a stray SLEEPING state does not spuriously wake.
    this->sequencer_sendSignal_result_checkShouldWake_keepSleeping();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_setBreakBeforeNextLine(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->m_breakBeforeNextLine = true;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearBreakBeforeNextLine(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->m_breakBeforeNextLine = false;
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingInvoke(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_hasPendingInvoke;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingLoad(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_hasPendingLoad;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_lastModuleHasStart(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_lastModuleHasStart;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_shouldBreak(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_breakBeforeNextLine;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_executingStart(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_executingStart;
}

}  // namespace Svc
