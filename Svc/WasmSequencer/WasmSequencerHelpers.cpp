// ======================================================================
// \title  WasmSequencerHelpers.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class helpers
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Os/Console.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "default/config/WasmSequencerConfig.hpp"
#include "spacewasm.h"

namespace Svc {
// ----------------------------------------------------------------------
// Interpreter store and page-backed allocators
// ----------------------------------------------------------------------

U8* WasmSequencer ::globalAlloc(const U32 size, const U32 align) {
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

void WasmSequencer ::globalDealloc(const U8* ptr) {
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

void WasmSequencer ::guestDealloc(const U8* ptr, const U32 size) {
    // Bump allocator: individual frees are no-ops. The whole guest pool is reset
    // when a new store is created (destroyStore).
    (void)ptr;
    (void)size;
}

spacewasm_read_result_t WasmSequencer ::readModuleChunk(const U8** outBuf, std::size_t* outLen) {
    FW_ASSERT(this->m_loadFile != nullptr);

    FwSizeType size = sizeof(this->m_readBuf);
    const Os::File::Status status = this->m_loadFile->read(this->m_readBuf, size);

    spacewasm_read_result_t readStatus;
    if (status != Os::File::Status::OP_OK) {
        *outLen = 0;
        readStatus = SPACEWASM_READ_ERROR;
    } else {
        // `size` is updated in-place with the number of bytes actually read.
        *outBuf = this->m_readBuf;
        *outLen = static_cast<std::size_t>(size);

        if (size == 0) {
            readStatus = SPACEWASM_READ_EOF;
        } else {
            readStatus = SPACEWASM_READ_OK;
        }
    }

    return readStatus;
}

Fw::Success WasmSequencer ::createStore() {
    this->destroyStore();

    static_assert(WasmSequencerConfig::MAX_GUEST_MODULES <= 255,
                  "SpaceWasm does not support more than 255 WebAssembly guest modules");

    this->takeAllocatorLock();

    spacewasm_host_t host;
    spacewasm_status_t status = spacewasm_host_new(1, &host);
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->hostFprimeV1(&host);

    spacewasm_compiler_options_t options;
    options.allow_memory_grow = false;
    options.max_backpatch_iterations = 0;
    options.max_code_pages = Svc::WasmSequencerConfig::MAX_CODE_PAGES;

    status = spacewasm_new(&host, Svc::WasmSequencerConfig::GUEST_STACK_SIZE, WasmSequencerConfig::MAX_GUEST_MODULES,
                           options, &this->m_wasm);

    this->releaseAllocatorLock();

    // If the store allocation fails, this means the dynamic memory is too small to host this number of modules...
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->log_DIAGNOSTIC_StoreAllocationSucceeded(WasmSequencerConfig::MAX_GUEST_MODULES);
    return Fw::Success::SUCCESS;
}

void WasmSequencer ::destroyStore() {
    if (this->m_wasm != nullptr) {
        this->takeAllocatorLock();
        spacewasm_destroy(this->m_wasm);
        this->releaseAllocatorLock();
        this->m_wasm = nullptr;
    }

    // Reset the guest linear-memory bump allocator; all guest allocations were
    // owned by the store that just went away.
    this->m_guest_offset = 0;

    // Clear any pending state.
    this->m_invokeStatus = SPACEWASM_OK;
    this->m_exit.reason = WasmSequencer_ExitReason::UNKNOWN;
    this->m_exit.code = 0;
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
            return Svc::WasmSequencer_TrapReason::UNINTIIALIZED_TABLE_ELEMENT;
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
    // LOAD_NAME supplies an explicit module name; use it verbatim.
    if (moduleName.length() > 0) {
        this->m_tlm.sequenceName = moduleName;
        return;
    }

    // RUN / LOAD: derive from the file's basename with any ".wasm" suffix stripped.
    const char* const path = filePath.toChar();
    const FwSizeType len = static_cast<FwSizeType>(filePath.length());

    // Find the start of the basename (character after the last '/').
    FwSizeType start = 0;
    for (FwSizeType i = 0; i < len; i++) {
        if (path[i] == '/') {
            start = i + 1;
        }
    }

    // Drop a trailing ".wasm" if present.
    FwSizeType end = len;
    static const char suffix[] = ".wasm";
    const FwSizeType suffixLen = static_cast<FwSizeType>(sizeof(suffix) - 1);
    if ((end - start) >= suffixLen) {
        bool match = true;
        for (FwSizeType i = 0; i < suffixLen; i++) {
            if (path[end - suffixLen + i] != suffix[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            end -= suffixLen;
        }
    }

    char name[FileNameStringSize];
    FwSizeType n = 0;
    for (FwSizeType i = start; i < end && n < static_cast<FwSizeType>(sizeof(name) - 1); i++) {
        name[n++] = path[i];
    }
    name[n] = '\0';
    this->m_tlm.sequenceName = name;
}

//! Panic hook the spacewasm interpreter calls on a fatal internal error.
// Must not return.
extern "C" void spacewasm_panic(const U8* filename,
                                std::size_t filename_len,
                                U32 line,
                                const U8* msg,
                                std::size_t len) {
    Fw::String fmtMsg;
    fmtMsg.format("Rust panic %.*s:%d: %.*s\n", static_cast<int>(filename_len), reinterpret_cast<const char*>(filename),
                  static_cast<int>(line), static_cast<int>(len), reinterpret_cast<const char*>(msg));
    Os::Console::write(fmtMsg);

    // Rust panics map to FSW assertions
    FW_ASSERT(false);
}

}  // namespace Svc
