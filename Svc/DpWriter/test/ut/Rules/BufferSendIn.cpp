// ======================================================================
// \title  BufferSendIn.cpp
// \author Rob Bocchino
// \brief  BufferSendIn class implementation
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#include <limits>
#include <string>

#include "Os/Stub/test/File.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpWriter/test/ut/Rules/BufferSendIn.hpp"
#include "Svc/DpWriter/test/ut/Rules/Testers.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState ::precondition__BufferSendIn__OK() const {
    const auto& fileData = Os::Stub::File::Test::StaticData::data;
    bool result = true;
    result &= (fileData.openStatus == Os::File::Status::OP_OK);
    result &= (fileData.writeStatus == Os::File::Status::OP_OK);
    return result;
}

void TestState ::action__BufferSendIn__OK() {
    // Clear the history
    this->clearHistory();
    // Reset the saved proc types
    // These are updated in the from_procBufferSendOut handler
    this->abstractState.m_procTypes = 0;
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a random buffer
    Fw::Buffer buffer = this->abstractState.getDpBuffer();
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Deserialize the container header
    Fw::DpContainer container;
    container.setBuffer(buffer);
    const Fw::SerializeStatus status = container.deserializeHeader();
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    // Check events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileWritten_SIZE(1);
    Fw::FileNameString fileName;
    this->constructDpFileName(container.getId(), container.getTimeTag(), fileName);
    ASSERT_EVENTS_FileWritten(0, buffer.getSize(), fileName.toChar());
    // Check processing types
    this->checkProcTypes(container);
    // Check DP notification
    ASSERT_from_dpWrittenOut_SIZE(1);
    ASSERT_from_dpWrittenOut(0, fileName, container.getPriority(), buffer.getSize());
    // Check deallocation
    ASSERT_from_deallocBufferSendOut_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Check file write
    ASSERT_EQ(buffer.getSize(), fileData.pointer);
    ASSERT_EQ(0, ::memcmp(buffer.getData(), fileData.writeResult, buffer.getSize()));
    // Update m_NumBytesWritten
    this->abstractState.m_NumBytesWritten.value += buffer.getSize();
    // Update m_NumSuccessfulWrites
    this->abstractState.m_NumSuccessfulWrites.value++;
}

bool TestState ::precondition__BufferSendIn__InvalidBuffer() const {
    bool result = true;
    return result;
}

void TestState ::action__BufferSendIn__InvalidBuffer() {
    // Clear the history
    this->clearHistory();
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct an invalid buffer
    Fw::Buffer buffer;
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_invalidBufferEventCount < DpWriterComponentBase::EVENTID_INVALIDBUFFER_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_InvalidBuffer_SIZE(1);
        this->abstractState.m_invalidBufferEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify no file output
    ASSERT_EQ(fileData.pointer, 0);
    // Verify no port output
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
}

bool TestState ::precondition__BufferSendIn__BufferTooSmallForPacket() const {
    bool result = true;
    return result;
}

void TestState ::action__BufferSendIn__BufferTooSmallForPacket() {
    // Clear the history
    this->clearHistory();
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a buffer that is too small to hold a data packet
    const FwSizeType minPacketSize = Fw::DpContainer::MIN_PACKET_SIZE;
    ASSERT_GT(minPacketSize, 1);
    const FwSizeType bufferSize = STest::Pick::lowerUpper(1, minPacketSize - 1);
    Fw::Buffer buffer(this->abstractState.m_bufferData, bufferSize);
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_bufferTooSmallForPacketEventCount <
        DpWriterComponentBase::EVENTID_BUFFERTOOSMALLFORPACKET_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_BufferTooSmallForPacket(0, bufferSize, minPacketSize);
        this->abstractState.m_bufferTooSmallForPacketEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify no file output
    ASSERT_EQ(fileData.pointer, 0);
    // Verify port output
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
}

bool TestState ::precondition__BufferSendIn__InvalidHeaderHash() const {
    bool result = true;
    return result;
}

void TestState ::action__BufferSendIn__InvalidHeaderHash() {
    // Clear the history
    this->clearHistory();
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a valid buffer
    Fw::Buffer buffer = this->abstractState.getDpBuffer();
    // Set up the container
    Fw::DpContainer container;
    container.setBuffer(buffer);
    // Get the header hash
    const U32 computedHash = container.getHeaderHash().asBigEndianU32();
    // Perturb the header hash
    const U32 storedHash = computedHash + 1;
    Utils::HashBuffer storedHashBuffer;
    const Fw::SerializeStatus serialStatus = storedHashBuffer.serialize(storedHash);
    ASSERT_EQ(serialStatus, Fw::FW_SERIALIZE_OK);
    container.setHeaderHash(storedHashBuffer);
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_invalidHeaderHashEventCount < DpWriterComponentBase::EVENTID_INVALIDHEADERHASH_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_InvalidHeaderHash(0, buffer.getSize(), storedHash, computedHash);
        this->abstractState.m_invalidHeaderHashEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify no file output
    ASSERT_EQ(fileData.pointer, 0);
    // Verify port output
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
}

bool TestState ::precondition__BufferSendIn__InvalidHeader() const {
    bool result = true;
    return result;
}

void TestState ::action__BufferSendIn__InvalidHeader() {
    // Clear the history
    this->clearHistory();
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a valid buffer
    Fw::Buffer buffer = this->abstractState.getDpBuffer();
    // Invalidate the packet descriptor
    U8* const buffAddr = buffer.getData();
    ASSERT_GT(static_cast<FwSizeType>(buffer.getSize()), static_cast<FwSizeType>(1));
    buffAddr[0]++;
    // Update the header hash
    Fw::DpContainer container;
    container.setBuffer(buffer);
    container.updateHeaderHash();
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_invalidHeaderEventCount < DpWriterComponentBase::EVENTID_INVALIDHEADER_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_InvalidHeader(0, buffer.getSize(), static_cast<U32>(Fw::FW_SERIALIZE_FORMAT_ERROR));
        this->abstractState.m_invalidHeaderEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify no file output
    ASSERT_EQ(fileData.pointer, 0);
    // Verify port output
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
}

bool TestState ::precondition__BufferSendIn__BufferTooSmallForData() const {
    bool result = true;
    return result;
}

void TestState ::action__BufferSendIn__BufferTooSmallForData() {
    // Clear the history
    this->clearHistory();
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a valid buffer
    Fw::Buffer buffer = this->abstractState.getDpBuffer();
    // Set up the container
    Fw::DpContainer container;
    container.setBuffer(buffer);
    // Invalidate the data size
    Fw::SerializeStatus serialStatus = container.deserializeHeader();
    ASSERT_EQ(serialStatus, Fw::FW_SERIALIZE_OK);
    const FwSizeType dataSize =
        STest::Pick::lowerUpper(AbstractState::MAX_DATA_SIZE + 1, std::numeric_limits<FwSizeStoreType>::max());
    container.setDataSize(dataSize);
    container.updateHeaderHash();
    container.serializeHeader();
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_bufferTooSmallForDataEventCount < DpWriterComponentBase::EVENTID_INVALIDHEADER_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_BufferTooSmallForData(0, buffer.getSize(), container.getPacketSize());
        this->abstractState.m_bufferTooSmallForDataEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify no file output
    ASSERT_EQ(fileData.pointer, 0);
    // Verify port output
    ASSERT_from_procBufferSendOut_SIZE(0);
    ASSERT_from_dpWrittenOut_SIZE(0);
    ASSERT_from_deallocBufferSendOut_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
}

bool TestState ::precondition__BufferSendIn__FileOpenError() const {
    const auto& fileData = Os::Stub::File::Test::StaticData::data;
    return (fileData.openStatus != Os::File::Status::OP_OK);
}

void TestState ::action__BufferSendIn__FileOpenError() {
    // Clear the history
    this->clearHistory();
    // Reset the saved proc types
    // These are updated in the from_procBufferSendOut handler
    this->abstractState.m_procTypes = 0;
    // Reset the file pointer in the stub file implementation
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    fileData.pointer = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a valid buffer
    Fw::Buffer buffer = this->abstractState.getDpBuffer();
    // Set up the container
    Fw::DpContainer container;
    container.setBuffer(buffer);
    container.deserializeHeader();
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_fileOpenErrorEventCount < DpWriterComponentBase::EVENTID_FILEOPENERROR_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        Fw::FileNameString fileName;
        this->constructDpFileName(container.getId(), container.getTimeTag(), fileName);
        const Os::File::Status openStatus = fileData.openStatus;
        ASSERT_EVENTS_FileOpenError(0, static_cast<U32>(openStatus), fileName.toChar());
        this->abstractState.m_fileOpenErrorEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify no file output
    ASSERT_EQ(fileData.pointer, 0);
    // Verify port output
    this->checkProcTypes(container);
    ASSERT_from_dpWrittenOut_SIZE(0);
    ASSERT_from_deallocBufferSendOut_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Increment m_NumFailedWrites
    this->abstractState.m_NumFailedWrites.value++;
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
}

bool TestState ::precondition__BufferSendIn__FileWriteError() const {
    const auto& fileData = Os::Stub::File::Test::StaticData::data;
    bool result = true;
    result &= (fileData.openStatus == Os::File::Status::OP_OK);
    result &= (fileData.writeStatus != Os::File::Status::OP_OK);
    return result;
}

void TestState ::action__BufferSendIn__FileWriteError() {
    // Clear the history
    this->clearHistory();
    // Reset the saved proc types
    // These are updated in the from_procBufferSendOut handler
    this->abstractState.m_procTypes = 0;
    // Update m_NumBuffersReceived
    this->abstractState.m_NumBuffersReceived.value++;
    // Construct a valid buffer
    Fw::Buffer buffer = this->abstractState.getDpBuffer();
    // Set up the container
    Fw::DpContainer container;
    container.setBuffer(buffer);
    container.deserializeHeader();
    // Get the file size
    const FwSizeType fileSize = container.getPacketSize();
    // Turn off file writing
    auto& fileData = Os::Stub::File::Test::StaticData::data;
    U8* const savedWriteResult = fileData.writeResult;
    fileData.writeResult = nullptr;
    // Adjust size result of write
    fileData.writeSizeResult = STest::Pick::lowerUpper(0, fileSize);
    // Send the buffer
    this->invoke_to_bufferSendIn(0, buffer);
    this->component.doDispatch();
    // Check events
    if (this->abstractState.m_fileWriteErrorEventCount < DpWriterComponentBase::EVENTID_FILEWRITEERROR_THROTTLE) {
        ASSERT_EVENTS_SIZE(1);
        Fw::FileNameString fileName;
        this->constructDpFileName(container.getId(), container.getTimeTag(), fileName);
        const Os::File::Status writeStatus = Os::Stub::File::Test::StaticData::data.writeStatus;
        ASSERT_EVENTS_FileWriteError(0, static_cast<U32>(writeStatus), static_cast<U32>(fileData.writeSizeResult),
                                     static_cast<U32>(fileSize), fileName.toChar());
        this->abstractState.m_fileWriteErrorEventCount++;
    } else {
        ASSERT_EVENTS_SIZE(0);
    }
    // Verify port output
    this->checkProcTypes(container);
    ASSERT_from_dpWrittenOut_SIZE(0);
    ASSERT_from_deallocBufferSendOut_SIZE(1);
    ASSERT_from_deallocBufferSendOut(0, buffer);
    // Increment m_NumFailedWrites
    this->abstractState.m_NumFailedWrites.value++;
    // Increment m_NumErrors
    this->abstractState.m_NumErrors.value++;
    // Turn on file writing
    fileData.writeResult = savedWriteResult;
}

namespace BufferSendIn {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::BufferTooSmallForData() {
    this->ruleBufferTooSmallForData.apply(this->testState);
    this->testState.printEvents();
}

void Tester::BufferTooSmallForPacket() {
    this->ruleBufferTooSmallForPacket.apply(this->testState);
    this->testState.printEvents();
}

void Tester::FileOpenError() {
    Testers::fileOpenStatus.ruleError.apply(this->testState);
    this->ruleFileOpenError.apply(this->testState);
    this->testState.printEvents();
}

void Tester::FileWriteError() {
    Testers::fileWriteStatus.ruleError.apply(this->testState);
    this->ruleFileWriteError.apply(this->testState);
    this->testState.printEvents();
}

void Tester::InvalidBuffer() {
    this->ruleInvalidBuffer.apply(this->testState);
    this->testState.printEvents();
}

void Tester::InvalidHeader() {
    this->ruleInvalidHeader.apply(this->testState);
    this->testState.printEvents();
}

void Tester::InvalidHeaderHash() {
    this->ruleInvalidHeaderHash.apply(this->testState);
    this->testState.printEvents();
}

void Tester::OK() {
    this->ruleOK.apply(this->testState);
    this->testState.printEvents();
}

}  // namespace BufferSendIn

}  // namespace Svc
