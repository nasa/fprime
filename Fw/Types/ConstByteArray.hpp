// ======================================================================
// \title  ConstByteArray.hpp
// \author bocchino
// \brief  hpp file for ConstByteArray type
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Fw_ConstByteArray_HPP
#define Fw_ConstByteArray_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Fw {

//! \class ConstByteArray
//! \brief A variable-length byte array with constant access
//!
struct ConstByteArray {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Construct a ConstByteArray
    //!
    ConstByteArray(const U8* const a_bytes,  //!< Pointer to the bytes
                   const FwSizeType a_size   //!< The array size
                   )
        : bytes(a_bytes), size(a_size) {}

    // ----------------------------------------------------------------------
    // Data
    // ----------------------------------------------------------------------

    //! The bytes
    const U8* const bytes;

    //! The size
    const FwSizeType size;
};

}  // namespace Fw

#endif
