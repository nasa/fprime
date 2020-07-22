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
#include <Fw/Types/BasicTypes.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Fw/Types/Assert.hpp>

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
        text = this->toChar();
    }
#endif

#ifdef BUILD_UT
    std::ostream& operator<<(std::ostream& os, const StringBase& str)
    {
        os << str.toChar();
        return os;
    }
#endif

    const StringBase& StringBase::operator=(const StringBase& other) {
        this->copyBuff(other.toChar(), this->getCapacity());
        return *this;
    }

    const char* StringBase::operator=(const char* other) {
        this->copyBuff(other, this->getCapacity());
        return this->toChar();
    }

    void StringBase::appendBuff(const char* buff, NATIVE_UINT_TYPE size) {
        const U32 capacity = this->getCapacity();
        const U32 length = this->length();
        FW_ASSERT(capacity > length, capacity, length);
        // Subtract 1 to leave space for null terminator
        const U32 remaining = capacity - length - 1;
        FW_ASSERT(remaining < capacity, remaining, capacity);
        (void) strncat((char*) this->toChar(), buff, remaining);
    }

}
