#ifndef FPP_TEST_UTILS_HPP
#define FPP_TEST_UTILS_HPP

namespace FppTest {

    namespace Utils {
    
        // Returns a random U32
        U8 getU8();

        // Returns a random U32
        U32 getU32();

        // Returns a random non-null char
        char getChar();

        // Populates buf with a random string of random length with max length size
        void setString(char *buf, U32 size);
            
    } // Utils

} // FppTest

#endif
