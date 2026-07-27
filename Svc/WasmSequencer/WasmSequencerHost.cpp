// ======================================================================
// \title  WasmSequencerHost.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer Wasm Host functions
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

namespace Svc {
spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_exit(spacewasm_caller_t*,
                                                            void*,
                                                            const spacewasm_value_t*,
                                                            size_t,
                                                            spacewasm_value_t*) {
    // TODO(tumbar) Add "exit" return code to SpaceWasm host functions
    //              This is basically the same as trap but not 'error-ey'
    // Exit the interpreter
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_command(spacewasm_caller_t* caller,
                                                               void* userdata,
                                                               const spacewasm_value_t* params,
                                                               size_t n_params,
                                                               spacewasm_value_t*) {
    auto* this_ = static_cast<WasmSequencer*>(userdata);

    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 2, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);

    const U32 ptr = static_cast<U32>(params[0].u.i32_);
    const U32 len = static_cast<U32>(params[1].u.i32_);

    spacewasm_hostcall_result_t return_status;
    if (len > FW_COM_BUFFER_MAX_SIZE) {
        this_->log_WARNING_HI_CommandTooLarge(len, FW_COM_BUFFER_MAX_SIZE);
        return_status = SPACEWASM_TRAP;
    } else {
        FW_ASSERT(!this_->m_hasPendingHostFunction);

        // Read the command buffer into our ComBuffer
        const auto status = spacewasm_mem_read(caller, ptr, this_->m_pendingHostFunction.buffer.getBuffAddr(), len);
        if (status != SPACEWASM_OK) {
            // Memory read failed
            this_->log_WARNING_HI_CommandInvalidPointer(status);
            return_status = SPACEWASM_TRAP;
        } else {
            // Memory read succeeded. Pause the interpreter and process it in the state machine
            this_->m_pendingHostFunction.kind = PendingHostInvocation::COMMAND;
            this_->m_pendingHostFunction.buffer.moveSerToOffset(len);
            this_->m_hasPendingHostFunction = true;

            // Always pause the interpreter to allow the state machine to process this request
            return_status = SPACEWASM_PAUSE;
        }
    }

    return return_status;
}

}  // namespace Svc
