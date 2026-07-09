// ======================================================================
// \title  DataFlow.cpp
// \author lestarch-autobot
// \brief  Rule implementations for the DataFlow rule group
//
// These rules exercise the decrypted data path (decryptIn -> dataOut) and
// the ownership return paths (dataReturnIn -> decryptReturnOut and
// bufferReturnIn -> dataReturnOut).
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/CcsdsSdlsDeframer/test/ut/CcsdsSdlsDeframerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// DataFlow.DecryptedData
// ----------------------------------------------------------------------

bool CcsdsSdlsDeframerTester::DataFlow__DecryptedData__precondition() const {
    return true;
}

void CcsdsSdlsDeframerTester::DataFlow__DecryptedData__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_decryptIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataOut(0, buffer, context);
}

// ----------------------------------------------------------------------
// DataFlow.DataReturn
// ----------------------------------------------------------------------

bool CcsdsSdlsDeframerTester::DataFlow__DataReturn__precondition() const {
    return true;
}

void CcsdsSdlsDeframerTester::DataFlow__DataReturn__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_dataReturnIn(0, buffer, context);

    ASSERT_from_decryptReturnOut_SIZE(1);
    ASSERT_from_decryptReturnOut(0, buffer, context);
}

// ----------------------------------------------------------------------
// DataFlow.BufferReturn
// ----------------------------------------------------------------------

bool CcsdsSdlsDeframerTester::DataFlow__BufferReturn__precondition() const {
    return true;
}

void CcsdsSdlsDeframerTester::DataFlow__BufferReturn__action() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_bufferReturnIn(0, buffer, context);

    // The original frame buffer must be returned upstream via the deframer return port
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
}

}  // namespace Ccsds

}  // namespace Svc
