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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Probe entry point. Returns a fixed sentinel (42) so callers can confirm the
//! Rust static library links and is callable across the FFI boundary.
uint32_t fprime_spacewasm_probe(void);

#ifdef __cplusplus
}
#endif

#endif  // SVC_WASMSEQUENCER_FPRIME_SPACEWASM_H
