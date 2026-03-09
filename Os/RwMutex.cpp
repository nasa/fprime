// ======================================================================
// \title Os/RwMutex.cpp
// \brief common function implementation for Os::RwMutex
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/RwMutex.hpp>

namespace Os {

RwMutex::RwMutex() : m_handle_storage(), m_delegate(*RwMutexInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
}

RwMutex::~RwMutex() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    m_delegate.~RwMutexInterface();
}

MutexHandle* RwMutex::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

RwMutex::Status RwMutex::take() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.take();
}

RwMutex::Status RwMutex::release() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.release();
}

void RwMutex::lock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    RwMutex::Status status = this->take();
    FW_ASSERT(status == RwMutex::Status::OP_OK,
              static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
}

void RwMutex::unLock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    RwMutex::Status status = this->release();
    FW_ASSERT(status == RwMutex::Status::OP_OK,
              static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
}

RwMutex::Status RwMutex::takeRead() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.takeRead();
}

RwMutex::Status RwMutex::releaseRead() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.releaseRead();
}

void RwMutex::lockRead() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    RwMutex::Status status = this->takeRead();
    FW_ASSERT(status == RwMutex::Status::OP_OK,
              static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
}

void RwMutex::unLockRead() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RwMutexInterface*>(&this->m_handle_storage[0]));
    RwMutex::Status status = this->releaseRead();
    FW_ASSERT(status == RwMutex::Status::OP_OK,
              static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
}

ReadLock::ReadLock(RwMutex& mutex) : m_mutex(mutex) {
    this->m_mutex.lockRead();
}

ReadLock::~ReadLock() {
    this->m_mutex.unLockRead();
}

WriteLock::WriteLock(RwMutex& mutex) : m_mutex(mutex) {
    this->m_mutex.lock();
}

WriteLock::~WriteLock() {
    this->m_mutex.unLock();
}

}  // namespace Os
