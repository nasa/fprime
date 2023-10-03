// ======================================================================
// \title  CmdSplitter.hpp
// \author mstarch
// \brief  cpp file for CmdSplitter test harness implementation class
// ======================================================================

#include "CmdSplitterTester.hpp"
#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <STest/Pick/Pick.hpp>
#include "FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CmdSplitterTester ::CmdSplitterTester() : CmdSplitterGTestBase("Tester", CmdSplitterTester::MAX_HISTORY_SIZE), component("CmdSplitter") {
    this->initComponents();
    this->connectPorts();
}

CmdSplitterTester ::~CmdSplitterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

Fw::ComBuffer CmdSplitterTester ::build_command_around_opcode(FwOpcodeType opcode) {
    Fw::ComBuffer comBuffer;
    EXPECT_EQ(comBuffer.serialize(static_cast<FwPacketDescriptorType>(Fw::ComPacket::FW_PACKET_COMMAND)), Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(comBuffer.serialize(opcode), Fw::FW_SERIALIZE_OK);

    Fw::CmdArgBuffer args;

    U32 random_size = STest::Pick::lowerUpper(0, args.getBuffCapacity());
    args.resetSer();
    for (FwSizeType i = 0; i < random_size; i++) {
        args.serialize(static_cast<U8>(STest::Pick::any()));
    }
    EXPECT_EQ(comBuffer.serialize(args), Fw::FW_SERIALIZE_OK);
    return comBuffer;
}

FwOpcodeType CmdSplitterTester ::setup_and_pick_valid_opcode(bool for_local) {
    const FwOpcodeType MAX_OPCODE = std::numeric_limits<FwOpcodeType>::max();
    if (for_local) {
        FwOpcodeType base = STest::Pick::lowerUpper(1, MAX_OPCODE);
        component.configure(base);
        EXPECT_GT(base, 0);  // Must leave some room for local commands
        return static_cast<FwOpcodeType>(
            STest::Pick::lowerUpper(0, FW_MIN(base - 1, MAX_OPCODE))
        );
    }
    FwOpcodeType base = STest::Pick::lowerUpper(0, MAX_OPCODE);
    component.configure(base);
    return static_cast<FwOpcodeType>(STest::Pick::lowerUpper(base, MAX_OPCODE));
}

void CmdSplitterTester ::test_local_routing() {
    REQUIREMENT("SVC-CMD-SPLITTER-000");
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-002");

    FwOpcodeType local_opcode = this->setup_and_pick_valid_opcode(true);
    Fw::ComBuffer testBuffer = this->build_command_around_opcode(local_opcode);

    U32 context = static_cast<U32>(STest::Pick::any());
    this->active_command_source = static_cast<NATIVE_INT_TYPE>(STest::Pick::lowerUpper(
        0, CmdSplitterPorts));
    this->invoke_to_CmdBuff(this->active_command_source, testBuffer, context);
    ASSERT_from_RemoteCmd_SIZE(0);
    ASSERT_from_LocalCmd_SIZE(1);
    ASSERT_from_LocalCmd(0, testBuffer, context);
}

void CmdSplitterTester ::test_remote_routing() {
    REQUIREMENT("SVC-CMD-SPLITTER-000");
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-003");

    FwOpcodeType remote_opcode = this->setup_and_pick_valid_opcode(false);
    Fw::ComBuffer testBuffer = this->build_command_around_opcode(remote_opcode);

    U32 context = static_cast<U32>(STest::Pick::any());
    this->active_command_source = static_cast<NATIVE_INT_TYPE>(STest::Pick::lowerUpper(
        0, CmdSplitterPorts));
    this->invoke_to_CmdBuff(this->active_command_source, testBuffer, context);
    ASSERT_from_LocalCmd_SIZE(0);
    ASSERT_from_RemoteCmd_SIZE(1);
    ASSERT_from_RemoteCmd(0, testBuffer, context);
}

void CmdSplitterTester ::test_error_routing() {
    REQUIREMENT("SVC-CMD-SPLITTER-000");
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-004");
    Fw::ComBuffer testBuffer;  // Intentionally left empty
    U32 context = static_cast<U32>(STest::Pick::any());
    this->active_command_source = static_cast<NATIVE_INT_TYPE>(STest::Pick::lowerUpper(
        0, CmdDispatcherSequencePorts));
    this->invoke_to_CmdBuff(this->active_command_source, testBuffer, context);
    ASSERT_from_RemoteCmd_SIZE(0);
    ASSERT_from_LocalCmd_SIZE(1);
    ASSERT_from_LocalCmd(0, testBuffer, context);
}

void CmdSplitterTester ::test_response_forwarding() {
    REQUIREMENT("SVC-CMD-SPLITTER-000");
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-005");

    FwOpcodeType opcode =
        static_cast<FwOpcodeType>(STest::Pick::lowerUpper(0, std::numeric_limits<FwOpcodeType>::max()));
    Fw::CmdResponse response;
    response.e = static_cast<Fw::CmdResponse::T>(STest::Pick::lowerUpper(0, Fw::CmdResponse::NUM_CONSTANTS));
    U32 cmdSeq = static_cast<U32>(STest::Pick::any());
    this->active_command_source = static_cast<NATIVE_INT_TYPE>(STest::Pick::lowerUpper(
        0, CmdDispatcherSequencePorts));

    this->invoke_to_seqCmdStatus(this->active_command_source, opcode, cmdSeq, response);
    ASSERT_from_forwardSeqCmdStatus_SIZE(1);
    ASSERT_from_forwardSeqCmdStatus(0, opcode, cmdSeq, response);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void CmdSplitterTester ::from_LocalCmd_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    EXPECT_EQ(this->active_command_source, portNum) << "Command source not respected";
    this->pushFromPortEntry_LocalCmd(data, context);
}

void CmdSplitterTester ::from_RemoteCmd_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    EXPECT_EQ(this->active_command_source, portNum) << "Command source not respected";
    this->pushFromPortEntry_RemoteCmd(data, context);
}

void CmdSplitterTester ::from_forwardSeqCmdStatus_handler(const NATIVE_INT_TYPE portNum,
                                               FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdResponse& response) {
    EXPECT_EQ(this->active_command_source, portNum) << "Command source not respected";
    this->pushFromPortEntry_forwardSeqCmdStatus(opCode, cmdSeq, response);
}

}  // end namespace Svc
