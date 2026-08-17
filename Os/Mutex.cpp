// ======================================================================
// \title Os/Mutex.cpp
// \brief implementation of Os::ScopeLock
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Mutex.hpp>

namespace Os {

ScopeLock::ScopeLock(Mutex& mutex) : m_mutex(mutex) {
    this->m_mutex.lock();
}

ScopeLock::~ScopeLock() {
    this->m_mutex.unLock();
}

}  // namespace Os
