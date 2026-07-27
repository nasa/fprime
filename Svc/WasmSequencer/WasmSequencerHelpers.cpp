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

    FwSizeType size = static_cast<FwSizeType>(sizeof(this->m_readBuf));
    const Os::File::Status status = this->m_loadFile->read(this->m_readBuf, size);
    if (status != Os::File::Status::OP_OK) {
        *outLen = 0;
        return SPACEWASM_READ_ERROR;
    }
    // `size` is updated in-place with the number of bytes actually read.
    *outBuf = this->m_readBuf;
    *outLen = static_cast<std::size_t>(size);
    return (size == 0) ? SPACEWASM_READ_EOF : SPACEWASM_READ_OK;
}

Fw::Success WasmSequencer ::createStore() {
    this->destroyStore();

    static_assert(WasmSequencerConfig::MAX_GUEST_MODULES <= 255,
                  "SpaceWasm does not support more than 255 WebAssembly guest modules");

    spacewasm_host_t host;
    spacewasm_status_t status = spacewasm_host_new(1, &host);
    FW_ASSERT(status == SPACEWASM_OK, status);

    U32 module_idx;
    status = spacewasm_add_host_module(&host, "fprime", 8, 0, &module_idx);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "exit", "i", "", WasmSequencer::fprime_wasm_exit, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "panic", "i", "", WasmSequencer::fprime_wasm_panic, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "tlm", "iiiii", "i",
                                         WasmSequencer::fprime_wasm_read_telemetry, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "prm", "iii", "i",
                                         WasmSequencer::fprime_wasm_read_parameter, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "cmd", "ii", "i", WasmSequencer::fprime_wasm_command, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "event", "iii", "", WasmSequencer::fprime_wasm_event, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(&host, module_idx, "rsleep", "I", "", WasmSequencer::fprime_wasm_rsleep, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    spacewasm_compiler_options_t options;
    status = spacewasm_add_host_function(&host, module_idx, "asleep", "I", "", WasmSequencer::fprime_wasm_asleep, this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    options.allow_memory_grow = false;
    options.max_backpatch_iterations = 0;
    options.max_code_pages = Svc::WasmSequencerConfig::MAX_CODE_PAGES;

    status = spacewasm_new(&host, Svc::WasmSequencerConfig::GUEST_STACK_SIZE, WasmSequencerConfig::MAX_GUEST_MODULES,
                           options, &this->m_wasm);

    // If the store allocation fails, this means the dynamic memory is too small to host this number of modules...
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->log_ACTIVITY_LO_StoreAllocationSucceeded(WasmSequencerConfig::MAX_GUEST_MODULES);
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

spacewasm_read_result_t WasmSequencer::wasmSeqReadModule(void* userdata, const U8** outBuf, std::size_t* outLen) {
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

    Fw::String fmtMsg;
    fmtMsg.format("Rust panic %.*s:%d: %.*s\n", static_cast<int>(filename_len), reinterpret_cast<const char*>(filename),
                  static_cast<int>(line), static_cast<int>(len), reinterpret_cast<const char*>(msg));
    Os::Console::write(fmtMsg);

    // TODO(tumbar) Emit a WARNING_HI event and reset the Rust state gracefully.
    FW_ASSERT(false);
}

}  // namespace Svc
