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

/// Maximum number of compiled IR code pages allowed across all modules loaded onto
/// a store (spacewasm_compiler_options_t `max_code_pages`).
constexpr U32 MAX_CODE_PAGES = 256;

/// The maximum number of Wasm modules allowed to load into the sequencer's store.
/// If this sequencer does not have enough memory to dynamic allocate this store,
/// the sequencer's preamble will trigger an assertion.
constexpr U8 MAX_GUEST_MODULES = 8;

/// Configures the control-flow backpatch chain length resolution limit.
/// This is only needed to give a bound to a potentially long loop during load time.
/// Set this to `0` to disable this upper limit. It is theoritically impossible for this
// loop go on forever but we set it to a large number by default for FSW completeness.
/// If you see a `ERR_POSSIBLE_BACKPATCH_CYCLE` error, it is safe to increase this number.
/// You'll only see this on massive modules (eg. Pyodide).
constexpr U32 MAX_BACKPATCH_ITERATIONS = 32768;

/// Maximum number of WasmSequencer instances that may register a global
/// allocator slot process-wide
constexpr FwSizeType MAX_SEQUENCERS = WASM_SEQ_MAX_SEQUENCERS;

/// Buffer size to allocate for streaming a Wasm module from the filesystem to the decoder/validator.
/// NOTE: this buffer is stack-allocated on the component task's stack for the duration of a module load
/// so it must stay small relative to the task stack. Raising it materially (e.g. to several KiB) risks a
// stack overflow inside the spacewasm load call chain on the flight target.
constexpr FwSizeType LOAD_READ_CHUNK_SIZE = 512;

/// Maximum number of concurrent `WAIT` commands each WasmSequencer can service
constexpr FwSizeType MAX_CONCURRENT_WAIT_COMMANDS = 8;

}  // namespace WasmSequencerConfig
}  // namespace Svc

#endif
