/**
 * \file ConstStringBase.cpp
 * \brief Implements F Prime read-only string base class
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringType.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <cstdarg>
#include <cstring>

namespace Fw {

ConstStringBase::ConstStringBase() {}

ConstStringBase::~ConstStringBase() {}

ConstStringBase::SizeType ConstStringBase::length() const {
    SizeType length = 0;
    if (this->getCapacity() > 0) {
        length = static_cast<SizeType>(StringUtils::string_length(this->toChar(), this->getCapacity()));
    }
    FW_ASSERT(length <= this->maxLength(), static_cast<FwAssertArgType>(length),
              static_cast<FwAssertArgType>(this->maxLength()));
    return length;
}

ConstStringBase::SizeType ConstStringBase::maxLength() const {
    const SizeType capacity = this->getCapacity();
    return capacity == 0 ? 0 : capacity - 1;
}

ConstStringBase::SizeType ConstStringBase::serializedSize() const {
    return static_cast<SizeType>(sizeof(FwSizeStoreType)) + this->length();
}

ConstStringBase::SizeType ConstStringBase::serializedTruncatedSize(FwSizeType maxLength) const {
    return static_cast<SizeType>(sizeof(FwSizeStoreType)) + static_cast<SizeType>(FW_MIN(this->length(), maxLength));
}

bool ConstStringBase::operator==(const ConstStringBase& other) const {
    SizeType len = this->length();
    if (len != other.length()) {
        return false;
    } else {
        return this->operator==(other.toChar());
    }
}

bool ConstStringBase::operator==(const CHAR* other) const {
    const CHAR* const us = this->toChar();
    if ((us == nullptr) or (other == nullptr)) {
        return false;
    }

    const SizeType capacity = this->getCapacity();
    const size_t result = static_cast<size_t>(strncmp(us, other, static_cast<size_t>(capacity)));
    return (result == 0);
}

bool ConstStringBase::operator!=(const ConstStringBase& other) const {
    return !operator==(other);
}

bool ConstStringBase::operator!=(const CHAR* other) const {
    return !operator==(other);
}

SerializeStatus ConstStringBase::serializeTo(SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(reinterpret_cast<const U8*>(this->toChar()), this->length());
}

SerializeStatus ConstStringBase::serializeTo(SerialBufferBase& buffer, SizeType maxLength, Fw::Endianness mode) const {
    const FwSizeType len = FW_MIN(maxLength, this->length());
    // Serialize length and then bytes
    return buffer.serializeFrom(reinterpret_cast<const U8*>(this->toChar()), len, Serialization::INCLUDE_LENGTH);
}

SerializeStatus ConstStringBase::deserializeFrom(SerialBufferBase& buffer, Fw::Endianness mode) {
    // Cannot deserialize into a read-only string
    // This should be overridden by derived classes
    return SerializeStatus::FW_DESERIALIZE_IMMUTABLE;
}

#ifdef BUILD_UT
std::ostream& operator<<(std::ostream& os, const ConstStringBase& str) {
    os << str.toChar();
    return os;
}
#endif

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
void ConstStringBase::toString(StringBase& text) const {
    text = *this;
}
#endif

}  // namespace Fw
