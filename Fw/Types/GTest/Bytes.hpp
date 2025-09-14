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
#include <Fw/FPrimeBasicTypes.hpp>

namespace Fw {

namespace GTest {

//! \class Bytes
//! \brief A byte string for testing
//!
class Bytes {
  public:
    //! Construct a Bytes object
    Bytes(const U8* const a_bytes,  //!< The byte array
          const size_t a_size       //!< The size
          )
        : bytes(a_bytes), size(a_size) {}

  public:
    //! Compare two Bytes objects
    static void compare(const Bytes& expected,  //! Expected value
                        const Bytes& actual     //! Actual value
    );

  private:
    //! The bytes
    const U8* const bytes;

    //! The size
    const size_t size;
};

}  // namespace GTest

}  // namespace Fw

#endif
