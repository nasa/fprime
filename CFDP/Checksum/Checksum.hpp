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
  //! \brief Class representing a 32-bit checksum as mandated by the CCSDS File
  //!        Delivery Protocol.
  //!
  //!        This checksum is calculated by update of an existing 32-bit value
  //!        with the "next" 32-bit string drawn from the file data. Beginning
  //!        at the start of the file, a 4-byte window moves up the file by four
  //!        bytes per update. The update itself replaces the existing checksum
  //!        with the byte-wise sum of the existing checksum and the file data
  //!        contained in the window. Overflows in the addition are permitted
  //!        and the carry discarded.
  //!
  //!        If an update is to be made beginning at an offset into the file
  //!        which is not aligned to a 4-byte boundary, the window is treated
  //!        as beginning at the last 4-byte boundary, but is left-zero-padded.
  //!        Similarly, where the file data for an update ends on an unaligned
  //!        byte, the window extends up to the next boundary and is
  //!        right-zero-padded.
  //!
  //!        ## Example
  //!
  //!        For buffer 0xDE 0xAD 0xBE 0xEF 0xCA 0xFE and initial zero checksum:
  //!
  //!        ------------------------------------ Update 1
  //!        Window     0xDE 0xAD 0xBE 0xEF
  //!        Checksum   0xDEADBEEF
  //!
  //!        ------------------------------------ Update 2
  //!        Window     0xCA 0xFE
  //!        Checksum   0xDEADBEEF+
  //!                   0xCAFE0000
  //!                   ----------
  //!                   0xA8ABBEEF <- Final value
  class Checksum {

    public:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct a fresh Checksum object.
      Checksum();

      //! Construct a Checksum object and initialize it with a value.
      Checksum(const U32 value);

      //! Copy a Checksum object.
      Checksum(const Checksum &original);

      //! Destroy a Checksum object.
      ~Checksum();

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Assign checksum to this.
      Checksum& operator=(const Checksum& checksum);

      //! Compare checksum and this for equality.
      bool operator==(const Checksum& checksum) const;

      //! Compare checksum and this for inequality.
      bool operator!=(const Checksum& checksum) const;

      //! Update the checksum value by accumulating words in the given data.
      //!
      //! \important The data and data-length passed to this method are specifically
      //!            those over which the update is made, rather than the entire
      //!            file. Typically, therefore, `data` will be a pointer to the
      //!            byte given by the offset, e.g. `&file_buffer[offset]`.
      //!
      void update(const U8* const data,  //!< Beginning of the data over which to update.
                  const U32 offset,      //!< Offset into the file at which the data begins.
                  const U32 length       //!< Length of the update data in bytes.
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
      U32 m_value;

    };

}

#endif
