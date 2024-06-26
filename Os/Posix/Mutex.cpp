// ======================================================================
// \title Os/Posix/Mutex.cpp
// \brief Posix implementation for Os::Mutex
// ======================================================================
#include <Os/Posix/Mutex.hpp>
#include <Os/Posix/error.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace Posix {
namespace Mutex {

PosixMutex::PosixMutex() : Os::MutexInterface(), m_handle() {
    // set attributes
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);

    // set to normal mutex type
    PlatformIntType status = pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_NORMAL);
    FW_ASSERT(status == 0, status);

    // set to check for priority inheritance
    status = pthread_mutexattr_setprotocol(&attribute, PTHREAD_PRIO_INHERIT);
    FW_ASSERT(status == 0, status);

    status = pthread_mutex_init(&this->m_handle.m_mutex_descriptor, &attribute);
    FW_ASSERT(status == 0, status);
}

PosixMutex::~PosixMutex() {
    PlatformIntType status = pthread_mutex_destroy(&this->m_handle.m_mutex_descriptor);
    FW_ASSERT(status == 0, status);
}

PosixMutex::Status PosixMutex::take() {
    PlatformIntType status = pthread_mutex_lock(&this->m_handle.m_mutex_descriptor);
    return Os::Posix::posix_status_to_mutex_status(status);
}

PosixMutex::Status PosixMutex::release() {
    PlatformIntType status = pthread_mutex_unlock(&this->m_handle.m_mutex_descriptor);
    return Os::Posix::posix_status_to_mutex_status(status);
}

void PosixMutex::lock() {
    PosixMutex::Status status = this->take();
    FW_ASSERT(status == PosixMutex::Status::OP_OK, status);
}

void PosixMutex::unLock() {
    PosixMutex::Status status = this->release();
    FW_ASSERT(status == PosixMutex::Status::OP_OK, status);
}

MutexHandle* PosixMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
