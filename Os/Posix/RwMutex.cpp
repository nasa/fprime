// ======================================================================
// \title Os/Posix/RwMutex.cpp
// \brief Posix implementation for Os::RwMutex
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Posix/RwMutex.hpp>
#include <Os/Posix/error.hpp>

namespace Os {
namespace Posix {
namespace Mutex {

PosixRwMutex::PosixRwMutex() : Os::RwMutexInterface(), m_handle() {
    // Set attributes
    pthread_rwlockattr_t attribute;
    int status = pthread_rwlockattr_init(&attribute);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));

    // Set writer priority (avoiding writer starvation)
    status = pthread_rwlockattr_setkind_np(&attribute, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));

    status = pthread_rwlock_init(&this->m_handle.m_rwlock_descriptor, &attribute);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));
}

PosixRwMutex::~PosixRwMutex() {
    int status = pthread_rwlock_destroy(&this->m_handle.m_rwlock_descriptor);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));
}

PosixRwMutex::Status PosixRwMutex::take() {
    int status = pthread_rwlock_wrlock(&this->m_handle.m_rwlock_descriptor);
    return Os::Posix::posix_status_to_mutex_status(status);
}

PosixRwMutex::Status PosixRwMutex::release() {
    int status = pthread_rwlock_unlock(&this->m_handle.m_rwlock_descriptor);
    return Os::Posix::posix_status_to_mutex_status(status);
}

PosixRwMutex::Status PosixRwMutex::takeRead() {
    int status = pthread_rwlock_rdlock(&this->m_handle.m_rwlock_descriptor);
    return Os::Posix::posix_status_to_mutex_status(status);
}

PosixRwMutex::Status PosixRwMutex::releaseRead() {
    int status = pthread_rwlock_unlock(&this->m_handle.m_rwlock_descriptor);
    return Os::Posix::posix_status_to_mutex_status(status);
}

MutexHandle* PosixRwMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
