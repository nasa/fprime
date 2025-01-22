// ======================================================================
// \title  Utils.cpp
// \author T. Chieu
// \brief  cpp file for Utils class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <string>
#include <limits>
#include <iostream>

#include "Fw/Types/Assert.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

    namespace Utils {

        U8 getNonzeroU8() {
            return static_cast<U8>(STest::Pick::lowerUpper(
                1,
                std::numeric_limits<U8>::max()
            ));
        }

        U32 getNonzeroU32() {
            return STest::Pick::lowerUpper(
                1,
                std::numeric_limits<U32>::max()
            );
        }

        char getChar() {
            return static_cast<char>(STest::Pick::lowerUpper(32, 126));
        }

        void setString(char* buf, FwSizeType capacity, FwSizeType minLength) {
            FW_ASSERT(buf != nullptr);
            // capacity must be able to hold a null-terminated string
            FW_ASSERT(capacity > 0);
            // min length must fit within capacity
            FW_ASSERT(minLength < capacity);
            U32 length = STest::Pick::lowerUpper(minLength, capacity - 1);

            for (U32 i = 0; i < length; i++) {
                FW_ASSERT(i < capacity);
                buf[i] = getChar();
            }

            FW_ASSERT(length < capacity);
            buf[length] = 0;
        }

    } // namespace Utils

} // namespace FppTest
