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

        NATIVE_UINT_TYPE len = this->length();

        const char* us = this->toChar();
        if ((us == 0) or (other == 0)) {
            return false;
        }
        // compare buffer
        for (NATIVE_UINT_TYPE byte = 0; byte < len; byte++) {
            if (us[byte] != other[byte]) {
                return false;
            }
        }
        return true;

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

    // Adapted from:
    // http://www.leidinger.net/FreeBSD/dox/libkern/html/d5/de6/strlcat_8c_source.html
    // strlcat source not standard, so borrowing.

       /*-
        * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
        * All rights reserved.
        *
        * Redistribution and use in source and binary forms, with or without
        * modification, are permitted provided that the following conditions
        * are met:
        * 1. Redistributions of source code must retain the above copyright
        *    notice, this list of conditions and the following disclaimer.
        * 2. Redistributions in binary form must reproduce the above copyright
        *    notice, this list of conditions and the following disclaimer in the
        *    documentation and/or other materials provided with the distribution.
        * 3. The name of the author may not be used to endorse or promote products
        *    derived from this software without specific prior written permission.
        *
        * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
        * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
        * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
        * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
        * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
        * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
        * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
        * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
        * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
        * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
        */

    void StringBase::appendBuff(const char* buff, NATIVE_UINT_TYPE size) {
        // size not used
        char *d = (char*) this->toChar();
        char *dst = d;

        const char *s = buff;
        NATIVE_UINT_TYPE n = this->getCapacity();
        NATIVE_UINT_TYPE siz = n;
        NATIVE_UINT_TYPE dlen;

        /* Find the end of buffer and adjust bytes left but don't go past end */
        while (n-- != 0 && *d != '\0') {
            d++;
        }

        dlen = d - dst;
        n = siz - dlen;

        if (n == 0) {
            return;
        }

        while (*s != '\0') {
            if (n != 1) {
                *d++ = *s;
                n--;
            }
            s++;
        }
        *d = '\0';

    }

}
