// ======================================================================
// \title  fprime_spacewasm.h
// \brief  C ABI contract for the fprime_spacewasm Rust static library
//
// This header is the hand-written interface between the Svc::WasmSequencer
// C++ component and the `fprime_spacewasm` Rust crate (a C ABI wrapper over
// the `spacewasm` WebAssembly interpreter). Declarations here must match the
// `extern "C"` functions exported from that crate.
// ======================================================================

#ifndef SVC_WASMSEQUENCER_FPRIME_SPACEWASM_H
#define SVC_WASMSEQUENCER_FPRIME_SPACEWASM_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

//! Probe entry point. Returns a fixed sentinel (42) so callers can confirm the
//! Rust static library links and is callable across the FFI boundary.
uint32_t fprime_spacewasm_probe(void);

//! Allocate a new interpreter Store, owned Rust-side as a `Box<Store>` and
//! backed by the page-allocated global allocator. On success returns 0 and
//! writes an opaque `Store*` to `*out_store`. On failure returns the non-zero
//! `spacewasm::AllocError` code and writes null to `*out_store`.
//!
//! \param max_modules  Maximum number of modules the Store can hold (<= 256)
//! \param out_store    Out-param receiving the opaque Store pointer
uint32_t fprime_spacewasm_store_new(uint32_t max_modules, void** out_store);

//! Destroy a Store previously returned by fprime_spacewasm_store_new. Passing
//! null is a no-op.
void fprime_spacewasm_store_delete(void* store);

#ifdef __cplusplus
}
#endif

#endif  // SVC_WASMSEQUENCER_FPRIME_SPACEWASM_H
