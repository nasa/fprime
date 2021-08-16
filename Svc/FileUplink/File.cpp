// ======================================================================
// \title  File.cpp
// \author bocchino
// \brief  cpp file for FileUplink::File
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/FileUplink/FileUplink.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Svc {

  Os::File::Status FileUplink::File ::
    open(const Fw::FilePacket::StartPacket& startPacket)
  {
    const U32 length = startPacket.destinationPath.length;
    char path[Fw::FilePacket::PathName::MAX_LENGTH + 1];
    memcpy(path, startPacket.destinationPath.value, length);
    path[length] = 0;
    Fw::LogStringArg logStringArg(path);
    this->name = logStringArg;
    this->size = startPacket.fileSize;
    CFDP::Checksum checksum;
    this->checksum = checksum;
    return this->osFile.open(path, Os::File::OPEN_WRITE);
  }

  Os::File::Status FileUplink::File ::
    write(
        const U8 *const data,
        const U32 byteOffset,
        const U32 length
    )
  {

    Os::File::Status status;
    status = this->osFile.seek(byteOffset);
    if (status != Os::File::OP_OK) {
        return status;
    }

    NATIVE_INT_TYPE intLength = length;
    //Note: not waiting for the file write to finish
    status = this->osFile.write(data, intLength, false);
    if (status != Os::File::OP_OK) {
        return status;
    }

    FW_ASSERT(static_cast<U32>(intLength) == length, intLength);
    this->checksum.update(data, byteOffset, length);
    return Os::File::OP_OK;

  }

}
