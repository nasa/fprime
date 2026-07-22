// ======================================================================
// \title  fprime.h
// \author tumbar
// \brief  Header for the F Prime WebAssembly System Interface (fwasi)
// ======================================================================

#ifndef FPRIME_SPACEWASM_H
#define FPRIME_SPACEWASM_H

#include "config/FwChanIdTypeAliasAc.h"
#include "config/FwPrmIdTypeAliasAc.h"
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <Fw/FPrimeBasicTypes.h>

#if defined(__wasm__)
#define WASM_IMPORT(module, name) __attribute__((__import_module__(module), __import_name__(name)))
#else
#define WASM_IMPORT(module, name)
#endif  // defined(__wasm__)

/// @brief Exit the current Wasm program with a return code.
/// This function does not return.
///
/// @param code: 0 indicates success, any non-zero exit code will
//               be reported in an event and the sequence will exit
//               with failure status
WASM_IMPORT("fprime", "exit")
extern void fprime_wasm_exit(I32 code);

typedef enum {

} FprimeTlmValid;

/// @brief Read a telemetry channel value and write it to the specified memory addresses
///
/// @param id Channel ID to read
/// @param time_ptr Guest memory address to write channel update time
/// @param time_size Size allocated for time_ptr, should be Fw::Time::SERIALIZED_SIZE
/// @param value_ptr Guest memory address to channel's value
/// @param value_len Size allocated for value_ptr
WASM_IMPORT("fprime", "tlm")
extern FprimeTlmValid fprime_wasm_read_telemetry(FwChanIdType id,
                                                 U32 time_ptr,
                                                 U32 time_size,
                                                 U32 value_ptr,
                                                 U32 value_size);

/// @brief Read a parameter value and write it to the specified memory addresses
///
/// @param id Parameter ID to read
/// @param value_ptr Guest memory address to parameters's value
/// @param value_len Size allocated for value_ptr
WASM_IMPORT("fprime", "prm")
extern FprimeTlmValid fprime_wasm_read_parameter(FwPrmIdType id, U32 value_ptr, U32 value_size);

/// @brief Dispatch a command, blocking call
///
/// @param buf_ptr Guest memory address to 
/// @param value_len Size allocated for value_ptr
WASM_IMPORT("fprime", "cmd")
extern FprimeTlmValid fprime_wasm_command(U32 buf_ptr, U32 buf_len);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // FPRIME_SPACEWASM_H
