#ifndef FPP_TEST_UTILS_HPP
#define FPP_TEST_UTILS_HPP

namespace FppTest {

    namespace Utils {
    
        // Returns a random nonzero U8
        U8 getU8();

        // Returns a random nonzero U32
        U32 getU32();

        // Returns a random non-null char
        char getChar();

        // Populates buf with a random nonempty string of random length with max length size
        void setString(char *buf, U32 size);
            
    } // namespace Utils

} // namespace FppTest

#endif
