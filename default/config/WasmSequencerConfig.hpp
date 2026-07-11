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

/// The amount of memory statically allocated per-WasmSequencer instance.
/// This memory does not include memory needed by the Wasm linear memory pages,
// it is only used for loading and representing the Wasm store and bytecode.
///
/// Depending on the size of the modules and the number of modules needed at one
// time in the same store, this number will need to be adjusted.
constexpr FwSizeType DYNAMIC_MEMORY_SIZE = 8192;

}  // namespace WasmSequencerConfig
}  // namespace Svc

#endif
