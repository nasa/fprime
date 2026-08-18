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

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/FileDownlink/FileDownlink.hpp>

namespace Svc {

Os::File::Status FileDownlink::File ::open(const Fw::FileNameString& sourceFileName,
                                           const Fw::FileNameString& destFileName) {
    // Set source name
    Fw::LogStringArg sourceLogStringArg(sourceFileName);
    this->m_sourceName = sourceLogStringArg;

    // Set dest name
    Fw::LogStringArg destLogStringArg(destFileName);
    this->m_destName = destLogStringArg;

    // Initialize checksum
    CFDP::Checksum checksum;
    this->m_checksum = checksum;

    // Open osFile for reading (sandbox path validation happens here)
    Os::File::Status status = this->m_osFile.open(sourceFileName.toChar(), Os::File::OPEN_READ);
    if (status != Os::File::OP_OK) {
        return status;
    }

    // Query size only after the path passes sandbox checks
    FwSizeType file_size;
    status = this->m_osFile.size(file_size);
    if (status != Os::File::OP_OK) {
        this->m_osFile.close();
        return Os::File::BAD_SIZE;
    }
    // If the size does not cast cleanly to the desired U32 type, return size error
    if (static_cast<FwSizeType>(static_cast<U32>(file_size)) != file_size) {
        this->m_osFile.close();
        return Os::File::BAD_SIZE;
    }
    this->m_size = static_cast<U32>(file_size);
    return Os::File::OP_OK;
}

Os::File::Status FileDownlink::File ::read(U8* const data, const U32 byteOffset, const U32 size) {
    Os::File::Status status;
    status = this->m_osFile.seek(byteOffset, Os::File::SeekType::ABSOLUTE);
    if (status != Os::File::OP_OK) {
        return status;
    }

    FwSizeType intSize = size;
    status = this->m_osFile.read(data, intSize);

    if (status != Os::File::OP_OK) {
        return status;
    }
    // Force a bad size error when the U32 carrying size is bad
    if (static_cast<U32>(intSize) != size) {
        return Os::File::BAD_SIZE;
    }
    this->m_checksum.update(data, byteOffset, size);

    return Os::File::OP_OK;
}
}  // namespace Svc
