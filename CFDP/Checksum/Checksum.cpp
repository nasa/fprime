// ======================================================================
// \title  CFDP/Checksum/Checksum.cpp
// \author bocchino
// \brief  cpp file for CFDP checksum class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "CFDP/Checksum/Checksum.hpp"
#include "Fw/Types/Assert.hpp"

static U32 min(const U32 a, const U32 b) {
  return (a < b) ? a : b;
}

namespace CFDP {

  Checksum ::
    Checksum() : m_value(0)
  {

  }

  Checksum ::
    Checksum(const U32 value) : m_value(value)
  {

  }

  Checksum ::
    Checksum(const Checksum &original)
  {
    this->m_value = original.getValue();
  }

  Checksum ::
    ~Checksum()
  {

  }

  Checksum& Checksum ::
    operator=(const Checksum& checksum)
  {
    this->m_value = checksum.m_value;
    return *this;
  }

  bool Checksum ::
    operator==(const Checksum& checksum) const
  {
    return this->m_value == checksum.m_value;
  }

  bool Checksum ::
    operator!=(const Checksum& checksum) const
  {
    return not (*this == checksum);
  }

  U32 Checksum ::
    getValue() const
  {
    return this->m_value;
  }

  void Checksum ::
    update(
        const U8 *const data,
        const U32 offset,
        const U32 length
    )
  {
    U32 index = 0;

    // Add the first word unaligned if necessary
    const U32 offsetMod4 = offset % 4;
    if (offsetMod4 != 0) {
      const U8 wordLength = static_cast<U8>(min(length, 4 - offsetMod4));
      this->addWordUnaligned(
          &data[index],
          static_cast<U8>(offset + index),
          wordLength
      );
      index += wordLength;
    }

    // Add the middle words aligned
    for ( ; index + 4 <= length; index += 4) {
      addWordAligned(&data[index]);
    }

    // Add the last word unaligned if necessary
    if (index < length) {
      const U8 wordLength = static_cast<U8>(length - index);
      this->addWordUnaligned(
          &data[index],
          static_cast<U8>(offset + index),
          wordLength
      );
    }

  }

  void Checksum ::
    addWordAligned(const U8 *const word)
  {
    for (U8 i = 0; i < 4; ++i) {
      addByteAtOffset(word[i], i);
    }
  }

  void Checksum ::
    addWordUnaligned(
        const U8 *word,
        const U8 position,
        const U8 length
    )
  {
    FW_ASSERT(length < 4);
    U8 offset = position % 4;
    for (U8 i = 0; i < length; ++i) {
      addByteAtOffset(word[i], offset);
      ++offset;
      if (offset == 4) {
        offset = 0;
      }
    }
  }

  void Checksum ::
    addByteAtOffset(
        const U8 byte,
        const U8 offset
    )
  {
    FW_ASSERT(offset < 4);
    const U32 addend = static_cast<U32>(byte) << (8*(3-offset));
    this->m_value += addend;
  }

}
