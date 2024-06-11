// ======================================================================
// \title  Utils.hpp
// \author T. Chieu
// \brief  hpp file for Utils class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_UTILS_HPP
#define FPP_TEST_UTILS_HPP

namespace FppTest {

    namespace Utils {
    
        // Returns a random nonzero U8
        U8 getNonzeroU8();

        // Returns a random nonzero U32
        U32 getNonzeroU32();

        // Returns a random non-null char
        char getChar();

        // Populates buf with a random string of random length that fits
        // within capacity, including the null terminator (i.e., length + 1 <= capacity)
        void setString(
            char *buf, //!< The buffer pointer
            FwSizeType capacity, //!< The buffer capacity
            FwSizeType minLength = 0 //!< The minimum string length, not including the null terminator
                                     //!< minLength + 1 must be <= capacity
        );
            
    } // namespace Utils

} // namespace FppTest

#endif
