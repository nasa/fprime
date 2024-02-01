// ======================================================================
// \title  CRCs.hpp
// \author Rob Bocchino
// \brief  AMPCS CRC files
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Types/String.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/CRCs.hpp"
#include "gtest/gtest.h"

#define BUFFER_SIZE 256

namespace Svc {

  namespace SequenceFiles {

    namespace AMPCS {

      namespace CRCs {

        namespace {

          //! Open a file
          void openFile(
              Os::File& file, //!< The file
              const char *const fileName, //!< The file name
              const Os::File::Mode mode //!< The mode
          ) {
            const Os::File::Status fileStatus =
              file.open(fileName, mode);
            ASSERT_EQ(Os::File::OP_OK, fileStatus);
          }

          //! Write a file
          void writeFile(
              Os::File& file, //!< The file
              const U8 *buffer, //!< The buffer
              const U32 size //!< The number of bytes to write
          ) {
            FwSignedSizeType sizeThenActualSize = size;
            const Os::File::Status status = file.write(
                buffer,
                sizeThenActualSize,
                Os::File::WaitType::WAIT
            );
            ASSERT_EQ(Os::File::OP_OK, status);
            const U32 actualSize = sizeThenActualSize;
            ASSERT_EQ(size, actualSize);
          }

        }

        void createFile(
            Fw::SerializeBufferBase& buffer,
            const char *const fileName
        ) {
          CRC crc;
          computeCRC(buffer, crc);
          writeCRC(crc.m_computed, fileName);
        }

        void computeCRC(
            Fw::SerializeBufferBase& buffer,
            CRC& crc
        ) {
          crc.init();
          const U8 *const addr = buffer.getBuffAddr();
          const U32 size = buffer.getBuffLength();
          crc.update(addr, size);
          crc.finalize();
        }

        void removeFile(
            const char *const fileName
        ) {
          Fw::String s("rm -f ");
          s += fileName;
          s += ".CRC32";
          int status = system(s.toChar());
          ASSERT_EQ(0, status);
        }

        void writeCRC(
            const U32 crc,
            const char *const fileName
        ) {
          Os::File file;
          U8 buffer[sizeof crc];
          Fw::SerialBuffer serialBuffer(buffer, sizeof(buffer));
          serialBuffer.serialize(crc);
          const U8 *const addr = serialBuffer.getBuffAddr();
          Fw::String hashFileName(fileName);
          hashFileName += ".CRC32";
          openFile(file, hashFileName.toChar(), Os::File::OPEN_WRITE);
          writeFile(file, addr, sizeof(crc));
          file.close();
        }

      }

    }

  }

}
