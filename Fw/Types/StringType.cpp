/**
 * \file
 * \author T. Canham
 * \brief Implements ISF string base class
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/Types/StringType.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

namespace Fw {

    StringBase::StringBase(void) {
    }

    StringBase::~StringBase(void) {
    }

    const char* StringBase::operator+=(const char* src) {
        this->appendBuff(src, strnlen(src, this->getCapacity()));
        return this->toChar();
    }

    const StringBase& StringBase::operator+=(const StringBase& src) {
        this->appendBuff(src.toChar(), src.length());
        return *this;
    }

    bool StringBase::operator==(const StringBase& other) const {
        NATIVE_UINT_TYPE len = this->length();
        if (len != other.length()) {
            return false;
        } else {
            return this->operator==(other.toChar());
        }
    }

    bool StringBase::operator==(const char* other) const {

        const char *const us = this->toChar();
        if ((us == NULL) or (other == NULL)) {
            return false;
        }

        const NATIVE_UINT_TYPE capacity = this->getCapacity();
        const size_t result = strncmp(us, other, capacity);
        return (result == 0);

    }

    void StringBase::format(const char* formatString, ...) {
        char* us = (char*) this->toChar();
        NATIVE_UINT_TYPE cap = this->getCapacity();
        FW_ASSERT(us);
        va_list args;
        va_start(args, formatString);
        (void) vsnprintf(us, cap, formatString, args);
        va_end(args);
        // null terminate
        us[cap - 1] = 0;
    }

    bool StringBase::operator!=(const StringBase& other) const {
        return !operator==(other);
    }

    bool StringBase::operator!=(const char* other) const {
        return !operator==(other);
    }

#if FW_SERIALIZABLE_TO_STRING
    void StringBase::toString(StringBase& text) const {
        text = *this;
    }
#endif

#ifdef BUILD_UT
    std::ostream& operator<<(std::ostream& os, const StringBase& str)
    {
        os << str.toChar();
        return os;
    }
#endif

    StringBase& StringBase::operator=(const StringBase& other) {
        Fw::StringUtils::string_copy(const_cast<char *>(this->toChar()), other.toChar(), this->getCapacity());
        return *this;
    }

    // Copy constructor doesn't make sense in this virtual class as there is nothing to copy. Derived classes should
    // call the empty constructor and then call their own copy function
    StringBase& StringBase::operator=(const char* other) { // lgtm[cpp/rule-of-two]
        Fw::StringUtils::string_copy(const_cast<char *>(this->toChar()), other, this->getCapacity());
        return *this;
    }

    void StringBase::appendBuff(const char* buff, NATIVE_UINT_TYPE size) {
        const U32 capacity = this->getCapacity();
        const U32 length = this->length();
        FW_ASSERT(capacity > length, capacity, length);
        // Subtract 1 to leave space for null terminator
        U32 remaining = capacity - length - 1;
        if(size < remaining) {
            remaining = size;
        }
        FW_ASSERT(remaining < capacity, remaining, capacity);
        (void) strncat(const_cast<char *>(this->toChar()), buff, remaining);
    }

    NATIVE_UINT_TYPE StringBase::length(void) const {
        return strnlen(this->toChar(),this->getCapacity());
    }

    SerializeStatus StringBase::serialize(SerializeBufferBase& buffer) const {
        return buffer.serialize(reinterpret_cast<const U8*>(this->toChar()),this->length());
    }

    SerializeStatus StringBase::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE maxSize = this->getCapacity() - 1;
        char *raw = const_cast<char *>(this->toChar());
        SerializeStatus stat = buffer.deserialize(reinterpret_cast<U8*>(raw),maxSize);
        // Null terminate deserialized string
        raw[maxSize] = 0;
        return stat;
    }
}
