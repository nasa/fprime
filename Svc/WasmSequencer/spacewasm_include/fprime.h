// ======================================================================
// \title  fprime.h
// \author tumbar
// \brief  Header for the F Prime WebAssembly System Interface (fwasi)
// ======================================================================

#ifndef FPRIME_SPACEWASM_GUEST_H
#define FPRIME_SPACEWASM_GUEST_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <Fw/FPrimeBasicTypes.h>

#if defined(__wasm__)
#define WASM_IMPORT(module, name) __attribute__((__import_module__(module), __import_name__(name)))
#else
#define WASM_IMPORT(module, name)
#endif  // defined(__wasm__)

#define WASM_MODULE_NAME ("fprime_v1")

/// @brief Exit the current Wasm program with a return code.
/// This function does not return.
///
/// @param code: 0 indicates success, any non-zero exit code will
//               be reported in an event and the sequence will exit
//               with failure status
WASM_IMPORT(WASM_MODULE_NAME, "exit")
extern void fprime_wasm_exit(I32 code);

/// @brief Exit the current Wasm program with a failure.
/// This function does not return.
///
/// @param code: Aribtrary code to indicate source of the panic
WASM_IMPORT(WASM_MODULE_NAME, "panic")
extern void fprime_wasm_panic(I32 code);

/// @brief Get the sequence arguments this sequence was invoked with
/// This function will write the arguments provided from the invoke/run
/// into the guest memory.
///
/// @param destination_ptr Guest memory address to argument buffer
/// @param destination_size Length of the argument buffer.
///                         This size must be greater than or equal to the length of arguments
///                         passed to the sequence otherwise the interpreter will trap
/// @returns The number of bytes written to [destination_ptr]
WASM_IMPORT(WASM_MODULE_NAME, "args")
extern U32 fprime_wasm_get_args(U32 destination_ptr, U32 destination_size);

/// @brief Read the current F´ system time into guest memory
///
/// The host serializes an Fw::Time into the guest buffer.
///
/// @param time_ptr Guest memory address to write the serialized time
/// @param time_size Size allocated for time_ptr, must equal Fw::Time::SERIALIZED_SIZE
WASM_IMPORT(WASM_MODULE_NAME, "time")
extern void fprime_wasm_get_time(U32 time_ptr, U32 time_size);

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
WASM_IMPORT(WASM_MODULE_NAME, "tlm")
extern FprimeTlmValid fprime_wasm_read_telemetry(I64 id, U32 time_ptr, U32 time_size, U32 value_ptr, U32 value_size);

/// Parameter validity, mirroring Fw::ParamValid. Unlike telemetry (FprimeTlmValid, a
/// two-state VALID/INVALID enum), a parameter read distinguishes four states, so it uses
/// its own encoding. Note FPRIME_PARAM_VALID is 1, not 0.
enum FprimeParamValid {
    FPRIME_PARAM_UNINIT = 0,   //!< Parameter storage has not been initialized
    FPRIME_PARAM_VALID = 1,    //!< Parameter has a valid value from the parameter database
    FPRIME_PARAM_INVALID = 2,  //!< Parameter value is invalid (e.g. not found)
    FPRIME_PARAM_DEFAULT = 3,  //!< Parameter is serving its compiled-in default value
};

/// @brief Read a parameter value and write it to the specified memory addresses
///
/// @param id Parameter ID to read
/// @param value_ptr Guest memory address to parameters's value
/// @param value_size Size allocated for value_ptr
/// @returns Parameter validity (FprimeParamValid); value bytes are written when the
///          parameter is present. This differs from the telemetry validity enum:
///          FPRIME_PARAM_VALID is 1, not 0.
WASM_IMPORT(WASM_MODULE_NAME, "prm")
extern FprimeParamValid fprime_wasm_read_parameter(I64 id, U32 value_ptr, U32 value_size);

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
WASM_IMPORT(WASM_MODULE_NAME, "cmd")
extern FprimeCmdResponse fprime_wasm_command(U32 buf_ptr, U32 buf_size);

enum FprimeEventSeverity : I32 {
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
WASM_IMPORT(WASM_MODULE_NAME, "event")
extern void fprime_wasm_event(FprimeEventSeverity severity, U32 msg_ptr, U32 msg_size);

/// @brief Pause the runtime for a specified time
///
/// @param us Microseconds to pause the runtime for
WASM_IMPORT(WASM_MODULE_NAME, "rsleep")
extern void fprime_wasm_rsleep(U64 us);

/// @brief Pause the runtime until a specified time
///
/// @param us Microseconds from system epoch to pause until
WASM_IMPORT(WASM_MODULE_NAME, "asleep")
extern void fprime_wasm_asleep(U64 us);

/// @brief Invoke a serial port
/// If the port is not connected, the module will panic/trap
///
/// @param index Port index to emit on the serialOut on
/// @param data_ptr Pointer to the data to send on the output port
/// @param data_size Length of the data to send on the output port
WASM_IMPORT(WASM_MODULE_NAME, "serial_send")
extern void fprime_wasm_serial_out(I32 index, U32 data_ptr, U32 data_size);

enum FprimeBlockingType {
    FPRIME_BLOCK_BLOCKING = 0,     //!< Block the wasm execution until a message is received (subject to timeouts)
    FPRIME_BLOCK_NONBLOCKING = 1,  //!< If there are no messages in the queue, return immediately
};

enum FprimeQueueStatus {
    FPRIME_QUEUE_OK = 0,     //!< Message was received and copied into the destination memory
    FPRIME_QUEUE_EMPTY = 1,  //!< Empty queue with a NONBLOCKING request
};

/// @brief Receive a message from a serial input queue
///
/// @param index Port number/queue index to receive message from
/// @param data_ptr Pointer to the destination to write serial messages into
/// @param data_size Size of the data_ptr memory. If a message is larger than this size, trap the wasm module
/// @param actual_size_ptr (U32*). On message receive, number of bytes received will be written here (little endian)
/// @param block_type Whether or not to block for a message when the queue on this index is empty
/// @returns Status on whether or not we received a message (blocking will always return OK)
WASM_IMPORT(WASM_MODULE_NAME, "serial_recv")
extern FprimeQueueStatus fprime_wasm_serial_receive(I32 index,
                                                    U32 data_ptr,
                                                    U32 data_size,
                                                    U32 actual_size_ptr,
                                                    FprimeBlockingType block_type);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // FPRIME_SPACEWASM_GUEST_H
