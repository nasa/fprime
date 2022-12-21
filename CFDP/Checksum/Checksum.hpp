// ======================================================================
// \title  CFDP/Checksum/Checksum.hpp
// \author bocchino
// \brief  hpp file for CFDP checksum class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef CFDP_Checksum_HPP
#define CFDP_Checksum_HPP

#include <FpConfig.hpp>

namespace CFDP {

  //! \class Checksum
  //! \brief Class representing a CFDP checksum
  //!
  class Checksum {

    public:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct a fresh Checksum object
      Checksum();

      //! Construct a Checksum object and initialize it with a value
      Checksum(const U32 value);

      //! Copy a Checksum object
      Checksum(const Checksum &original);

      //! Destroy a Checksum object
      ~Checksum();

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Assign checksum to this
      Checksum& operator=(const Checksum& checksum);

      //! Compare checksum and this for equality
      bool operator==(const Checksum& checksum) const;

      //! Compare checksum and this for inequality
      bool operator!=(const Checksum& checksum) const;

      //! Update the checksum value by accumulating the words in the data
      void update(
          const U8 *const data, //!< The data
          const U32 offset, //!< The offset of the start of the data, relative to the start of the file
          const U32 length //!< The length of the data in bytes
      );

      //! Get the checksum value
      U32 getValue() const;

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private instance methods
      // ----------------------------------------------------------------------

      //! Add a four-byte aligned word to the checksum value
      void addWordAligned(
          const U8 *const word //! The word
      );

      //! Add a four-byte unaligned word to the checksum value
      void addWordUnaligned(
          const U8 *const word, //! The word
          const U8 position, //! The position of the word relative to the start of the file
          const U8 length //! The number of valid bytes in the word
      );

      //! Add byte to value at offset in word
      void addByteAtOffset(
          const U8 byte, //! The byte
          const U8 offset //! The offset
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The accumulated checksum value
      U32 value;

    };

}

#endif
