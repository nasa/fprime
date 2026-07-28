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
    FwSizeType start = this->m_guest_offset + a - 1 & ~(a - 1);
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

    spacewasm_host_t host;
    spacewasm_status_t status = spacewasm_host_new(1, &host);
    FW_ASSERT(status == SPACEWASM_OK, status);

#define HOST_FN(member_fn)                                                                                         \
    [](struct spacewasm_caller_t* caller, void* userdata, const struct spacewasm_value_t* params, size_t n_params, \
       struct spacewasm_value_t* out_result) {                                                                     \
        FW_ASSERT(userdata);                                                                                       \
        return static_cast<WasmSequencer*>(userdata)->member_fn(caller, params, n_params, out_result);             \
    }

    U32 module_idx;
    status = spacewasm_add_host_module(&host, "fprime", 8, 0, &module_idx);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "exit", "i", "", HOST_FN(wasmExit), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "panic", "i", "", HOST_FN(wasmPanic), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "tlm", "Iiiii", "i", HOST_FN(wasmReadTelemetry), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "prm", "Iii", "i", HOST_FN(wasmReadParameter), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "cmd", "ii", "i", HOST_FN(wasmCommand), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "event", "iii", "", HOST_FN(wasmEvent), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "rsleep", "I", "", HOST_FN(wasmRsleep), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    spacewasm_compiler_options_t options;
    status = spacewasm_add_host_function(&host, module_idx, "asleep", "I", "", HOST_FN(wasmAsleep), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

#undef HOST_FN

    options.allow_memory_grow = false;
    options.max_backpatch_iterations = 0;
    options.max_code_pages = Svc::WasmSequencerConfig::MAX_CODE_PAGES;

    status = spacewasm_new(&host, Svc::WasmSequencerConfig::GUEST_STACK_SIZE, WasmSequencerConfig::MAX_GUEST_MODULES,
                           options, &this->m_wasm);

    // If the store allocation fails, this means the dynamic memory is too small to host this number of modules...
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->log_DIAGNOSTIC_StoreAllocationSucceeded(WasmSequencerConfig::MAX_GUEST_MODULES);
    return Fw::Success::SUCCESS;
}

void WasmSequencer ::destroyStore() {
    if (this->m_wasm != nullptr) {
        spacewasm_destroy(this->m_wasm);
        this->m_wasm = nullptr;
    }

    // Reset the guest linear-memory bump allocator; all guest allocations were
    // owned by the store that just went away.
    this->m_guest_offset = 0;

    // Clear any pending state.
    this->m_invokeStatus = SPACEWASM_OK;
    this->m_loadStatus = SPACEWASM_OK;
}

Svc::WasmSequencer_AllocError::T WasmSequencer ::mapAllocError(spacewasm_status_t status) {
    switch (status) {
        case SPACEWASM_ERR_OUT_OF_MEMORY:
            return Svc::WasmSequencer_AllocError::OUT_OF_MEMORY;
        case SPACEWASM_ERR_PAGE_TOO_SMALL:
            return Svc::WasmSequencer_AllocError::PAGE_TOO_SMALL;
        case SPACEWASM_ERR_ALLOC_FAILED:
        default:
            return Svc::WasmSequencer_AllocError::ALLOCATION_FAILED;
    }
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

    Fw::String fmtMsg;
    fmtMsg.format("Rust panic %.*s:%d: %.*s\n", static_cast<int>(filename_len), reinterpret_cast<const char*>(filename),
                  static_cast<int>(line), static_cast<int>(len), reinterpret_cast<const char*>(msg));
    Os::Console::write(fmtMsg);

    // TODO(tumbar) Is there a clean way to expose the filename/line to FW_ASSERT?
    FW_ASSERT(false);
}

}  // namespace Svc
