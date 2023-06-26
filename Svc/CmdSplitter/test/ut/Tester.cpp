// ======================================================================
// \title  CmdSplitter.hpp
// \author mstarch
// \brief  cpp file for CmdSplitter test harness implementation class
// ======================================================================

#include "Tester.hpp"
#include <CmdSplitterCfg.hpp>
#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <STest/Pick/Pick.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester() : CmdSplitterGTestBase("Tester", Tester::MAX_HISTORY_SIZE), component("CmdSplitter") {
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

Fw::ComBuffer Tester ::build_command_around_opcode(FwOpcodeType opcode) {
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

void Tester ::test_local_routing() {
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-002");

    ASSERT_GT(Svc::CMD_SPLITTER_REMOTE_OPCODE_BASE, 0);  // Must leave some room for local commands
    FwOpcodeType local_opcode = static_cast<FwOpcodeType>(STest::Pick::lowerUpper(
        0, FW_MIN(Svc::CMD_SPLITTER_REMOTE_OPCODE_BASE - 1, std::numeric_limits<FwOpcodeType>::max())));
    Fw::ComBuffer testBuffer = this->build_command_around_opcode(local_opcode);

    U32 context = static_cast<U32>(STest::Pick::any());
    this->invoke_to_CmdBuff(0, testBuffer, context);
    ASSERT_from_RemoteCmd_SIZE(0);
    ASSERT_from_LocalCmd_SIZE(1);
    ASSERT_from_LocalCmd(0, testBuffer, context);
}

void Tester ::test_remote_routing() {
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-003");

    ASSERT_LT(Svc::CMD_SPLITTER_REMOTE_OPCODE_BASE,
              std::numeric_limits<FwOpcodeType>::max());  // Must leave some room for remote commands
    FwOpcodeType local_opcode = static_cast<FwOpcodeType>(
        STest::Pick::lowerUpper(Svc::CMD_SPLITTER_REMOTE_OPCODE_BASE, std::numeric_limits<FwOpcodeType>::max()));
    Fw::ComBuffer testBuffer = this->build_command_around_opcode(local_opcode);

    U32 context = static_cast<U32>(STest::Pick::any());
    this->invoke_to_CmdBuff(0, testBuffer, context);
    ASSERT_from_LocalCmd_SIZE(0);
    ASSERT_from_RemoteCmd_SIZE(1);
    ASSERT_from_RemoteCmd(0, testBuffer, context);
}

void Tester ::test_error_routing() {
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-004");
    Fw::ComBuffer testBuffer;  // Intentionally left empty
    U32 context = static_cast<U32>(STest::Pick::any());
    this->invoke_to_CmdBuff(0, testBuffer, context);
    ASSERT_from_RemoteCmd_SIZE(0);
    ASSERT_from_LocalCmd_SIZE(1);
    ASSERT_from_LocalCmd(0, testBuffer, context);
}

void Tester ::test_response_forwarding() {
    REQUIREMENT("SVC-CMD-SPLITTER-001");
    REQUIREMENT("SVC-CMD-SPLITTER-005");

    FwOpcodeType opcode =
        static_cast<FwOpcodeType>(STest::Pick::lowerUpper(0, std::numeric_limits<FwOpcodeType>::max()));
    Fw::CmdResponse response;
    response.e = static_cast<Fw::CmdResponse::T>(STest::Pick::lowerUpper(0, Fw::CmdResponse::NUM_CONSTANTS));
    U32 cmdSeq = static_cast<U32>(STest::Pick::any());
    this->invoke_to_seqCmdStatus(0, opcode, cmdSeq, response);
    ASSERT_from_forwardSeqCmdStatus_SIZE(1);
    ASSERT_from_forwardSeqCmdStatus(0, opcode, cmdSeq, response);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_LocalCmd_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_LocalCmd(data, context);
}

void Tester ::from_RemoteCmd_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_RemoteCmd(data, context);
}

void Tester ::from_forwardSeqCmdStatus_handler(const NATIVE_INT_TYPE portNum,
                                               FwOpcodeType opCode,
                                               U32 cmdSeq,
                                               const Fw::CmdResponse& response) {
    this->pushFromPortEntry_forwardSeqCmdStatus(opCode, cmdSeq, response);
}

}  // end namespace Svc
