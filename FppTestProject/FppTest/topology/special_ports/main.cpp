// ======================================================================
// \title  main.cpp
// \author tumbar
// \brief  Main program for FppTest top_ports deployment
// ======================================================================

#include "gtest/gtest.h"

#include "FppTest/topology/special_ports/SpecialPortsTopologyAc.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "SpecialPortsTopologyDefs.hpp"

#include "Os/Os.hpp"

namespace FppTest {
static TopologyState state;

class CompTester {
  public:
    enum {
        OPCODE_START = Comp::OPCODE_START,
        OPCODE_DATA = Comp::OPCODE_DATA,
        OPCODE_END = Comp::OPCODE_END,
        OPCODE_PARAM_SET = Comp::OPCODE_PARAM_SET,
        OPCODE_PARAM_SAVE = Comp::OPCODE_PARAM_SAVE,
    };
};

class FrameworkTester : public CompTester, public testing::Test {
  public:
    static void SetUpTestSuite() {
        Os::init();
        setup(state);
    }

    static void TearDownTestSuite() { teardown(state); }

// Macro to generate queue checking functions
#define CHECK_QUEUE(name, queue_field, type)                            \
    static void check_##name(const std::initializer_list<type> items) { \
        EXPECT_EQ(framework.queue_field.getSize(), items.size());       \
        FwIndexType i = 0;                                              \
        for (const auto& expected : items) {                            \
            type got;                                                   \
            auto status = framework.queue_field.dequeue(got);           \
            EXPECT_EQ(status, Fw::Success::SUCCESS) << "Index: " << i;  \
            EXPECT_EQ(expected, got) << "Index: " << i;                 \
            i += 1;                                                     \
        }                                                               \
    }

    CHECK_QUEUE(cmd_reg, cmd_reg_queue, FrameworkPortData::CmdReg)
    CHECK_QUEUE(cmd_res, cmd_response_queue, FrameworkPortData::CmdResponse)
    CHECK_QUEUE(log, log_queue, FrameworkPortData::Log)
    CHECK_QUEUE(log_text, log_text_queue, FrameworkPortData::LogText)
    CHECK_QUEUE(tlm, tlm_queue, FrameworkPortData::Tlm)
    CHECK_QUEUE(prm_get, prm_get_queue, FrameworkPortData::PrmGet)
    CHECK_QUEUE(prm_set, prm_set_queue, FrameworkPortData::PrmSet)
    CHECK_QUEUE(dp_get, dp_get_queue, FrameworkPortData::DpGet)
    CHECK_QUEUE(dp_request, dp_request_queue, FrameworkPortData::DpRequest)
    CHECK_QUEUE(dp_send, dp_send_queue, FrameworkPortData::DpSend)
    CHECK_QUEUE(ping, ping_queue, FrameworkPortData::Ping)

#undef CHECK_QUEUE

  protected:
    void SetUp() override {
        framework.cmd_reg_queue.clear();
        framework.cmd_response_queue.clear();
        framework.log_queue.clear();
        framework.log_text_queue.clear();
        framework.tlm_queue.clear();
        framework.prm_get_queue.clear();
        framework.prm_set_queue.clear();
        framework.dp_get_queue.clear();
        framework.dp_request_queue.clear();
        framework.dp_send_queue.clear();
        framework.ping_queue.clear();
    }

    void TearDown() override {
        // Validate that all the data queues are empty
        EXPECT_EQ(framework.cmd_reg_queue.getSize(), 0);
        EXPECT_EQ(framework.cmd_response_queue.getSize(), 0);
        EXPECT_EQ(framework.log_queue.getSize(), 0);
        EXPECT_EQ(framework.log_text_queue.getSize(), 0);
        EXPECT_EQ(framework.tlm_queue.getSize(), 0);
        EXPECT_EQ(framework.prm_get_queue.getSize(), 0);
        EXPECT_EQ(framework.prm_set_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_get_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_request_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_send_queue.getSize(), 0);
        EXPECT_EQ(framework.ping_queue.getSize(), 0);
    }
};

TEST_F(FrameworkTester, Log) {
    framework.setTime(Fw::Time(10, 10));

    const Fw::String arg1 = "three";
    comp1.emitEvent(1, 2.0, arg1);
    framework.sync();

    Fw::LogBuffer buf1;
    buf1.serializeFrom(static_cast<U32>(1));
    buf1.serializeFrom(static_cast<F32>(2.0));
    buf1.serializeFrom(arg1);

    framework.setTime(Fw::Time(20, 10));

    const Fw::String arg2 = "four";
    comp2.emitEvent(4, 8.5, arg2);
    framework.sync();

    Fw::LogBuffer buf2;
    buf2.serializeFrom(static_cast<U32>(4));
    buf2.serializeFrom(static_cast<F32>(8.5));
    buf2.serializeFrom(arg2);

    check_log({
        {0, comp1.getIdBase() + 0, Fw::Time(10, 10), Fw::LogSeverity::ACTIVITY_HI, buf1},
        {0, comp2.getIdBase() + 0, Fw::Time(20, 10), Fw::LogSeverity::ACTIVITY_HI, buf2},
    });

    check_log_text({
        {0, comp1.getIdBase() + 0, Fw::Time(10, 10), Fw::LogSeverity::ACTIVITY_HI,
         Fw::String("(comp1) Event : a: 1, b: 2.000000, c: three")},
        {0, comp2.getIdBase() + 0, Fw::Time(20, 10), Fw::LogSeverity::ACTIVITY_HI,
         Fw::String("(comp2) Event : a: 4, b: 8.500000, c: four")},
    });
}

TEST_F(FrameworkTester, Tlm) {
    framework.setTime(Fw::Time(10, 11));

    comp1.emitTelemetry(0x1);
    framework.sync();

    Fw::TlmBuffer buf1;
    buf1.serializeFrom(static_cast<U32>(0x1));

    framework.setTime(Fw::Time(20, 11));

    comp2.emitTelemetry(0x2);
    framework.sync();

    Fw::TlmBuffer buf2;
    buf2.serializeFrom(static_cast<U32>(0x2));

    check_tlm({
        {0, comp1.getIdBase() + 0, Fw::Time(10, 11), buf1},
        {0, comp2.getIdBase() + 0, Fw::Time(20, 11), buf2},
    });
}

TEST_F(FrameworkTester, CmdReg) {
    framework.setTime(Fw::Time(10, 12));
    regCommands();
    check_cmd_reg({
        {0, comp1.getIdBase() + OPCODE_START},
        {0, comp1.getIdBase() + OPCODE_DATA},
        {0, comp1.getIdBase() + OPCODE_END},
        {0, comp1.getIdBase() + OPCODE_PARAM_SET},
        {0, comp1.getIdBase() + OPCODE_PARAM_SAVE},

        {1, comp2.getIdBase() + OPCODE_START},
        {1, comp2.getIdBase() + OPCODE_DATA},
        {1, comp2.getIdBase() + OPCODE_END},
        {1, comp2.getIdBase() + OPCODE_PARAM_SET},
        {1, comp2.getIdBase() + OPCODE_PARAM_SAVE},
    });
}

TEST_F(FrameworkTester, CmdDp) {
    framework.setTime(Fw::Time(10, 13));

    Fw::CmdArgBuffer args;
    args.serializeFrom(static_cast<U32>(2));
    auto status = framework.sendCommand(comp1.getIdBase() + OPCODE_START, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    args.resetSer();
    args.serializeFrom(static_cast<U32>(0xA));
    args.serializeFrom(static_cast<F32>(12.5));
    args.serializeFrom(Fw::String("c"));
    status = framework.sendCommand(comp1.getIdBase() + OPCODE_DATA, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    args.resetSer();
    args.serializeFrom(static_cast<U32>(0xB));
    args.serializeFrom(static_cast<F32>(15.125));
    args.serializeFrom(Fw::String("cc"));
    status = framework.sendCommand(comp1.getIdBase() + OPCODE_DATA, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    args.resetSer();
    status = framework.sendCommand(comp1.getIdBase() + OPCODE_END, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    framework.sync();

    framework.setTime(Fw::Time(10, 14));

    args.resetSer();
    args.serializeFrom(static_cast<U32>(2));
    status = framework.sendCommand(comp2.getIdBase() + OPCODE_START, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    args.resetSer();
    args.serializeFrom(static_cast<U32>(2));
    status = framework.sendCommand(comp2.getIdBase() + OPCODE_START, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::BUSY);

    args.resetSer();
    status = framework.sendCommand(comp2.getIdBase() + OPCODE_END, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    args.resetSer();
    status = framework.sendCommand(comp2.getIdBase() + OPCODE_END, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::BUSY);

    framework.sync();

    check_cmd_res({
        {0, comp1.getIdBase() + OPCODE_START, 0, Fw::CmdResponse::OK},
        {0, comp1.getIdBase() + OPCODE_DATA, 0, Fw::CmdResponse::OK},
        {0, comp1.getIdBase() + OPCODE_DATA, 0, Fw::CmdResponse::OK},
        {0, comp1.getIdBase() + OPCODE_END, 0, Fw::CmdResponse::OK},

        {0, comp2.getIdBase() + OPCODE_START, 0, Fw::CmdResponse::OK},
        {0, comp2.getIdBase() + OPCODE_START, 0, Fw::CmdResponse::BUSY},
        {0, comp2.getIdBase() + OPCODE_END, 0, Fw::CmdResponse::OK},
        {0, comp2.getIdBase() + OPCODE_END, 0, Fw::CmdResponse::BUSY},
    });

    check_dp_request({
        {0, comp1.getIdBase(), 121},
        {1, comp2.getIdBase(), 121},
    });

    Fw::TlmBuffer buf1;
    // Serialize the packet type
    buf1.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_DP));
    // Serialize the container id
    buf1.serializeFrom(static_cast<FwDpIdType>(comp1.getIdBase()));
    buf1.serializeFrom(static_cast<FwDpPriorityType>(0));
    // Serialize the time tag
    buf1.serializeFrom(Fw::Time(10, 13));
    // Serialize the processing types
    buf1.serializeFrom(static_cast<Fw::DpCfg::ProcType::SerialType>(0));
    // Serialize the user data
    U8 userData[Fw::DpCfg::CONTAINER_USER_DATA_SIZE]{};
    buf1.serializeFrom(userData, static_cast<FwSizeType>(sizeof userData), Fw::Serialization::OMIT_LENGTH);
    // Serialize the data product state
    buf1.serializeFrom(Fw::DpState(Fw::DpState::UNTRANSMITTED));
    // Serialize the data size
    buf1.serializeSize(47);
    // Serialize the header CRC
    buf1.serializeFrom(static_cast<U32>(0x06B77648));

    // Serialize the first record
    buf1.serializeFrom(static_cast<FwDpIdType>(comp1.getIdBase() + 0));
    buf1.serializeFrom(FixedSizeData(0xA, 12.5, Fw::String("c")));

    // Serialize the second record
    buf1.serializeFrom(static_cast<FwDpIdType>(comp1.getIdBase() + 0));
    buf1.serializeFrom(FixedSizeData(0xB, 15.125, Fw::String("cc")));
    // Empty data hash
    buf1.serializeFrom(static_cast<U32>(0));

    // DP from the second component does not have any records
    // Just serialize the header
    Fw::TlmBuffer buf2;
    // Serialize the packet type
    buf2.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_DP));
    // Serialize the container id
    buf2.serializeFrom(static_cast<FwDpIdType>(comp2.getIdBase()));
    buf2.serializeFrom(static_cast<FwDpPriorityType>(0));
    // Serialize the time tag
    buf2.serializeFrom(Fw::Time(10, 14));
    // Serialize the processing types
    buf2.serializeFrom(static_cast<Fw::DpCfg::ProcType::SerialType>(0));
    // Serialize the user data
    buf2.serializeFrom(userData, static_cast<FwSizeType>(sizeof userData), Fw::Serialization::OMIT_LENGTH);
    // Serialize the data product state
    buf2.serializeFrom(Fw::DpState(Fw::DpState::UNTRANSMITTED));
    // Serialize the data size
    buf2.serializeSize(0);
    // Serialize the CRC
    buf2.serializeFrom(static_cast<U32>(0xC0082b9f));
    // Empty data hash
    buf2.serializeFrom(static_cast<U32>(0));

    check_dp_send({{0, comp1.getIdBase(), buf1}, {1, comp2.getIdBase(), buf2}});
}

TEST_F(FrameworkTester, Ping) {
    framework.ping(10);
    framework.ping(20);

    check_ping({
        {0, 10},
        {0, 10},
        {0, 20},
        {0, 20},
    });
}

TEST_F(FrameworkTester, Prm) {
    framework.setTime(Fw::Time(10, 13));

    Fw::CmdArgBuffer args;
    args.serializeFrom(static_cast<U32>(10));
    auto status = framework.sendCommand(comp1.getIdBase() + OPCODE_PARAM_SET, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    framework.setTime(Fw::Time(10, 14));

    args.resetSer();
    args.serializeFrom(static_cast<U32>(20));
    status = framework.sendCommand(comp2.getIdBase() + OPCODE_PARAM_SET, 0, args);
    EXPECT_EQ(status, Fw::CmdResponse::OK);

    check_cmd_res({
        {0, comp1.getIdBase() + OPCODE_PARAM_SET, 0, Fw::CmdResponse::OK},
        {0, comp2.getIdBase() + OPCODE_PARAM_SET, 0, Fw::CmdResponse::OK},
    });

    Fw::TlmBuffer tlm1;
    tlm1.serializeFrom(static_cast<FwPrmIdType>(0));
    tlm1.serializeFrom(static_cast<U32>(10));

    Fw::TlmBuffer tlm2;
    tlm2.serializeFrom(static_cast<FwPrmIdType>(0));
    tlm2.serializeFrom(static_cast<U32>(20));

    check_tlm({
        {0, comp1.getIdBase() + 1 /* param updated */, Fw::Time(10, 13), tlm1},
        {0, comp2.getIdBase() + 1 /* param updated */, Fw::Time(10, 14), tlm2},
    });
}

}  // namespace FppTest
