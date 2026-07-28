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

/// @brief Exit the current Wasm program with a failure.
/// This function does not return.
///
/// @param code: Aribtrary code to indicate source of the panic
WASM_IMPORT("fprime", "panic")
extern void fprime_wasm_panic(I32 code);

enum FprimeTlmValid {
    FPRIME_TLM_VALID = 0,
    FPRIME_TLM_INVALID = 1,
};

/// @brief Read a telemetry channel value and write it to the specified memory addresses
///
/// @param id Channel ID to read
/// @param time_ptr Guest memory address to write channel update time
/// @param time_size Size allocated for time_ptr, should be Fw::Time::SERIALIZED_SIZE
/// @param value_ptr Guest memory address to channel's value
/// @param value_size Size allocated for value_ptr
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
/// @param value_size Size allocated for value_ptr
WASM_IMPORT("fprime", "prm")
extern FprimeTlmValid fprime_wasm_read_parameter(FwPrmIdType id, U32 value_ptr, U32 value_size);

enum FprimeCmdResponse {
    /// Command successfully executed
    FPRIME_CMD_OK = 0,
    /// Invalid opcode dispatched
    FPRIME_CMD_INVALID_OPCODE = 1,
    /// Command failed validation
    FPRIME_CMD_VALIDATION_ERROR = 2,
    /// Command failed to deserialize
    FPRIME_CMD_FORMAT_ERROR = 3,
    /// Command had execution error
    FPRIME_CMD_EXECUTION_ERROR = 4,
    /// Component busy
    FPRIME_CMD_BUSY = 5,
};

/// @brief Dispatch a command, blocking call.
///
/// Command must be encoded with the FwOpcodeType + arguments.
/// A FORMAT_ERROR will be returned if the format is not valid!
///
/// @param buf_ptr Guest memory address to encoded command
/// @param buf_size Size allocated for value_ptr
WASM_IMPORT("fprime", "cmd")
extern FprimeCmdResponse fprime_wasm_command(U32 buf_ptr, U32 buf_size);

enum FprimeEventSeverity {
    FPRIME_EVENT_FATAL = 1,        //!< A fatal non-recoverable event
    FPRIME_EVENT_WARNING_HI = 2,   //!< A serious but recoverable event
    FPRIME_EVENT_WARNING_LO = 3,   //!< A less serious but recoverable event
    FPRIME_EVENT_COMMAND = 4,      //!< An activity related to commanding
    FPRIME_EVENT_ACTIVITY_HI = 5,  //!< Important informational events
    FPRIME_EVENT_ACTIVITY_LO = 6,  //!< Less important informational events
    FPRIME_EVENT_DIAGNOSTIC = 7,   //!< Software diagnostic events
};

/// @brief Emit an event from the current WasmSequencer component at a given severity level
///
/// @param severity Event severity level to emit
/// @param msg_ptr Guest memory address to event message string
/// @param msg_size Size allocated for value_ptr
WASM_IMPORT("fprime", "event")
extern void fprime_wasm_event(FprimeEventSeverity severity, U32 msg_ptr, U32 msg_size);

/// @brief Pause the runtime for a specified time
///
/// @param us Microseconds to pause the runtime for
WASM_IMPORT("fprime", "rsleep")
extern void fprime_wasm_rsleep(U64 us);

/// @brief Pause the runtime until a specified time
///
/// @param us Microseconds from system epoch to pause until
WASM_IMPORT("fprime", "asleep")
extern void fprime_wasm_asleep(U64 us);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // FPRIME_SPACEWASM_H
