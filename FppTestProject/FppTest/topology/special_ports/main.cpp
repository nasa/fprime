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

    void check_log(std::initializer_list<FppTest::FrameworkPortData::Log> logs) {
        EXPECT_EQ(framework.log_queue.getSize(), logs.size());

        for (const auto& expected : logs) {
            FppTest::FrameworkPortData::Log got;
            auto status = framework.log_queue.dequeue(got);
            EXPECT_EQ(status, Fw::Success::SUCCESS);
            EXPECT_EQ(expected, got);
        }
    }

    void check_log_text(std::initializer_list<FppTest::FrameworkPortData::LogText> logs) {
        EXPECT_EQ(framework.log_text_queue.getSize(), logs.size());

        for (const auto& expected : logs) {
            FppTest::FrameworkPortData::LogText got;
            auto status = framework.log_text_queue.dequeue(got);
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
        EXPECT_EQ(framework.time_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_get_queue.getSize(), 0);
        EXPECT_EQ(framework.dp_request_queue.getSize(), 0);
        EXPECT_EQ(framework.ping_queue.getSize(), 0);
    }
};

TEST_F(FrameworkTester, LogRecv) {
    Fw::String arg = "three";
    comp.emitEvent(1, 2.0, arg);
    comp.finish();
    framework.wait();

    check_log({
        FppTest::FrameworkPortData::Log (
            0,
            Fw::Time()
        )
    })

    FppTest::FrameworkPortData::Log l;
    auto status = framework.log_queue.dequeue(l);
    EXPECT_EQ(status, Fw::Success::SUCCESS);
}
}  // namespace FppTest
