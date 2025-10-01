// ======================================================================
// \title  FileBuffer.hpp
// \author bocchino
// \brief  cpp file for FileDownlinkTester::FileBuffer
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <cstring>

#include "FileDownlinkTester.hpp"

namespace Svc {

FileDownlinkTester::FileBuffer ::FileBuffer(const U8* const data, const size_t size) : m_index(0) {
    this->push(data, size);
    FW_ASSERT(this->m_index == size);
}

FileDownlinkTester::FileBuffer ::FileBuffer(const History<Fw::FilePacket::DataPacket>& dataPackets) : m_index(0) {
    size_t numPackets = dataPackets.size();
    for (size_t i = 0; i < numPackets; ++i) {
        const Fw::FilePacket::DataPacket& dataPacket = dataPackets.at(i);
        this->push(dataPacket.m_data, dataPacket.m_dataSize);
    }
}

void FileDownlinkTester::FileBuffer ::push(const U8* const data, const size_t size) {
    FW_ASSERT(this->m_index + size <= FILE_BUFFER_CAPACITY);
    memcpy(&this->m_data[this->m_index], data, size);
    this->m_index += size;
}

void FileDownlinkTester::FileBuffer ::write(const char* const fileName) {
    Os::File::Status status;
    Os::File file;

    status = file.open(fileName, Os::File::OPEN_WRITE);
    FW_ASSERT(status == Os::File::OP_OK);

    const U32 size = this->m_index;
    FwSizeType intSize = size;
    status = file.write(this->m_data, intSize);
    FW_ASSERT(status == Os::File::OP_OK);
    FW_ASSERT(static_cast<U32>(intSize) == size);

    file.close();
}

void FileDownlinkTester::FileBuffer ::getChecksum(CFDP::Checksum& checksum) {
    CFDP::Checksum c;
    c.update(this->m_data, 0, this->m_index);
    checksum = c;
}

bool FileDownlinkTester::FileBuffer ::compare(const FileBuffer& fb1, const FileBuffer& fb2) {
    if (fb1.m_index != fb2.m_index) {
        fprintf(stderr, "FileBuffer: sizes do not match (%lu vs %lu)\n", fb1.m_index, fb2.m_index);
        return false;
    }

    FW_ASSERT(fb1.m_index <= FILE_BUFFER_CAPACITY);
    if (memcmp(fb1.m_data, fb2.m_data, fb1.m_index) != 0) {
        fprintf(stderr, "FileBuffer: data does not match\n");
        return false;
    }

    return true;
}

}  // namespace Svc
