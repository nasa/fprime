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

#include <FpConfig.hpp>

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
      ByteArray(
          U8 *const bytes, //!< Pointer to the bytes
          const U32 size //!< The array size
      ) :
        bytes(bytes),
        size(size)
      {

      }

      // ----------------------------------------------------------------------
      // Data 
      // ----------------------------------------------------------------------

      //! The bytes
      U8 *const bytes;

      //! The size
      const U32 size;

  };

}

#endif
