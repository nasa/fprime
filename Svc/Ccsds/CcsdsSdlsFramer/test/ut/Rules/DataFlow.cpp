// ======================================================================
// \title  DataFlow.cpp
// \author devin
// \brief  Rule implementations for the DataFlow rule group
//
// These rules exercise the encrypted data path (encryptIn -> allocation,
// SA prepend, dataOut), the allocation-failure error path, the ownership
// return paths (dataReturnIn -> bufferDeallocate and bufferReturnIn ->
// dataReturnOut), and the comStatus pass-through.
// ======================================================================

#include <cstring>

#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/CcsdsSdlsFramer/test/ut/CcsdsSdlsFramerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// DataFlow.EncryptedData
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::DataFlow__EncryptedData__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::DataFlow__EncryptedData__action() {
    this->clearHistory();

    const U16 sa = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
    const FwSizeType dataSize = STest::Pick::lowerUpper(1, TEST_BUFFER_SIZE);
    U8 storage[TEST_BUFFER_SIZE];
    for (FwSizeType i = 0; i < dataSize; i++) {
        storage[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
    }
    Fw::Buffer buffer(storage, static_cast<Fw::Buffer::SizeType>(dataSize));
    ComCfg::FrameContext context;
    context.set_saIndex(sa);

    this->m_allocateUndersized = false;
    this->invoke_to_encryptIn(0, Svc::Ccsds::SdlsStatus::SUCCESS, buffer, context);

    // A frame buffer of encrypted size plus the SA index must be allocated
    ASSERT_from_bufferAllocate_SIZE(1);
    ASSERT_from_bufferAllocate(0, static_cast<FwSizeType>(dataSize + sizeof(U16)));

    // The frame must carry the big-endian SA index followed by the encrypted data
    ASSERT_from_dataOut_SIZE(1);
    const FromPortEntry_dataOut& entry = this->fromPortHistory_dataOut->at(0);
    ASSERT_EQ(entry.data.getSize(), dataSize + sizeof(U16));
    ASSERT_EQ(entry.data.getData()[0], static_cast<U8>(sa >> 8));
    ASSERT_EQ(entry.data.getData()[1], static_cast<U8>(sa & 0xFF));
    ASSERT_EQ(::memcmp(&entry.data.getData()[sizeof(U16)], storage, dataSize), 0);

    // Ownership of the encrypted buffer must return to the encryption helper
    ASSERT_from_encryptReturnOut_SIZE(1);
    ASSERT_from_encryptReturnOut(0, buffer, context);

    ASSERT_EVENTS_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(0);
}

// ----------------------------------------------------------------------
// DataFlow.AllocationFailure
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::DataFlow__AllocationFailure__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::DataFlow__AllocationFailure__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    // Fail the allocation either with an undersized buffer or an invalid one
    const bool invalid = (STest::Pick::lowerUpper(0, 1) == 1);
    this->m_allocateInvalid = invalid;
    this->m_allocateUndersized = !invalid;
    this->invoke_to_encryptIn(0, Svc::Ccsds::SdlsStatus::SUCCESS, buffer, context);
    this->m_allocateInvalid = false;
    this->m_allocateUndersized = false;

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_BufferAllocationFailed_SIZE(1);

    // The frame must be dropped: undersized allocation deallocated (invalid buffers are not
    // deallocated), encrypted buffer returned
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_bufferDeallocate_SIZE(invalid ? 0 : 1);
    ASSERT_from_encryptReturnOut_SIZE(1);
    ASSERT_from_encryptReturnOut(0, buffer, context);
}

// ----------------------------------------------------------------------
// DataFlow.DataReturn
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::DataFlow__DataReturn__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::DataFlow__DataReturn__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_dataReturnIn(0, buffer, context);

    // The allocated frame buffer must be deallocated when it returns from downstream
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_from_bufferDeallocate(0, buffer);
}

// ----------------------------------------------------------------------
// DataFlow.BufferReturn
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::DataFlow__BufferReturn__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::DataFlow__BufferReturn__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_bufferReturnIn(0, buffer, context);

    // The original data buffer must be returned upstream via the framer return port
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
}

// ----------------------------------------------------------------------
// DataFlow.ComStatus
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::DataFlow__ComStatus__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::DataFlow__ComStatus__action() {
    this->clearHistory();

    Fw::Success condition = (STest::Pick::lowerUpper(0, 1) == 0) ? Fw::Success::SUCCESS : Fw::Success::FAILURE;

    this->invoke_to_comStatusIn(0, condition);

    // The com status must pass through unmodified
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, condition);
}

}  // namespace Ccsds

}  // namespace Svc
