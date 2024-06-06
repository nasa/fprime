// ======================================================================
// \title Os/Stub/Mutex.cpp
// \brief stub implementation for Os::Mutex
// ======================================================================
#include "Os/Stub/Mutex.hpp"

namespace Os {
namespace Stub {
namespace Mutex {

    void StubMutex::lock() {
        // Do nothing
    }

    void StubMutex::unLock() {
        // Do nothing
    }

    MutexHandle* StubMutex::getHandle() {
        return &this->m_handle;
    }
} // namespace Mutex
} // namespace Stub
} // namespace Os
