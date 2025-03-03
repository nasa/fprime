// ======================================================================
// \title Os/Stub/Mutex.cpp
// \brief stub implementation for Os::Mutex
// ======================================================================
#include "Os/Stub/Mutex.hpp"

namespace Os {
namespace Stub {
namespace Mutex {

    StubMutex::Status StubMutex::take() {
        // Attempt to mark the mutex as taken.
        if (this->m_handle.m_mutex_taken.exchange(true)) {
            // The mutex was already taken, so fail the operation.
            // (This stub is for platforms without the ability to block.)
            return Status::ERROR_BUSY;
        }
        // The mutex was not already taken.
        // Now that it has been marked as taken, we have successfully entered the critical section.
        return Status::OP_OK;
    }

    StubMutex::Status StubMutex::release() {
        // Attempt to mark the mutex as not taken.
        if (!this->m_handle.m_mutex_taken.exchange(false)) {
            // The mutex was already not taken, which indicates a coding defect.
            return Status::ERROR_OTHER;
        }
        // The mutex was taken.
        // Now that it has been marked as not taken, we have successfully exited the critical section.
        return Status::OP_OK;
    }

    MutexHandle* StubMutex::getHandle() {
        return &this->m_handle;
    }
} // namespace Mutex
} // namespace Stub
} // namespace Os
