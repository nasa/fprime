// ======================================================================
// \title Os/RawTime.cpp
// \brief common function implementation for Os::RawTime
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/RawTime.hpp>

namespace Os {

RawTime::RawTime() : m_handle_storage(), m_delegate(*RawTimeInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
}

RawTime::~RawTime() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    m_delegate.~RawTimeInterface();
}

RawTime::RawTime(const RawTime& other) :
    m_handle_storage(),
    m_delegate(*RawTimeInterface::getDelegate(m_handle_storage, &other.m_delegate)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
}

RawTime& RawTime::operator=(const RawTime& other) {
    if (this != &other) {
        this->m_delegate = *RawTimeInterface::getDelegate(m_handle_storage, &other.m_delegate);
    }
    return *this;
}

RawTimeHandle* RawTime::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

RawTime::Status RawTime::getRawTime() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getRawTime();
}

RawTime::Status RawTime::getDiffUsec(const RawTime& other, U32& result) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getDiffUsec(* const_cast<RawTimeHandle*>(const_cast<RawTime&>(other).getHandle()), result);
}

RawTime::Status RawTime::getDiffUsec(const RawTimeHandle& other, U32& result) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getDiffUsec(other, result);
}

Fw::SerializeStatus RawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    // ASSERT on buffer.getBuffCapacity() ??
    return this->m_delegate.serialize(buffer);
}
Fw::SerializeStatus RawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.deserialize(buffer);
}


}  // namespace Os
