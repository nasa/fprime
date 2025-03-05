# ======================================================================
# \title Os/Models/Mutex.fpp
# \brief FPP type definitions for Os/Mutex.hpp concepts
# ======================================================================

module Os {
    @ FPP shadow-enum representing Os::Mutex::Status
    enum MutexStatus {
        OP_OK,          @< Operation was successful
        ERROR_BUSY,     @< Mutex is busy
        ERROR_DEADLOCK, @< Deadlock condition detected
        NOT_SUPPORTED,  @< Mutex feature is not supported
        ERROR_OTHER     @< All other errors
    }
}
