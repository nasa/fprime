// ====================================================================== 
// \title  ByteArray.hpp
// \author bocchino
// \brief  hpp file for ByteArray type
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef Fw_ByteArray_HPP
#define Fw_ByteArray_HPP

#include "Fw/Types/BasicTypes.hpp"

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
