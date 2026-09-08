// ======================================================================
// \title  WasmSequencerHelpers.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class helpers
// ======================================================================

#include <cstddef>
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/StringBase.hpp"
#include "Os/Console.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_MemoryGrowFailReasonEnumAc.hpp"
#include "Svc/WasmSequencer/fprime_spacewasm/include/fprime_spacewasm.h"
#include "config/FwAssertArgTypeAliasAc.h"
#include "config/FwSizeTypeAliasAc.h"
#include "config/WasmSequencerConfig.hpp"
#include "spacewasm.h"

namespace Svc {
// ----------------------------------------------------------------------
// Interpreter store and page-backed allocators
// ----------------------------------------------------------------------

U8* WasmSequencer ::globalAlloc(const U32 size, const U32 align) {
    // The spacewasm PageAllocator only ever requests fixed-size pages of exactly
    // SPACEWASM_PAGE_SIZE, aligned no more than the pool's alignment.
    FW_ASSERT(size == Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE, static_cast<FwAssertArgType>(size));
    FW_ASSERT(align <= 8, static_cast<FwAssertArgType>(align));
    FW_ASSERT(!this->m_heapPoisoned);

    if (this->m_heapPagesUsed < this->m_config.heapPages) {
        auto page = this->m_heapPages[this->m_heapPagesUsed];
        FW_ASSERT(page != nullptr);
        this->m_heapPagesUsed += 1;
        return page;
    } else {
        // Out of pages.
        return nullptr;
    }
}

void WasmSequencer ::globalDealloc(const U8* ptr) {
    if (ptr == nullptr) {
        return;
    }

    // Make sure the pointer that was given back to us is ours
    bool found = false;
    for (FwSizeType i = 0; i < this->m_config.heapPages; i++) {
        if (ptr == this->m_heapPages[i]) {
            found = true;
            break;
        }
    }

    FW_ASSERT(found);

    // Decrement the number of pages used.
    // Deallocation only happens when the store is being destroyed.
    // We will assert that the used page count drops to zero once store destruction completes
    FW_ASSERT(this->m_heapPagesUsed > 0);
    this->m_heapPagesUsed -= 1;
    this->m_heapPoisoned = true;
}

U8* WasmSequencer ::guestAlloc(FwSizeType size, U32 align) {
    if (size == 0) {
        return nullptr;
    }

    // Reject any request that cannot possibly fit the guest pool up front.
    if (size > this->m_config.guestMemorySize || align > SPACEWASM_MEMORY_ALIGNMENT) {
        return nullptr;
    }

    // Round the current offset up to the requested alignment
    const FwSizeType a = (align < 1) ? 1 : static_cast<FwSizeType>(align);
    const FwSizeType start = (this->m_guestPoolOffset + a - 1) & ~(a - 1);

    // Compare against the pre-subtracted bound so `start + size` cannot overflow.
    // `size <= this->m_config.guestMemorySize` (checked above) makes the subtraction non-negative.
    if (start > this->m_config.guestMemorySize - size) {
        return nullptr;
    }
    this->m_guestPoolOffset = start + size;
    return &this->m_guestPool[start];
}

U8* WasmSequencer ::guestRealloc(U8* ptr, FwSizeType oldSize, FwSizeType newSize, U32 align) {
    // We have received a memory.grow request.
    // The following MUST be true for us to actually give the guest it's memory (otherwise fail):
    // 1. This linear memory is the final allocated memory in the guest bump allocator
    // 2. We have enough space free to actually do this request
    // 3. You buy me a donut.

    std::ptrdiff_t moduleMemOffset = ptr - this->m_guestPool;

    // The pointer must lie within our guest pool.
    FW_ASSERT(moduleMemOffset >= 0, static_cast<FwAssertArgType>(moduleMemOffset));
    const FwSizeType offset = static_cast<FwSizeType>(moduleMemOffset);

    // Check 1. i.e. the pointer is what we expect given old_size and current guest offset
    if (offset + oldSize == this->m_guestPoolOffset) {
        // Check 2. We have enough space in the guest pool to service this request
        if (offset + newSize <= this->m_config.guestMemorySize) {
            // ...now the donuts
            // Allocate the new guest memory size
            this->m_guestPoolOffset = offset + newSize;

            // We return the same pointer since this is a strict grow and
            // we already reserved the slot in front of this memory
            return ptr;
        }

        // We are the last allocation, but the grown size does not fit the guest pool.
        this->log_WARNING_LO_MemoryGrowRejected(WasmSequencer_MemoryGrowFailReason::INSUFFICIENT_POOL_SPACE,
                                                static_cast<U64>(newSize));
        return nullptr;
    }

    // We are not the last allocation in the bump pool, so we cannot grow in place.
    this->log_WARNING_LO_MemoryGrowRejected(WasmSequencer_MemoryGrowFailReason::NOT_LAST_ALLOCATION,
                                            static_cast<U64>(newSize));
    return nullptr;
}

void WasmSequencer ::guestDealloc(const U8* ptr, const FwSizeType size) {
    // Bump allocator: individual frees are no-ops. The whole guest pool is reset
    // when a new store is created (destroyStore).
    (void)ptr;
    (void)size;
}

U8* WasmSequencer ::guestAllocCallback(void* userdata, size_t size, size_t align) {
    FW_ASSERT(userdata != nullptr);
    return static_cast<WasmSequencer*>(userdata)->guestAlloc(static_cast<FwSizeType>(size), static_cast<U32>(align));
}

U8* WasmSequencer ::guestReallocCallback(void* userdata, U8* ptr, size_t old_size, size_t new_size, size_t align) {
    FW_ASSERT(userdata != nullptr);
    // Do NOT narrow old_size/new_size to U32: a 4 GiB grow (new_size == 2^32) would alias to 0 and
    // be accepted as a no-op grow. Pass full width; guestRealloc's fits-pool check rejects it.
    return static_cast<WasmSequencer*>(userdata)->guestRealloc(
        ptr, static_cast<FwSizeType>(old_size), static_cast<FwSizeType>(new_size), static_cast<U32>(align));
}

void WasmSequencer ::guestDeallocCallback(void* userdata, U8* ptr, size_t size, size_t align) {
    FW_ASSERT(userdata != nullptr);
    (void)align;
    static_cast<WasmSequencer*>(userdata)->guestDealloc(ptr, static_cast<FwSizeType>(size));
}

void WasmSequencer ::createStore() {
    FW_ASSERT(this->m_wasm == nullptr);

    this->takeAllocatorLock();

    spacewasm_host_t host;
    spacewasm_status_t status = spacewasm_host_new(1, &host);
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->hostFprimeV1(&host);

    spacewasm_compiler_options_t options;
    options.allow_memory_grow = true;  // implemented in a restricted way (see guestRealloc)
    options.max_backpatch_iterations = Svc::WasmSequencerConfig::MAX_BACKPATCH_ITERATIONS;
    options.max_code_pages = this->m_config.maxCodePages;

    status = spacewasm_new(&host, this->m_config.stackSize, this->m_config.maxGuestModules, options, &this->m_wasm);

    this->releaseAllocatorLock();

    // Make sure the store allocation succeeded.
    // Failure means the heap memory is too small to host this number of modules + Wasm stack...
    //
    // If status == SPACEWASM_ERR_PAGE_TOO_SMALL:
    // - Increase Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE
    // If SPACEWASM_ERR_OUT_OF_MEMORY / SPACEWASM_ERR_ALLOC_FAILED:
    // - Increase heapPages in configure()
    // - Lower maxGuestModules in configure()
    // - Lower stackSize in configure()
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->log_DIAGNOSTIC_StoreAllocationSucceeded(this->m_config.maxGuestModules);
}

void WasmSequencer ::destroyStore() {
    FW_ASSERT(this->m_wasm != nullptr);

    this->takeAllocatorLock();
    spacewasm_destroy(this->m_wasm);
    this->releaseAllocatorLock();
    this->m_wasm = nullptr;

    // Make sure we cleanly deallocated all the heap memory
    FW_ASSERT(this->m_heapPagesUsed == 0, static_cast<FwAssertArgType>(this->m_heapPagesUsed));

    // Reset the guest linear-memory bump allocator; all guest allocations were
    // owned by the store that just went away.
    this->m_guestPoolOffset = 0;
    this->m_heapPoisoned = false;
}

spacewasm_status_t WasmSequencer ::validateModuleMain(WasmSequencer_ModuleIdx moduleIdx) const {
    FW_ASSERT(this->m_wasm != nullptr);

    U32 mainIndex;
    auto status = spacewasm_find_export_func(this->m_wasm, static_cast<U32>(moduleIdx), "main", &mainIndex);

    if (status == SPACEWASM_OK) {
        // We accept both the [] -> [] and [] -> i32 main signatures. Checking
        // against "" first yields PARAM_LEN_MISMATCH (not BAD_SIGNATURE) for an
        // i32-returning main -- BAD_SIGNATURE only flags a malformed signature
        // *string* -- so fall back on any mismatch, not just BAD_SIGNATURE.
        status = spacewasm_check_func_signature(this->m_wasm, static_cast<U32>(moduleIdx), mainIndex, "", "");
        if (status != SPACEWASM_OK) {
            status = spacewasm_check_func_signature(this->m_wasm, static_cast<U32>(moduleIdx), mainIndex, "", "i");
        }
    }

    return status;
}

U32 WasmSequencer ::makeCmdUid() const {
    // cmdUid is formatted XXYY, where XX are the low 16 bits of m_sequencesStarted
    // and YY are the low 16 bits of m_tlm.commandsDispatched. On the way back in via
    // cmdResponseIn this lets us check A) that the response is from the current
    // sequence (modulo 2^16) and B) that it is this exact command instance and not
    // another dispatch of the same opcode.
    return static_cast<U32>(((this->m_sequencesStarted & 0xFFFF) << 16) | (this->m_tlm.commandsDispatched & 0xFFFF));
}

void WasmSequencer ::respondToRequest(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response) {
    switch (value.get_source()) {
        case WasmSequencer_SignalSource::COMMAND_RUN:
        case WasmSequencer_SignalSource::COMMAND_INVOKE:
        case WasmSequencer_SignalSource::COMMAND_LOAD:
            // The request originated from a command; answer it on cmdResponse.
            this->cmdResponse_out(value.get_cmdCtx().get_opcode(), value.get_cmdCtx().get_cmdSeq(), response);
            break;
        case WasmSequencer_SignalSource::PORT_RUN:
        case WasmSequencer_SignalSource::PORT_INVOKE:
            // Port-driven requests have no command response to send.
            break;
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(value.get_source()));
            break;
    }
}

void WasmSequencer ::respondToWaiting(const Fw::CmdResponse& response) {
    // Drain every WAIT command blocked on sequence completion, answering each.
    WaitingCmd cmd{};
    while (this->m_waiting.dequeue(cmd) == Fw::Success::SUCCESS) {
        this->cmdResponse_out(cmd.opCode, cmd.cmdSeq, response);
    }
}

void WasmSequencer ::reportSeqDone(const Svc::WasmSequencer_RequestContext& value, const Fw::CmdResponse& response) {
    // seqStart/seqDone are RUN-scoped: seqStartOut is only emitted for RUN sources
    // (see reportModuleStarted), so only emit the matching seqDoneOut for those.
    // A non-RUN completion (INVOKE / LOAD) reports neither, keeping the pair balanced.
    if (value.get_source() != Svc::WasmSequencer_SignalSource::COMMAND_RUN &&
        value.get_source() != Svc::WasmSequencer_SignalSource::PORT_RUN) {
        return;
    }
    if (this->isConnected_seqDoneOut_OutputPort(0)) {
        this->seqDoneOut_out(0, 0, 0, response);
    }
}

spacewasm_status_t WasmSequencer ::setGlobal(const Fw::StringBase& moduleName,
                                             const Fw::StringBase& name,
                                             spacewasm_value_t value) {
    U32 moduleIdx = 0;
    spacewasm_status_t status = spacewasm_find_module(this->m_wasm, moduleName.toChar(), &moduleIdx);
    if (status != SPACEWASM_OK) {
        return status;
    }

    U32 globalIdx;
    status = spacewasm_find_global(this->m_wasm, moduleIdx, name.toChar(), &globalIdx);
    if (status != SPACEWASM_OK) {
        return status;
    }

    return spacewasm_set_global(this->m_wasm, moduleIdx, globalIdx, value);
}

spacewasm_status_t WasmSequencer ::getGlobal(const Fw::StringBase& moduleName,
                                             const Fw::StringBase& name,
                                             spacewasm_value_t& value) {
    U32 moduleIdx = 0;
    spacewasm_status_t status = spacewasm_find_module(this->m_wasm, moduleName.toChar(), &moduleIdx);
    if (status != SPACEWASM_OK) {
        return status;
    }

    U32 globalIdx;
    status = spacewasm_find_global(this->m_wasm, moduleIdx, name.toChar(), &globalIdx);
    if (status != SPACEWASM_OK) {
        return status;
    }

    return spacewasm_get_global(this->m_wasm, moduleIdx, globalIdx, &value);
}

Svc::WasmSequencer_TrapReason::T WasmSequencer ::mapTrapReason(spacewasm_trap_t trap) {
    // spacewasm_trap_t values 0..14 map 1:1 onto the TrapReason enum ordinals.
    switch (trap) {
        case SPACEWASM_TRAP_UNREACHABLE:
            return Svc::WasmSequencer_TrapReason::UNREACHABLE;
        case SPACEWASM_TRAP_HOST:
            return Svc::WasmSequencer_TrapReason::HOST;
        case SPACEWASM_TRAP_DIVIDE_BY_ZERO:
            return Svc::WasmSequencer_TrapReason::DIVIDE_BY_ZERO;
        case SPACEWASM_TRAP_INVALID_TABLE_INDEX:
            return Svc::WasmSequencer_TrapReason::INVALID_TABLE_INDEX;
        case SPACEWASM_TRAP_INVALID_TABLE_FUNCTION_TYPE:
            return Svc::WasmSequencer_TrapReason::INVALID_TABLE_FUNCTION_TYPE;
        case SPACEWASM_TRAP_UNINITIALIZED_TABLE_ELEMENT:
            return Svc::WasmSequencer_TrapReason::UNINITIALIZED_TABLE_ELEMENT;
        case SPACEWASM_TRAP_GLOBAL_GET_FAILED:
            return Svc::WasmSequencer_TrapReason::GLOBAL_GET_FAILED;
        case SPACEWASM_TRAP_GLOBAL_SET_FAILED:
            return Svc::WasmSequencer_TrapReason::GLOBAL_SET_FAILED;
        case SPACEWASM_TRAP_OUT_OF_MEMORY:
            return Svc::WasmSequencer_TrapReason::OUT_OF_MEMORY;
        case SPACEWASM_TRAP_MEMORY_REF_NOT_UNIQUE:
            return Svc::WasmSequencer_TrapReason::MEMORY_REF_NOT_UNIQUE;
        case SPACEWASM_TRAP_MEMORY_OUT_OF_BOUNDS:
            return Svc::WasmSequencer_TrapReason::MEMORY_OUT_OF_BOUNDS;
        case SPACEWASM_TRAP_STACK_OVERFLOW:
            return Svc::WasmSequencer_TrapReason::STACK_OVERFLOW;
        case SPACEWASM_TRAP_UNREPRESENTABLE_RESULT:
            return Svc::WasmSequencer_TrapReason::UNREPRESENTABLE_RESULT;
        case SPACEWASM_TRAP_INTEGER_OVERFLOW:
            return Svc::WasmSequencer_TrapReason::INTEGER_OVERFLOW;
        case SPACEWASM_TRAP_BAD_CONVERSION_TO_INTEGER:
            return Svc::WasmSequencer_TrapReason::BAD_CONVERSION_TO_INTEGER;
        default:
            return Svc::WasmSequencer_TrapReason::HOST;
    }
}

void WasmSequencer ::setSequenceName(const Fw::StringBase& filePath, const Fw::StringBase& moduleName) {
    // A non-empty module name was supplied to LOAD; use it verbatim.
    if (moduleName.length() > 0) {
        this->m_tlm.sequenceName = moduleName;
        return;
    }

    // RUN / LOAD: derive from the file's basename with any ".wasm" suffix stripped.
    const char* const path = filePath.toChar();
    const FwSizeType len = static_cast<FwSizeType>(filePath.length());

    // Find the start of the basename (character after the last '/').
    FwSizeType nameLen = 0;
    const char* const base = WasmSequencer::pathBaseName(path, len, nameLen);

    // Drop a trailing ".wasm" if present.
    static const char suffix[] = ".wasm";
    const FwSizeType suffixLen = static_cast<FwSizeType>(sizeof(suffix) - 1);
    if (nameLen >= suffixLen) {
        bool match = true;
        for (FwSizeType i = 0; i < suffixLen; i++) {
            if (base[nameLen - suffixLen + i] != suffix[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            nameLen -= suffixLen;
        }
    }

    char name[FileNameStringSize];
    FwSizeType n = 0;
    for (FwSizeType i = 0; i < nameLen && n < static_cast<FwSizeType>(sizeof(name) - 1); i++) {
        name[n++] = base[i];
    }
    name[n] = '\0';
    this->m_tlm.sequenceName = name;
}

const char* WasmSequencer ::pathBaseName(const char* path, FwSizeType len, FwSizeType& outLen) {
    FW_ASSERT(path != nullptr);
    // Basename starts just after the last '/', or at the start if there is none.
    FwSizeType start = 0;
    for (FwSizeType i = 0; i < len; i++) {
        if (path[i] == '/') {
            start = i + 1;
        }
    }
    outLen = len - start;
    return path + start;
}

bool WasmSequencer ::pathHasParentTraversal(const Fw::StringBase& path) {
    const char* const s = path.toChar();
    const FwSizeType len = static_cast<FwSizeType>(path.length());

    // Walk the '/'-delimited segments; reject if any segment is exactly "..".
    FwSizeType segStart = 0;
    for (FwSizeType i = 0; i <= len; i++) {
        if (i == len || s[i] == '/') {
            const FwSizeType segLen = i - segStart;
            if (segLen == 2 && s[segStart] == '.' && s[segStart + 1] == '.') {
                return true;
            }
            segStart = i + 1;
        }
    }
    return false;
}

Os::Mutex* WasmSequencer::getGlobalAllocatorLock() {
    //! Process-wide lock serializing access to the spacewasm global-allocator registry.
    //! Initialized by the first component instance's constructor
    static Os::Mutex s_globalAllocatorLock;
    return &s_globalAllocatorLock;
}

void WasmSequencer ::takeAllocatorLock() {
    getGlobalAllocatorLock()->lock();

    auto status = spacewasm_fprime_acquire_global_allocator(this);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::releaseAllocatorLock() {
    auto status = spacewasm_fprime_release_global_allocator(this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    getGlobalAllocatorLock()->unlock();
}

//! Panic hook the spacewasm interpreter calls on a fatal internal error.
// Must not return.
extern "C" void spacewasm_panic(const U8* filename,
                                std::size_t filename_len,
                                U32 line,
                                const U8* msg,
                                std::size_t len) {
    Fw::String fmtMsg;
    (void)fmtMsg.format("Rust panic %.*s:%d: %.*s\n", static_cast<int>(filename_len),
                        reinterpret_cast<const char*>(filename), static_cast<int>(line), static_cast<int>(len),
                        reinterpret_cast<const char*>(msg));
    Os::Console::write(fmtMsg);

    // Rust panics map to FSW assertions
    FW_ASSERT(false);
}

}  // namespace Svc
