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

#include "STest/Pick/Pick.hpp"

#include <string>
#include <limits>
#include <iostream>

namespace FppTest {
        
    namespace Utils {
    
        U8 getU8() {
            return static_cast<U8>(STest::Pick::lowerUpper(
                1,
                std::numeric_limits<U8>::max()
            ));
        }

        U32 getU32() {
            return STest::Pick::lowerUpper(
                1,
                std::numeric_limits<U8>::max()
            );
        }

        char getChar() {
            return static_cast<char>(STest::Pick::lowerUpper(1, 127));
        }

        void setString(char* buf, U32 size) {
            U32 length = STest::Pick::lowerUpper(1, size);

            if (length == 0) {
                buf[0] = 0;
                return;
            }

            for (U32 i = 0; i < length - 1; i++) {
                buf[i] = getChar();
            }

            buf[length-1] = 0;
        }
        
    } // namespace Utils

} // namespace FppTest
