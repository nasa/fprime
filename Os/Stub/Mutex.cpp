// ======================================================================
// \title Os/Stub/Mutex.cpp
// \brief stub implementation for Os::Mutex
// ======================================================================
#include "Os/Stub/Mutex.hpp"

namespace Os {
namespace Stub {
namespace Mutex {

    void StubMutex::lock() {
        // no op
    }

    void StubMutex::unLock() {
        // no op
    }

    StubMutex::Status StubMutex::take() {
        return Status::NO_OP;
    }

    StubMutex::Status StubMutex::release() {
        return Status::NO_OP;
    }

    MutexHandle* StubMutex::getHandle() {
        return &this->m_handle;
    }
} // namespace Mutex
} // namespace Stub
} // namespace Os
