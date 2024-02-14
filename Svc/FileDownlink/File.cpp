// ======================================================================
// \title  File.cpp
// \author bocchino
// \brief  cpp file for FileDownlink::File
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/FileDownlink/FileDownlink.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>
#include <Os/FileSystem.hpp>

namespace Svc {

  Os::File::Status FileDownlink::File ::
    open(
        const char *const sourceFileName,
        const char *const destFileName
    )
  {

    // Set source name
    Fw::LogStringArg sourceLogStringArg(sourceFileName);
    this->m_sourceName = sourceLogStringArg;

    // Set dest name
    Fw::LogStringArg destLogStringArg(destFileName);
    this->m_destName = destLogStringArg;

    // Set size
    FwSizeType file_size;
    const Os::FileSystem::Status status =
      Os::FileSystem::getFileSize(sourceFileName, file_size);
    if (status != Os::FileSystem::OP_OK)
      return Os::File::BAD_SIZE;
    // If the size does not cast cleanly to the desired U32 type, return size error
    if (static_cast<FwSizeType>(static_cast<U32>(file_size)) != file_size) {
        return Os::File::BAD_SIZE;
    }
    this->m_size = static_cast<U32>(file_size);

    // Initialize checksum
    CFDP::Checksum checksum;
    this->m_checksum = checksum;

    // Open osFile for reading
    return this->m_osFile.open(sourceFileName, Os::File::OPEN_READ);

  }

  Os::File::Status FileDownlink::File ::
    read(
        U8 *const data,
        const U32 byteOffset,
        const U32 size
    )
  {

    Os::File::Status status;
    status = this->m_osFile.seek(byteOffset);
    if (status != Os::File::OP_OK)
      return status;

    NATIVE_INT_TYPE intSize = size;
    status = this->m_osFile.read(data, intSize);
    if (status != Os::File::OP_OK)
      return status;
    // Force a bad size error when the U32 carrying size is bad
    if (static_cast<U32>(intSize) != size) {
        return Os::File::BAD_SIZE;
    }
    this->m_checksum.update(data, byteOffset, size);

    return Os::File::OP_OK;

  }
}
