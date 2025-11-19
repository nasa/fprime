// ======================================================================
// \title  main.cpp
// \author bocchino
// \brief  Main program for FppTest deployment
// ======================================================================

#include "FppTest/component/types/FormalParamTypes.hpp"
#include "gtest/gtest.h"

#include "FppTest/topology/deployment/topology/FppTestTopologyAc.hpp"
#include "Os/Os.hpp"
#include "topology/FppTestTopologyDefs.hpp"

#define SYNC_PORT_CALL_NO_ARGS(sender_with_port, receiver, argsType, portNum) \
    {                                                                         \
        auto args = argsType();                                               \
        U8 expected_data[1024];                                               \
        Fw::SerialBuffer expected(expected_data, sizeof(expected_data));      \
        args.serializeTo(expected, Fw::Endianness::BIG);                      \
        receiver.numMessages = 0;                                             \
        receiver.recv.resetSer();                                             \
        sender_with_port(portNum);                                            \
        ASSERT_TRUE(expected == FppTest::receiver.recv);                      \
        ASSERT_EQ(FppTest::receiver.lastPortNum, portNum);                    \
        ASSERT_EQ(FppTest::receiver.numMessages, 1);                          \
    }

/**
 * Macro for generating sync/guarded port calls from a sender to a receiver and checking for data integrity.
 *
 * Note: This does not work with 0 argument ports. Use `SYNC_PORT_CALL_NO_ARGS` instead.
 *
 * @param sender_with_port i.e. sender1Sync.arrayArgsOut_out
 * @param receiver receiver1
 * @param argsType A type in FormalParamTypes.hpp
 * @param portNum port number
 */
#define SYNC_PORT_CALL(sender_with_port, receiver, argsType, portNum, ...) \
    {                                                                      \
        auto args = argsType();                                            \
        U8 expected_data[1024];                                            \
        Fw::SerialBuffer expected(expected_data, sizeof(expected_data));   \
        args.serializeTo(expected, Fw::Endianness::BIG);                   \
        receiver.numMessages = 0;                                          \
        receiver.recv.resetSer();                                          \
        sender_with_port(portNum, __VA_ARGS__);                            \
        ASSERT_TRUE(expected == FppTest::receiver.recv);                   \
        ASSERT_EQ(FppTest::receiver.lastPortNum, portNum);                 \
        ASSERT_EQ(FppTest::receiver.numMessages, 1);                       \
    }

namespace FppTest {
class SenderTester : public testing::Test {
  public:
    TopologyState state;

    void SetUp() override {
        Os::init();
        setup(state);
    }

    void TearDown() override { teardown(state); }

    static void test_no_args() {
        // Sync -> Receiver
        SYNC_PORT_CALL_NO_ARGS(sender1Sync.noArgsOut_out, receiver1, Types::Empty, 0);
        SYNC_PORT_CALL_NO_ARGS(sender1Sync.noArgsOut_out, receiver1, Types::Empty, 1);
        SYNC_PORT_CALL_NO_ARGS(sender2Sync.noArgsOut_out, receiver2, Types::Empty, 0);
        SYNC_PORT_CALL_NO_ARGS(sender2Sync.noArgsOut_out, receiver2, Types::Empty, 1);
        SYNC_PORT_CALL_NO_ARGS(sender1Guarded.noArgsOut_out, receiver1, Types::Empty, 0);
        SYNC_PORT_CALL_NO_ARGS(sender1Guarded.noArgsOut_out, receiver1, Types::Empty, 1);
        SYNC_PORT_CALL_NO_ARGS(sender2Guarded.noArgsOut_out, receiver2, Types::Empty, 0);
        SYNC_PORT_CALL_NO_ARGS(sender2Guarded.noArgsOut_out, receiver2, Types::Empty, 1);
    }

    static void test_array_args() {
        // Sync -> Receiver
        SYNC_PORT_CALL(sender1Sync.arrayArgsOut_out, receiver1, Types::ArrayTypes, 0, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender1Sync.arrayArgsOut_out, receiver1, Types::ArrayTypes, 1, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Sync.arrayArgsOut_out, receiver2, Types::ArrayTypes, 0, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Sync.arrayArgsOut_out, receiver2, Types::ArrayTypes, 1, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        // Guarded -> Receiver
        SYNC_PORT_CALL(sender1Guarded.arrayArgsOut_out, receiver1, Types::ArrayTypes, 0, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender1Guarded.arrayArgsOut_out, receiver1, Types::ArrayTypes, 1, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Guarded.arrayArgsOut_out, receiver2, Types::ArrayTypes, 0, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Guarded.arrayArgsOut_out, receiver2, Types::ArrayTypes, 1, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);
    }
};

TEST_F(SenderTester, NoArgs) {
    test_no_args();
}

TEST_F(SenderTester, ArrayArgs) {
    test_array_args();
}
}  // namespace FppTest
