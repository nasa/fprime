// ======================================================================
// \title  Frame.cpp
// \author devin
// \brief  Rule implementations for the Frame rule group
//
// These rules exercise the framing path (dataIn): SA selection from the
// frame context, SA selection from the SA_INDEX parameter, and the
// encryption-failure error path.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/CcsdsSdlsFramer/test/ut/CcsdsSdlsFramerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Frame.ContextSa
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::Frame__ContextSa__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::Frame__ContextSa__action() {
    this->clearHistory();

    // Pick an SA index distinct from the unset sentinel and set it in the context
    const U16 unsetSaIndex = ComCfg::FrameContext().get_saIndex();
    U16 sa = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
    if (sa == unsetSaIndex) {
        sa = static_cast<U16>(sa - 1);
    }
    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, static_cast<Fw::Buffer::SizeType>(STest::Pick::lowerUpper(1, TEST_BUFFER_SIZE)));
    ComCfg::FrameContext context;
    context.set_saIndex(sa);

    this->invoke_to_dataIn(0, buffer, context);

    // The context's SA index must reach the encryption helper
    ASSERT_from_encryptOut_SIZE(1);
    const FromPortEntry_encryptOut& entry = this->fromPortHistory_encryptOut->at(0);
    ASSERT_EQ(entry.securityAssociationIndex, sa);
    ASSERT_EQ(entry.context.get_saIndex(), sa);

    // Nominal path: no events, no direct data return
    ASSERT_EVENTS_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Frame.ParameterSa
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::Frame__ParameterSa__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::Frame__ParameterSa__action() {
    this->clearHistory();

    // An unset (default) context SA index must fall back to the SA_INDEX parameter
    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, static_cast<Fw::Buffer::SizeType>(STest::Pick::lowerUpper(1, TEST_BUFFER_SIZE)));
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_encryptOut_SIZE(1);
    const FromPortEntry_encryptOut& entry = this->fromPortHistory_encryptOut->at(0);
    ASSERT_EQ(entry.securityAssociationIndex, TEST_PARAM_SA_INDEX);
    ASSERT_EQ(entry.context.get_saIndex(), TEST_PARAM_SA_INDEX);

    ASSERT_EVENTS_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Frame.EncryptFailure
// ----------------------------------------------------------------------

bool CcsdsSdlsFramerTester::Frame__EncryptFailure__precondition() const {
    return true;
}

void CcsdsSdlsFramerTester::Frame__EncryptFailure__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    // Pick a random non-SUCCESS status passed forward by the encryption helper
    const Svc::Ccsds::SdlsStatus failures[] = {Svc::Ccsds::SdlsStatus::UNKNOWN_SA, Svc::Ccsds::SdlsStatus::UNKNOWN_PORT,
                                               Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE};
    const Svc::Ccsds::SdlsStatus status = failures[STest::Pick::lowerUpper(0, 2)];

    this->invoke_to_encryptIn(0, status, buffer, context);

    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EncryptionFailed_SIZE(1);
    ASSERT_EVENTS_EncryptionFailed(0, status);

    // The frame is dropped: no allocation or frame output, ownership returns to the encryption helper
    ASSERT_from_bufferAllocate_SIZE(0);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_encryptReturnOut_SIZE(1);
    ASSERT_from_encryptReturnOut(0, buffer, context);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success(Fw::Success::SUCCESS));
}

}  // namespace Ccsds

}  // namespace Svc
