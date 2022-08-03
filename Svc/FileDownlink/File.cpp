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
    this->sourceName = sourceLogStringArg;

    // Set dest name
    Fw::LogStringArg destLogStringArg(destFileName);
    this->destName = destLogStringArg;

    // Set size
    U64 size;
    const Os::FileSystem::Status status = 
      Os::FileSystem::getFileSize(sourceFileName, size);
    if (status != Os::FileSystem::OP_OK)
      return Os::File::BAD_SIZE;
    this->size = static_cast<U32>(size);

    // Initialize checksum
    CFDP::Checksum checksum;
    this->checksum = checksum;

    // Open osFile for reading
    return this->osFile.open(sourceFileName, Os::File::OPEN_READ);

  }

  Os::File::Status FileDownlink::File ::
    read(
        U8 *const data,
        const U32 byteOffset,
        const U32 size
    )
  {

    Os::File::Status status;
    status = this->osFile.seek(byteOffset);
    if (status != Os::File::OP_OK)
      return status;

    NATIVE_INT_TYPE intSize = size;
    status = this->osFile.read(data, intSize);
    if (status != Os::File::OP_OK)
      return status;
    FW_ASSERT(static_cast<U32>(intSize) == size);

    this->checksum.update(data, byteOffset, size);

    return Os::File::OP_OK;

  }
}
