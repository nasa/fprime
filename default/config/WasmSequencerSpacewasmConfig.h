// ======================================================================
// \title  WasmSequencerSpacewasmConfig.h
// \author tumbar
// \brief  Shared configuration for WasmSequencer/Spacewasm memory
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef WASMSEQUENCER_SPACEWASM_CONFIG_H
#define WASMSEQUENCER_SPACEWASM_CONFIG_H

// Size in bytes of each page served to the spacewasm interpreter heap.
#define WASM_SEQ_SPACEWASM_PAGE_SIZE 8192

// Maximum number of interpreter-heap pages. Backs the C++ static pool
// (m_memory_pool = PAGE_SIZE * MAX_PAGES)
#define WASM_SEQ_SPACEWASM_MAX_PAGES 4

// Maximum number of WasmSequencer instances that may register a global
// allocator slot process-wide. Backs the Rust ALLOCATORS array length; a
// component beyond this count fails registration with ERR_CAPACITY.
#define WASM_SEQ_MAX_SEQUENCERS 8

#endif  // WASMSEQUENCER_SPACEWASM_CONFIG_H
