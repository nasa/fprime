// ======================================================================
// \title Os/DelegateMutex.cpp
// \brief common function implementation for Os::MutexInterface and Os::DelegateMutex
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateMutex.hpp>
#include <Os/Mutex.hpp>

namespace Os {

DelegateMutex::DelegateMutex() : m_handle_storage(), m_delegate(*MutexInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
}

DelegateMutex::~DelegateMutex() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    m_delegate.~MutexInterface();
}

MutexHandle* DelegateMutex::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

DelegateMutex::Status DelegateMutex::take() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.take();
}

DelegateMutex::Status DelegateMutex::release() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.release();
}

void DelegateMutex::lock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.lock();
}

void DelegateMutex::unLock() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<MutexInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.unLock();
}

// ------------------------------------------------------------
// MutexInterface default implementations
// Built on pure virtual methods. Located here (not MutexInterface.cpp)
// to keep Mutex implementation code in one translation unit.
// ------------------------------------------------------------

void MutexInterface::lock() {
    Status status = this->take();
    FW_ASSERT(status == Status::OP_OK, static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)),
              status);
}

void MutexInterface::unLock() {
    Status status = this->release();
    FW_ASSERT(status == Status::OP_OK, static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)),
              status);
}

}  // namespace Os
