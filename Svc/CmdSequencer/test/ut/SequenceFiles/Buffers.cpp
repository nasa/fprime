// ======================================================================
// \title  Buffers.cpp
// \author Rob Bocchino
// \brief  F Prime sequence file headers
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "gtest/gtest.h"
#include "Os/File.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace Buffers {

      NATIVE_UINT_TYPE FileBuffer ::
        getBuffCapacity() const
      {
        return sizeof(m_buff);
      }

      U8* FileBuffer ::
        getBuffAddr()
      {
        return m_buff;
      }

      const U8* FileBuffer ::
        getBuffAddr() const
      {
        return m_buff;
      }

      void write(
          const Fw::SerializeBufferBase& buffer,
          const char* fileName
      ) {
        Os::File file;
        ASSERT_EQ(file.open(fileName, Os::File::OPEN_WRITE), Os::File::OP_OK);
        FwSignedSizeType size = buffer.getBuffLength();
        const U32 expectedSize = size;
        const U8 *const buffAddr = buffer.getBuffAddr();
        ASSERT_EQ(
            file.write(buffAddr, size, Os::File::WaitType::WAIT),
            Os::File::OP_OK
        );
        ASSERT_EQ(expectedSize, static_cast<U32>(size));
        file.close();
      }

    }

  }

}
