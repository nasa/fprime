// ======================================================================
// \title  ByteArray.hpp
// \author bocchino
// \brief  hpp file for ByteArray type
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Fw_ByteArray_HPP
#define Fw_ByteArray_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Fw {

//! \class ByteArray
//! \brief A variable-length byte array
//!
struct ByteArray {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Construct a ByteArray
    //!
    ByteArray(U8* const a_bytes,       //!< Pointer to the bytes
              const FwSizeType a_size  //!< The array size
              )
        : bytes(a_bytes), size(a_size) {}

    // ----------------------------------------------------------------------
    // Data
    // ----------------------------------------------------------------------

    //! The bytes
    U8* const bytes;

    //! The size
    const FwSizeType size;
};

}  // namespace Fw

#endif
