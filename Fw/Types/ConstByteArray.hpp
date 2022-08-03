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

#include <FpConfig.hpp>

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
      ConstByteArray(
          const U8 *const bytes, //!< Pointer to the bytes
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
      const U8 *const bytes;

      //! The size
      const U32 size;

  };

}

#endif
