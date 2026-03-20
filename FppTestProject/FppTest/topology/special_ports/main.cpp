// ======================================================================
// \title  main.cpp
// \author tumbar
// \brief  Main program for FppTest top_ports deployment
// ======================================================================

#include "gtest/gtest.h"

#include "FppTest/topology/special_ports/SpecialPortsTopologyAc.hpp"
#include "SpecialPortsTopologyDefs.hpp"

#include "Os/Os.hpp"

namespace FppTest {
static TopologyState state;

class FrameworkTester : public testing::Test {
  public:
    static void SetUpTestSuite() {
        Os::init();
        setup(state);
    }

    static void TearDownTestSuite() { teardown(state); }

    void SetUp() override { framework.clear(); }

    static void check_log(const std::initializer_list<FrameworkPortData::Log> logs) {
        EXPECT_EQ(framework.log_queue.getSize(), logs.size());

        for (const auto& expected : logs) {
            FrameworkPortData::Log got;
            auto status = framework.log_queue.dequeue(got);
            EXPECT_EQ(status, Fw::Success::SUCCESS);
            EXPECT_EQ(expected, got);
        }
    }

    static void check_log_text(const std::initializer_list<FrameworkPortData::LogText> logs) {
        EXPECT_EQ(framework.log_text_queue.getSize(), logs.size());

        for (const auto& expected : logs) {
            FrameworkPortData::LogText got;
            auto status = framework.log_text_queue.dequeue(got);
            EXPECT_EQ(status, Fw::Success::SUCCESS);
            EXPECT_EQ(expected, got);
        }
    }

    static void check_tlm(const std::initializer_list<FrameworkPortData::Tlm> tlms) {
        EXPECT_EQ(framework.tlm_queue.getSize(), tlms.size());

        for (const auto& expected : tlms) {
            FrameworkPortData::Tlm got;
            auto status = framework.tlm_queue.dequeue(got);
            EXPECT_EQ(status, Fw::Success::SUCCESS);
            EXPECT_EQ(expected, got);
        }
    }

    void TearDown() override {
        // Validate that all the data queues are empty
        EXPECT_EQ(framework.cmd_reg_queue.getSize(), 0);
        EXPECT_EQ(framework.cmd_response.getSize(), 0);
        EXPECT_EQ(framework.log_queue.getSize(), 0);
        EXPECT_EQ(framework.log_text_queue.getSize(), 0);
        EXPECT_EQ(framework.tlm_queue.getSize(), 0);
        EXPECT_EQ(framework.prm_get_queue.getSize(), 0);
        EXPECT_EQ(framework.prm_set_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_get_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_request_queue.getSize(), 0);
        EXPECT_EQ(framework.ping_queue.getSize(), 0);
    }
};

TEST_F(FrameworkTester, LogRecv) {
    framework.setTime(Fw::Time(10, 10));

    const Fw::String arg = "three";
    comp.emitEvent(1, 2.0, arg);
    comp.finish();
    framework.wait();

    Fw::LogBuffer buf;
    buf.serializeFrom(static_cast<U32>(1));
    buf.serializeFrom(static_cast<F32>(2.0));
    buf.serializeFrom(arg);

    check_log({
        {comp.getIdBase() + 0, Fw::Time(10, 10), Fw::LogSeverity::ACTIVITY_HI, buf},
    });

    check_log_text({
        {comp.getIdBase() + 0, Fw::Time(10, 10), Fw::LogSeverity::ACTIVITY_HI,
         Fw::String("(comp) Event : a: 1, b: 2.000000, c: three")},
    });
}

TEST_F(FrameworkTester, TlmRecv) {
    framework.setTime(Fw::Time(10, 11));

    comp.emitTelemetry(0x1);
    comp.finish();
    framework.wait();

    Fw::TlmBuffer buf;
    buf.serializeFrom(static_cast<U32>(0x1));

    check_tlm({{comp.getIdBase() + 0, Fw::Time(10, 11), buf}});
}

}  // namespace FppTest
