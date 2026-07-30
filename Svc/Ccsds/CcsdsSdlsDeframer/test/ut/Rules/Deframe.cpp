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

#include "Fw/Types/Serializable.hpp"
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
    Fw::ExternalSerializeBuffer saWriter(storage, sizeof(U16));
    ASSERT_EQ(saWriter.serializeFrom(sa), Fw::FW_SERIALIZE_OK);
    U8 payload[TEST_BUFFER_SIZE];
    for (FwSizeType i = 0; i < payloadSize; i++) {
        payload[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
        storage[sizeof(U16) + i] = payload[i];
    }
    Fw::Buffer buffer(storage, static_cast<Fw::Buffer::SizeType>(sizeof(U16) + payloadSize));
    ComCfg::FrameContext context;

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

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    // Pick a random non-SUCCESS status passed forward by the decryption helper
    const Svc::Ccsds::SdlsStatus failures[] = {Svc::Ccsds::SdlsStatus::UNKNOWN_SA, Svc::Ccsds::SdlsStatus::UNKNOWN_PORT,
                                               Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE};
    const Svc::Ccsds::SdlsStatus status = failures[STest::Pick::lowerUpper(0, 2)];

    this->invoke_to_decryptIn(0, status, buffer, context);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DecryptionFailed_SIZE(1);
    ASSERT_EVENTS_DecryptionFailed(0, status);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SDLS_DECRYPTION_FAILURE);

    // The failed buffer is dropped: ownership returns to the decryption subsystem, no data downstream
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_decryptReturnOut_SIZE(1);
    ASSERT_from_decryptReturnOut(0, buffer, context);
    ASSERT_from_dataReturnOut_SIZE(0);
}

}  // namespace Ccsds

}  // namespace Svc
