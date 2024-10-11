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

RawTime::RawTime(const RawTime& other)
    : m_handle_storage(), m_delegate(*RawTimeInterface::getDelegate(m_handle_storage, &other.m_delegate)) {
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

RawTime::Status RawTime::now() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.now();
}

RawTime::Status RawTime::getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& result) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getTimeInterval(other, result);
}

Fw::SerializeStatus RawTime::serialize(Fw::SerializeBufferBase& buffer) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.serialize(buffer);
}

Fw::SerializeStatus RawTime::deserialize(Fw::SerializeBufferBase& buffer) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.deserialize(buffer);
}

RawTime::Status RawTime::getDiffUsec(const RawTime& other, U32& result) const {
    Fw::TimeInterval interval;
    Status status = this->getTimeInterval(other, interval);
    if (status != Status::OP_OK) {
        return status;
    }

    // Check overflows in computation
    U32 seconds = interval.getSeconds();
    U32 useconds = interval.getUSeconds();
    if (seconds > (std::numeric_limits<U32>::max() / 1000000)) {
        result = std::numeric_limits<U32>::max();
        return Status::OP_OVERFLOW;
    }
    U32 secToUsec = seconds * 1000000;
    if (secToUsec > (std::numeric_limits<U32>::max() - useconds)) {
        result = std::numeric_limits<U32>::max();
        return Status::OP_OVERFLOW;
    }
    // No overflow, we can safely add values to get total microseconds
    result = secToUsec + useconds;
    return status;
}

}  // namespace Os
