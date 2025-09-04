// ======================================================================
// \title  TooLargeFile.cpp
// \author Rob Bocchino
// \brief  TooLargeFile implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "Svc/CmdSequencer/test/ut/SequenceFiles/TooLargeFile.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/AMPCS/AMPCS.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"

namespace Svc {

namespace SequenceFiles {

TooLargeFile ::TooLargeFile(const U32 a_bufferSize, const Format::t a_format)
    : File("too_large", a_format), bufferSize(a_bufferSize) {}

void TooLargeFile ::serializeFPrime(Fw::SerializeBufferBase& buffer) {
    // Header
    const U32 dataSize = this->getDataSize();
    const U32 numRecords = 16;
    const TimeBase timeBase = TimeBase::TB_WORKSTATION_TIME;
    const U32 timeContext = 0;
    FPrime::Headers::serialize(dataSize, numRecords, timeBase, timeContext, buffer);
}

void TooLargeFile ::serializeAMPCS(Fw::SerializeBufferBase& buffer) {
    // Header
    AMPCS::Headers::serialize(buffer);
    // Data
    const AMPCSSequence::Record::TimeFlag::t timeFlag = AMPCSSequence::Record::TimeFlag::RELATIVE;
    const AMPCSSequence::Record::Time::t time = 0;
    const U32 dataSize = this->getDataSize();
    const U32 cmdFieldSize = dataSize - sizeof(AMPCSSequence::Record::TimeFlag::Serial::t) -
                             sizeof(AMPCSSequence::Record::Time::t) - sizeof(AMPCSSequence::Record::CmdLength::t);
    U8 cmdFieldBuffer[cmdFieldSize];
    ::memset(cmdFieldBuffer, 0, cmdFieldSize);
    Fw::SerialBuffer cmdField(cmdFieldBuffer, sizeof(cmdFieldBuffer));
    cmdField.setBuffLen(cmdFieldSize);
    AMPCS::Records::serialize(timeFlag, time, cmdField, buffer);
    ASSERT_EQ(sizeof(AMPCSSequence::SequenceHeader::t) + dataSize, buffer.getBuffLength());
    // CRC
    AMPCS::CRCs::createFile(buffer, this->getName().toChar());
}

U32 TooLargeFile ::getDataSize() const {
    return 2 * this->bufferSize;
}

}  // namespace SequenceFiles

}  // namespace Svc
