// ====================================================================== 
// \title  Buffers.cpp
// \author Rob Bocchino
// \brief  F Prime sequence file headers
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
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

#include "gtest/gtest.h"
#include "Os/File.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace Buffers {

      NATIVE_UINT_TYPE FileBuffer ::
        getBuffCapacity(void) const 
      {
        return sizeof(m_buff);
      }

      U8* FileBuffer ::
        getBuffAddr(void) 
      {
        return m_buff;
      }

      const U8* FileBuffer ::
        getBuffAddr(void) const 
      {
        return m_buff;
      }

      void write(
          const Fw::SerializeBufferBase& buffer,
          const char* fileName
      ) {
        Os::File file;
        ASSERT_EQ(file.open(fileName, Os::File::OPEN_WRITE), Os::File::OP_OK);
        NATIVE_INT_TYPE size = buffer.getBuffLength();
        const U32 expectedSize = size;
        const U8 *const buffAddr = buffer.getBuffAddr();
        ASSERT_EQ(
            file.write(buffAddr, size, true),
            Os::File::OP_OK
        );
        ASSERT_EQ(expectedSize, static_cast<U32>(size));
        file.close();
      }

    }

  }

}
