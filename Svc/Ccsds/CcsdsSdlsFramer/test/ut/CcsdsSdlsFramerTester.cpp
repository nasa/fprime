// ======================================================================
// \title  CcsdsSdlsFramerTester.cpp
// \author devin
// \brief  cpp file for CcsdsSdlsFramer component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/CcsdsSdlsFramer/test/ut/CcsdsSdlsFramerTester.hpp"

namespace Svc {

namespace Ccsds {

const U16 CcsdsSdlsFramerTester::TEST_PARAM_SA_INDEX;

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CcsdsSdlsFramerTester ::CcsdsSdlsFramerTester()
    : CcsdsSdlsFramerGTestBase("CcsdsSdlsFramerTester", CcsdsSdlsFramerTester::MAX_HISTORY_SIZE),
      component("CcsdsSdlsFramer") {
    this->initComponents();
    this->connectPorts();
    // Stage the SA_INDEX parameter and load it into the component
    this->paramSet_SA_INDEX(TEST_PARAM_SA_INDEX, Fw::ParamValid::VALID);
    this->component.loadParameters();
}

CcsdsSdlsFramerTester ::~CcsdsSdlsFramerTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Handler overrides for typed from ports
// ----------------------------------------------------------------------

void CcsdsSdlsFramerTester ::from_encryptOut_handler(FwIndexType portNum,
                                                     U16 securityAssociationIndex,
                                                     Fw::Buffer& data,
                                                     const ComCfg::FrameContext& context) {
    this->pushFromPortEntry_encryptOut(securityAssociationIndex, data, context);
}

Fw::Buffer CcsdsSdlsFramerTester ::from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) {
    this->pushFromPortEntry_bufferAllocate(size);
    if (this->m_allocateInvalid) {
        return Fw::Buffer();
    }
    if (this->m_allocateUndersized) {
        return Fw::Buffer(this->m_allocationStorage, static_cast<Fw::Buffer::SizeType>(size - 1));
    }
    FW_ASSERT(size <= TEST_ALLOCATION_SIZE, static_cast<FwAssertArgType>(size));
    return Fw::Buffer(this->m_allocationStorage, static_cast<Fw::Buffer::SizeType>(size));
}

}  // namespace Ccsds

}  // namespace Svc
