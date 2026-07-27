// ======================================================================
// \title  WasmSequencerConfig.hpp
// \author tumbar
// \brief  hpp file for WebAssembly Sequencer configuration
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef WASMSEQUENCERCONFIG_HPP
#define WASMSEQUENCERCONFIG_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
namespace WasmSequencerConfig {

/// Backing for the process-wide spacewasm global page allocator. These two
/// constants MUST match the values baked into the `spacewasm_c_api` crate's
/// `config.rs` (`GLOBAL_ALLOCATOR_PAGE_SIZE` / `GLOBAL_ALLOCATOR_MAX_PAGES`).
/// The crate requests fixed-size pages of exactly SPACEWASM_PAGE_SIZE and never
/// holds more than SPACEWASM_MAX_PAGES at once; the component serves them from a
/// static pool. This memory is used only for the Wasm store and bytecode, NOT
/// for guest linear-memory pages (see GUEST_MEMORY_SIZE).
constexpr FwSizeType SPACEWASM_PAGE_SIZE = 8192;
constexpr FwSizeType SPACEWASM_MAX_PAGES = 4;

/// Total static pool backing the interpreter heap: 16 * 8192 = 128 KiB.
constexpr FwSizeType DYNAMIC_MEMORY_SIZE = SPACEWASM_PAGE_SIZE * SPACEWASM_MAX_PAGES;

/// Size in bytes of the guest operand stack allocated per store
/// (spacewasm_store_new `stack_size`).
constexpr FwSizeType GUEST_STACK_SIZE = 256;

/// Maximum number of compiled code pages allowed across all modules loaded onto
/// a store (spacewasm_compiler_options_t `max_code_pages`).
constexpr U32 MAX_CODE_PAGES = 256;

/// Static pool backing the per-load guest linear-memory allocator
/// (spacewasm_allocator_new). A Wasm page is 64 KiB; modules are compiled with
/// memory.grow disabled, so this bounds the largest guest linear memory the
/// sequencer will accept.
constexpr FwSizeType GUEST_MEMORY_SIZE = 2048;

/// The maximum number of Wasm modules allowed to load into the sequencer's store.
/// If this sequencer does not have enough memory to dynamic allocate this store,
/// the sequencer's preamble will trigger an assertion.
constexpr U8 MAX_GUEST_MODULES = 8;

}  // namespace WasmSequencerConfig
}  // namespace Svc

#endif
