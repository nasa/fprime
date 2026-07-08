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

#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Os/File.hpp"
#include "gtest/gtest.h"

namespace Svc {

namespace SequenceFiles {

namespace Buffers {

FileBuffer::FileBuffer() : Fw::LinearBufferBase() {
    this->m_buffAddr = this->m_buff;
    this->m_capacity = sizeof(this->m_buff);
}

FileBuffer::FileBuffer(const FileBuffer& other) : Fw::LinearBufferBase() {
    this->m_buffAddr = this->m_buff;
    this->m_capacity = sizeof(this->m_buff);
    Fw::SerializeStatus stat = Fw::LinearBufferBase::setBuff(other.m_buff, other.m_serLoc);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == stat, static_cast<FwAssertArgType>(stat));
}

FwSizeType FileBuffer ::getBuffCapacity() const {
    return this->getCapacity();
}

void write(const Fw::LinearBufferBase& buffer, const char* fileName) {
    Os::File file;
    ASSERT_EQ(file.open(fileName, Os::File::OPEN_WRITE), Os::File::OP_OK);
    FwSizeType size = buffer.getSize();
    const U32 expectedSize = size;
    const U8* const buffAddr = buffer.getBuffAddr();
    ASSERT_EQ(file.write(buffAddr, size, Os::File::WaitType::WAIT), Os::File::OP_OK);
    ASSERT_EQ(expectedSize, static_cast<U32>(size));
    file.close();
}

}  // namespace Buffers

}  // namespace SequenceFiles

}  // namespace Svc
