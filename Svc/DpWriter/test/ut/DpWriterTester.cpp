// ======================================================================
// \title  DpWriterTester.cpp
// \author bocchino
// \brief  cpp file for DpWriter component test harness implementation class
// ======================================================================

#include "DpWriterTester.hpp"
#include "Os/Stub/test/File.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

DpWriterTester ::DpWriterTester()
    : DpWriterGTestBase("DpWriterTester", DpWriterTester::MAX_HISTORY_SIZE), component("DpWriter") {
    this->initComponents();
    this->connectPorts();
    Os::Stub::File::Test::StaticData::data.setNextStatus(Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::data.writeResult = this->abstractState.m_writeResultData;
    Os::Stub::File::Test::StaticData::data.writeResultSize = sizeof(this->abstractState.m_writeResultData);
    Os::Stub::File::Test::StaticData::data.pointer = 0;
}

DpWriterTester ::~DpWriterTester() {}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void DpWriterTester::from_procBufferSendOut_handler(NATIVE_INT_TYPE portNum, Fw::Buffer& buffer) {
    this->pushFromPortEntry_procBufferSendOut(buffer);
    this->abstractState.m_procTypes |= (1 << portNum);
}

// ----------------------------------------------------------------------
// Public member functions
// ----------------------------------------------------------------------

void DpWriterTester::printEvents() {
    this->printTextLogHistory(stdout);
}

// ----------------------------------------------------------------------
// Protected helper functions
// ----------------------------------------------------------------------

Os::File::Status DpWriterTester::pickOsFileError() {
    U32 u32Status = STest::Pick::lowerUpper(Os::File::OP_OK + 1, Os::File::MAX_STATUS - 1);
    return static_cast<Os::File::Status>(u32Status);
}

#define TESTER_CHECK_CHANNEL(NAME)                                           \
    {                                                                        \
        const auto changeStatus = this->abstractState.m_##NAME.updatePrev(); \
        if (changeStatus == TestUtils::OnChangeStatus::CHANGED) {            \
            ASSERT_TLM_##NAME##_SIZE(1);                                     \
            ASSERT_TLM_##NAME(0, this->abstractState.m_##NAME.value);        \
        } else {                                                             \
            ASSERT_TLM_##NAME##_SIZE(0);                                     \
        }                                                                    \
    }

void DpWriterTester::constructDpFileName(FwDpIdType id, const Fw::Time& timeTag, Fw::StringBase& fileName) {
    fileName.format(DP_FILENAME_FORMAT, this->component.m_dpFileNamePrefix.toChar(), id, timeTag.getSeconds(),
                    timeTag.getUSeconds());
}

void DpWriterTester::checkProcTypes(const Fw::DpContainer& container) {
    FwIndexType expectedNumProcTypes = 0;
    const Fw::DpCfg::ProcType::SerialType procTypes = container.getProcTypes();
    for (FwIndexType i = 0; i < Fw::DpCfg::ProcType::NUM_CONSTANTS; i++) {
        if (procTypes & (1 << i)) {
            ++expectedNumProcTypes;
        }
    }
    ASSERT_from_procBufferSendOut_SIZE(expectedNumProcTypes);
    ASSERT_EQ(container.getProcTypes(), this->abstractState.m_procTypes);
}

void DpWriterTester::checkTelemetry() {
    TESTER_CHECK_CHANNEL(NumBuffersReceived);
    TESTER_CHECK_CHANNEL(NumBytesWritten);
    TESTER_CHECK_CHANNEL(NumSuccessfulWrites);
    TESTER_CHECK_CHANNEL(NumFailedWrites);
    TESTER_CHECK_CHANNEL(NumErrors);
}

}  // namespace Svc
