// ====================================================================== 
// \title  ConstByteArray.hpp
// \author bocchino
// \brief  hpp file for ConstByteArray type
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

#ifndef Fw_ConstByteArray_HPP
#define Fw_ConstByteArray_HPP

#include "Fw/Types/BasicTypes.hpp"

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
