/**
 * \file StringBase.cpp
 * \author T. Canham
 * \brief Implements F Prime string base class
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

StringBase::StringBase() {}

StringBase::~StringBase() {}

const CHAR* StringBase::operator+=(const CHAR* src) {
    this->appendBuff(src, static_cast<SizeType>(StringUtils::string_length(src, this->getCapacity())));
    return this->toChar();
}

const StringBase& StringBase::operator+=(const ConstStringBase& src) {
    this->appendBuff(src.toChar(), src.length());
    return *this;
}

const StringBase& StringBase::operator+=(const StringBase& src) {
    return StringBase::operator+=(static_cast<const ConstStringBase&>(src));
}

FormatStatus StringBase::format(const CHAR* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    FormatStatus status = this->vformat(formatString, args);
    va_end(args);
    return status;
}

FormatStatus StringBase::vformat(const CHAR* formatString, va_list args) {
    CHAR* us = const_cast<CHAR*>(this->toChar());
    SizeType cap = this->getCapacity();
    FW_ASSERT(us != nullptr);
    // Needed until SizeType an FwSizeType are the same
    static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<SizeType>::max(),
                  "String size type must fit into FwSizeType");
    return Fw::stringFormat(us, static_cast<FwSizeType>(cap), formatString, args);
}

#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
void StringBase::toString(StringBase& text) const {
    text = *this;
}
#endif

StringBase& StringBase::operator=(const ConstStringBase& other) {
    if (this != &other) {
        (void)Fw::StringUtils::string_copy(const_cast<char*>(this->toChar()), other.toChar(), this->getCapacity());
    }
    return *this;
}

StringBase& StringBase::operator=(const StringBase& other) {
    return StringBase::operator=(static_cast<const ConstStringBase&>(other));
}

// Copy constructor doesn't make sense in this virtual class as there is nothing to copy. Derived classes should
// call the empty constructor and then call their own copy function
StringBase& StringBase::operator=(const CHAR* other) {  // lgtm[cpp/rule-of-two]
    (void)Fw::StringUtils::string_copy(const_cast<char*>(this->toChar()), other, this->getCapacity());
    return *this;
}

void StringBase::appendBuff(const CHAR* buff, SizeType size) {
    const SizeType capacity = this->getCapacity();
    const SizeType length = this->length();
    FW_ASSERT(capacity > length, static_cast<FwAssertArgType>(capacity), static_cast<FwAssertArgType>(length));
    // Subtract 1 to leave space for null terminator
    SizeType remaining = capacity - length - 1;
    if (size < remaining) {
        remaining = size;
    }
    FW_ASSERT(remaining < capacity, static_cast<FwAssertArgType>(remaining), static_cast<FwAssertArgType>(capacity));
    (void)strncat(const_cast<CHAR*>(this->toChar()), buff, static_cast<size_t>(remaining));
}

SerializeStatus StringBase::deserializeFrom(SerialBufferBase& buffer, Fw::Endianness mode) {
    // Get the max size of the deserialized string
    const SizeType maxSize = this->maxLength();
    // Initial estimate of actual size is max size
    // This estimate is refined when calling the deserialize function below
    SizeType actualSize = maxSize;
    // Public interface returns const char*, but implementation needs char*
    // So use const_cast
    CHAR* raw = const_cast<CHAR*>(this->toChar());
    // Deserialize length
    // Fail if length exceeds max size (the initial value of actualSize)
    // Otherwise deserialize length bytes and set actualSize to length
    SerializeStatus stat = buffer.deserializeTo(reinterpret_cast<U8*>(raw), actualSize, Serialization::INCLUDE_LENGTH);
    if (stat == FW_SERIALIZE_OK) {
        // Deserialization succeeded: null-terminate string at actual size
        FW_ASSERT(actualSize <= maxSize, static_cast<FwAssertArgType>(actualSize),
                  static_cast<FwAssertArgType>(maxSize));
        raw[actualSize] = 0;
    } else {
        // Deserialization failed: leave string unmodified, but ensure that it
        // is null-terminated
        raw[maxSize] = 0;
    }
    return stat;
}

}  // namespace Fw
