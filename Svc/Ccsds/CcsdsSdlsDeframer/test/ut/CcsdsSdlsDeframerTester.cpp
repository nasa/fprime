// ======================================================================
// \title  CcsdsSdlsDeframerTester.cpp
// \author lestarch-autobot
// \brief  cpp file for CcsdsSdlsDeframer component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/CcsdsSdlsDeframer/test/ut/CcsdsSdlsDeframerTester.hpp"

#include <cstring>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CcsdsSdlsDeframerTester ::CcsdsSdlsDeframerTester()
    : CcsdsSdlsDeframerGTestBase("CcsdsSdlsDeframerTester", CcsdsSdlsDeframerTester::MAX_HISTORY_SIZE),
      component("CcsdsSdlsDeframer") {
    this->initComponents();
    this->connectPorts();
}

CcsdsSdlsDeframerTester ::~CcsdsSdlsDeframerTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Handler overrides for typed from ports
// ----------------------------------------------------------------------

void CcsdsSdlsDeframerTester ::from_decryptOut_handler(FwIndexType portNum,
                                                       U16 securityAssociationIndex,
                                                       Fw::Buffer& data,
                                                       const ComCfg::FrameContext& context) {
    this->m_lastDecryptSize = data.getSize();
    if (data.getSize() <= TEST_BUFFER_SIZE) {
        (void)::memcpy(this->m_lastDecryptData, data.getData(), data.getSize());
    }
    this->pushFromPortEntry_decryptOut(securityAssociationIndex, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
