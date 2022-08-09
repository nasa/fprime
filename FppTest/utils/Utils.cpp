#include "STest/Pick/Pick.hpp"

#include <string>
#include <limits>
#include <iostream>

namespace FppTest {
        
    namespace Utils {
    
        U8 getU8() {
            return static_cast<U8>(STest::Pick::lowerUpper(
                std::numeric_limits<U8>::min(),
                std::numeric_limits<U8>::max()
            ));
        }

        U32 getU32() {
            return STest::Pick::lowerUpper(
                std::numeric_limits<U32>::min(),
                std::numeric_limits<U32>::max()
            );
        }

        char getChar() {
            return static_cast<char>(STest::Pick::lowerUpper(1, 255));
        }

        void setString(char* buf, U32 size) {
            U32 length = STest::Pick::lowerUpper(0, size);

            if (length == 0) {
                buf[0] = 0;
                return;
            }

            for (int i = 0; i < length - 1; i++) {
                buf[i] = getChar();
            }

            buf[length-1] = 0;
        }
        
    } // Utils

} // FppTest
