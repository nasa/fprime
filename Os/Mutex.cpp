// ======================================================================
// \title Os/Mutex.cpp
// \brief common function implementation for Os::Mutex
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/Mutex.hpp>

namespace Os {

Mutex::Mutex() : m_handle_storage(), m_delegate(*MutexInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
}

Mutex::~Mutex() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    m_delegate.~MutexInterface();
}

MutexHandle* Mutex::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

Mutex::Status Mutex::take() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.take();
}

Mutex::Status Mutex::release() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.release();
}

void Mutex::lock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    Mutex::Status status = this->take();
    FW_ASSERT(status == Mutex::Status::OP_OK, status);
}

void Mutex::unLock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    Mutex::Status status = this->release();
    FW_ASSERT(status == Mutex::Status::OP_OK, status);
}

ScopeLock::ScopeLock(Mutex& mutex) : m_mutex(mutex) {
    this->m_mutex.lock();
}

ScopeLock::~ScopeLock() {
    this->m_mutex.release();
}

}  // namespace Os
