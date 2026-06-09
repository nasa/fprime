# ======================================================================
# \title Os/Models/CountingSemaphore.fpp
# \brief FPP type definitions for Os/CountingSemaphore.hpp concepts
# ======================================================================

module Os {
    @ FPP shadow-enum representing Os::CountingSemaphore::Status
    enum CountingSemaphoreStatus : U8 {
        OP_OK,                 @< Operation was successful
        ERROR_TIMEOUT,         @< Timeout occurred during wait
        ERROR_INVALID,         @< Invalid semaphore or argument
        ERROR_NOT_IMPLEMENTED, @< Feature not implemented
        NOT_SUPPORTED,         @< CountingSemaphore does not support operation
        ERROR_OTHER            @< All other errors
    }
}
