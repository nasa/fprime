// ======================================================================
// \title Os/Posix/Mutex.cpp
// \brief stub implementation for Os::Mutex
// ======================================================================
#include <pthread.h>
#include <Fw/Types/Assert.hpp>
#include <Os/Mutex.hpp>
#include <Os/Posix/Mutex.hpp>
#include <new>

namespace Os {
namespace Posix {
namespace Mutex {

PosixMutex::PosixMutex() : m_handle() {
    pthread_mutex_t* handle = new (std::nothrow) pthread_mutex_t;
    FW_ASSERT(handle != nullptr);

    // set attributes
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    NATIVE_INT_TYPE stat;
    // set to error checking
//        stat = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
//        FW_ASSERT(stat == 0,stat);

    // set to normal mutex type
    stat = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    FW_ASSERT(stat == 0, stat);

    // set to check for priority inheritance
    stat = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    FW_ASSERT(stat == 0, stat);

    stat = pthread_mutex_init(handle, &attr);
    FW_ASSERT(stat == 0, stat);

    this->m_handle.m_mutex_descriptor = reinterpret_cast<POINTER_CAST>(handle);
}

PosixMutex::~PosixMutex() {
    // Question: this destructor vs the one in Os/Mutex.cpp ???
    NATIVE_INT_TYPE stat = pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(this->m_handle.m_mutex_descriptor));
    FW_ASSERT(stat == 0, stat);
    delete reinterpret_cast<pthread_mutex_t*>(this->m_handle.m_mutex_descriptor);
}

void PosixMutex::lock() {
    NATIVE_INT_TYPE stat = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(this->m_handle.m_mutex_descriptor));
    FW_ASSERT(stat == 0, stat);
}

void PosixMutex::unLock() {
    NATIVE_INT_TYPE stat = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(this->m_handle.m_mutex_descriptor));
    FW_ASSERT(stat == 0, stat);
}

MutexHandle* PosixMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
