// ======================================================================
// \title Os/DelegateMutex.cpp
// \brief implementation of Os::DelegateMutex, plus common Os::MutexInterface and Os::ScopeLock code
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateMutex.hpp>

namespace Os {

// ----------------------------------------------------------------------
// DelegateMutex: link-time delegating implementation
// ----------------------------------------------------------------------

DelegateMutex::DelegateMutex() : m_delegate(*MutexInterface::getDelegate(m_handle_storage)) {
    // Note: m_handle_storage is intentionally NOT value-initialized. getDelegate() placement-news
    // the delegate into it; zeroing first is dead work (a byte-wise memset of the handle array on
    // Vorago). Mirrors the RawTime fix in fprime PR #5240 (see #5297).
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

// ----------------------------------------------------------------------
// MutexInterface common implementations
// Built on pure virtual take()/release(). Located here (not MutexInterface.cpp)
// to keep Mutex implementation code in one translation unit.
// ----------------------------------------------------------------------

void MutexInterface::lock() {
    MutexInterface::Status status = this->take();
    FW_ASSERT(status == MutexInterface::Status::OP_OK,
              static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
}

void MutexInterface::unLock() {
    MutexInterface::Status status = this->release();
    FW_ASSERT(status == MutexInterface::Status::OP_OK,
              static_cast<FwAssertArgType>(reinterpret_cast<PlatformPointerCastType>(this)), status);
}

// ----------------------------------------------------------------------
// ScopeLock
// ----------------------------------------------------------------------

ScopeLock::ScopeLock(MutexInterface& mutex) : m_mutex(mutex) {
    this->m_mutex.lock();
}

ScopeLock::~ScopeLock() {
    this->m_mutex.unLock();
}

}  // namespace Os
