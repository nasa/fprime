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
#include "config/FwSizeTypeAliasAc.h"
#include "config/WasmSequencerSpacewasmConfig.h"

namespace Svc {
namespace WasmSequencerConfig {
constexpr FwSizeType SPACEWASM_PAGE_SIZE = WASM_SEQ_SPACEWASM_PAGE_SIZE;
constexpr FwSizeType SPACEWASM_MAX_PAGES = WASM_SEQ_SPACEWASM_MAX_PAGES;

/// Maximum number of WasmSequencer instances that may register a global
/// allocator slot process-wide
constexpr FwSizeType MAX_SEQUENCERS = WASM_SEQ_MAX_SEQUENCERS;

/// Total static pool backing the interpreter heap: 4 * 8192 = 32 KiB.
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

/// Maximum size of a serial port invocation leaving the WasmSequencer.
/// Data will be copied out of the Wasm guest into WasmSequencer memory to
/// invoke the serial output port
constexpr U32 MAX_SERIAL_PORT_SIZE = 256;

/// Buffer size to allocate for streaming a Wasm module from the filesystem to the decoder/validator
constexpr FwSizeType LOAD_READ_CHUNK_SIZE = 512;

enum class SerialInQueueFullBehavior {
    DROP_OLDEST,  //!< Oldest message will be de-queued and dropped to make space for the new message. Keep dropping
                  //!< messages until enough space is made
    DROP_NEWEST,  //!< Drop the latest message if it cannot fit in the remaining queue space
    ASSERT,       //!< Trigger an assertion if the queue fills and cannot process another message
};

constexpr SerialInQueueFullBehavior SERIAL_IN_QUEUE_FULL_BEHAVIOR = SerialInQueueFullBehavior::DROP_OLDEST;

/// Size of each serialIn port in bytes
constexpr FwSizeType SERIAL_IN_QUEUE_SIZE = 256;

}  // namespace WasmSequencerConfig
}  // namespace Svc

#endif
