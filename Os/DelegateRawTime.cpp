// ======================================================================
// \title Os/DelegateRawTime.cpp
// \brief common function implementation for Os::RawTimeInterface and Os::DelegateRawTime
// ======================================================================
#include <Fw/Types/Assert.hpp>
#include <Os/DelegateRawTime.hpp>
#include <Os/RawTime.hpp>

namespace Os {

DelegateRawTime::DelegateRawTime() : m_delegate(*RawTimeInterface::getDelegate(m_handle_storage)) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
}

DelegateRawTime::DelegateRawTime(RawTimeSource source)
    : m_delegate(*RawTimeInterface::getDelegate(m_handle_storage, nullptr, source)), m_source(source) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
}

DelegateRawTime::~DelegateRawTime() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    m_delegate.~RawTimeInterface();
}

// m_handle_storage is placement-new storage populated by getDelegate below
// cppcheck-suppress missingMemberCopy
DelegateRawTime::DelegateRawTime(const DelegateRawTime& other)
    : m_delegate(*RawTimeInterface::getDelegate(m_handle_storage, &other.m_delegate, other.m_source)),
      m_source(other.m_source) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
}

DelegateRawTime& DelegateRawTime::operator=(const DelegateRawTime& other) {
    if (this != &other) {
        this->m_source = other.m_source;
        this->m_delegate = *RawTimeInterface::getDelegate(m_handle_storage, &other.m_delegate, other.m_source);
    }
    return *this;
}

RawTimeHandle* DelegateRawTime::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

DelegateRawTime::Status DelegateRawTime::now() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.now();
}

DelegateRawTime::Status DelegateRawTime::getTimeInterval(const Os::RawTime& other, Fw::TimeInterval& result) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getTimeInterval(other, result);
}

Fw::SerializeStatus DelegateRawTime::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.serializeTo(buffer, mode);
}

Fw::SerializeStatus DelegateRawTime::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.deserializeFrom(buffer, mode);
}

DelegateRawTime::Status DelegateRawTime::getDiffUsec(const RawTime& other, U32& result) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getDiffUsec(other, result);
}

bool DelegateRawTime::operator==(const RawTime& other) const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const RawTimeInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate == other;
}

RawTimeSource DelegateRawTime::getSource() const {
    return this->m_source;
}

// ------------------------------------------------------------
// RawTimeInterface default implementations
// Built on pure virtual methods. Located here (not RawTimeInterface.cpp)
// to keep RawTime implementation code in one translation unit.
// ------------------------------------------------------------

RawTimeInterface::Status RawTimeInterface::getDiffUsec(const RawTime& other, U32& result) const {
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

bool RawTimeInterface::operator==(const RawTime& other) const {
    Fw::TimeInterval interval;
    Status status = this->getTimeInterval(other, interval);
    // If we error out, then the values are either:
    //    1) impossible to compare, in which case it's perfectly reasonable to consider them different, or
    //    2) too far apart to fit in a TimeInterval, in which case they are definitely different
    return status == Status::OP_OK && interval.getSeconds() == 0 && interval.getUSeconds() == 0;
}

}  // namespace Os
