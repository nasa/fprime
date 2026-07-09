// ======================================================================
// \title  Deframe.cpp
// \author lestarch-autobot
// \brief  Rule implementations for the Deframe rule group
//
// These rules exercise the deframing path (dataIn): nominal SA extraction
// and forwarding to the decryption helper, the insufficient-length error
// path, and the decryption-failure error path.
// ======================================================================

#include <cstring>

#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/CcsdsSdlsDeframer/test/ut/CcsdsSdlsDeframerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Deframe.Nominal
// ----------------------------------------------------------------------

bool CcsdsSdlsDeframerTester::Deframe__Nominal__precondition() const {
    return true;
}

void CcsdsSdlsDeframerTester::Deframe__Nominal__action() {
    this->clearHistory();

    // Build a frame: leading big-endian U16 SA index followed by a random payload
    const U16 sa = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
    const FwSizeType payloadSize = STest::Pick::lowerUpper(0, TEST_BUFFER_SIZE - sizeof(U16));
    U8 storage[TEST_BUFFER_SIZE];
    storage[0] = static_cast<U8>(sa >> 8);
    storage[1] = static_cast<U8>(sa & 0xFF);
    U8 payload[TEST_BUFFER_SIZE];
    for (FwSizeType i = 0; i < payloadSize; i++) {
        payload[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
        storage[sizeof(U16) + i] = payload[i];
    }
    Fw::Buffer buffer(storage, static_cast<Fw::Buffer::SizeType>(sizeof(U16) + payloadSize));
    ComCfg::FrameContext context;

    this->m_decryptStatus = Svc::Ccsds::SdlsStatus::SUCCESS;
    this->invoke_to_dataIn(0, buffer, context);

    // The SA index and updated context must reach the decryption helper
    ASSERT_from_decryptOut_SIZE(1);
    const FromPortEntry_decryptOut& entry = this->fromPortHistory_decryptOut->at(0);
    ASSERT_EQ(entry.securityAssociationIndex, sa);
    ASSERT_EQ(entry.context.get_saIndex(), sa);

    // The SA index must be removed: only the payload goes to the decryption helper
    ASSERT_EQ(this->m_lastDecryptSize, payloadSize);
    ASSERT_EQ(::memcmp(this->m_lastDecryptData, payload, payloadSize), 0);

    // Nominal path: no events, no error notifications, no buffer return
    ASSERT_EVENTS_SIZE(0);
    ASSERT_from_errorNotify_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Deframe.InsufficientLength
// ----------------------------------------------------------------------

bool CcsdsSdlsDeframerTester::Deframe__InsufficientLength__precondition() const {
    return true;
}

void CcsdsSdlsDeframerTester::Deframe__InsufficientLength__action() {
    this->clearHistory();

    // A frame shorter than the 16-bit SA index cannot be deframed
    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, static_cast<Fw::Buffer::SizeType>(STest::Pick::lowerUpper(0, sizeof(U16) - 1)));
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InsufficientLength_SIZE(1);
    ASSERT_from_decryptOut_SIZE(0);

    // The undeframable buffer must still be returned via the return port
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
}

// ----------------------------------------------------------------------
// Deframe.DecryptFailure
// ----------------------------------------------------------------------

bool CcsdsSdlsDeframerTester::Deframe__DecryptFailure__precondition() const {
    return true;
}

void CcsdsSdlsDeframerTester::Deframe__DecryptFailure__action() {
    this->clearHistory();

    const U16 sa = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
    U8 storage[TEST_BUFFER_SIZE];
    storage[0] = static_cast<U8>(sa >> 8);
    storage[1] = static_cast<U8>(sa & 0xFF);
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    // Stage a random non-SUCCESS status from the decryption helper
    const Svc::Ccsds::SdlsStatus failures[] = {Svc::Ccsds::SdlsStatus::UNKNOWN_SA, Svc::Ccsds::SdlsStatus::UNKNOWN_PORT,
                                               Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE};
    this->m_decryptStatus = failures[STest::Pick::lowerUpper(0, 2)];

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_decryptOut_SIZE(1);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DecryptionFailed_SIZE(1);
    ASSERT_EVENTS_DecryptionFailed(0, this->m_decryptStatus);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SDLS_DECRYPTION_FAILURE);

    // The failing decryptor returns the buffer via bufferReturnIn: no direct return here
    ASSERT_from_dataReturnOut_SIZE(0);

    this->m_decryptStatus = Svc::Ccsds::SdlsStatus::SUCCESS;
}

}  // namespace Ccsds

}  // namespace Svc
