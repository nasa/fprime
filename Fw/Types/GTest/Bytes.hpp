// ====================================================================== 
// \title  Fw/Types/GTest/Bytes.hpp
// \author bocchino
// \brief  hpp file for Bytes
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef Fw_GTest_Bytes_HPP
#define Fw_GTest_Bytes_HPP

#include <gtest/gtest.h>
#include <FpConfig.hpp>

namespace Fw {

  namespace GTest {

    //! \class Bytes
    //! \brief A byte string for testing
    //!
    class Bytes {

      public:

        //! Construct a Bytes object
        Bytes(
            const U8 *const bytes, //!< The byte array
            const size_t size //!< The size
        ) :
          bytes(bytes),
          size(size)
        {

        }

      public:

        //! Compare two Bytes objects
        static void compare(
            const Bytes& expected, //! Expected value
            const Bytes& actual //! Actual value
        );

      private:

        //! The bytes
        const U8 *const bytes;

        //! The size
        const size_t size;

    };

  };

}

#endif
