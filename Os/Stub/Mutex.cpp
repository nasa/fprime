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
        return Status::OP_OK;
    }

    StubMutex::Status StubMutex::release() {
        return Status::OP_OK;
    }

    MutexHandle* StubMutex::getHandle() {
        return &this->m_handle;
    }
} // namespace Mutex
} // namespace Stub
} // namespace Os
