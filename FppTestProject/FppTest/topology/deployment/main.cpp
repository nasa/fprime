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

#define SYNC_PORT_CALL_NO_ARGS(sender, portName, receiver, portNum)      \
    {                                                                    \
        auto args = Types::Empty();                                      \
        U8 expected_data[1024];                                          \
        Fw::SerialBuffer expected(expected_data, sizeof(expected_data)); \
        args.serializeTo(expected, Fw::Endianness::BIG);                 \
        receiver.numMessages = 0;                                        \
        receiver.recv.resetSer();                                        \
        sender.portName##_out(portNum);                                  \
        ASSERT_EQ(expected, FppTest::receiver.recv);                     \
        ASSERT_EQ(FppTest::receiver.lastPortNum, portNum);               \
        ASSERT_EQ(FppTest::receiver.numMessages, 1);                     \
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
#define SYNC_PORT_CALL(sender, portName, receiver, argsType, portNum, ...) \
    {                                                                      \
        auto args = argsType();                                            \
        U8 expected_data[1024];                                            \
        Fw::SerialBuffer expected(expected_data, sizeof(expected_data));   \
        args.serializeTo(expected, Fw::Endianness::BIG);                   \
        receiver.numMessages = 0;                                          \
        receiver.recv.resetSer();                                          \
        sender.portName##_out(portNum, __VA_ARGS__);                       \
        ASSERT_EQ(expected, FppTest::receiver.recv);                       \
        ASSERT_EQ(FppTest::receiver.lastPortNum, portNum);                 \
        ASSERT_EQ(FppTest::receiver.numMessages, 1);                       \
    }

#define SYNC_PORT_CALL_NO_ARGS_RETURN(sender, portName, receiver, retVal, portNum, ...) \
    {                                                                                   \
        auto args = Types::Empty();                                                     \
        U8 expected_data[1024];                                                         \
        Fw::SerialBuffer expected(expected_data, sizeof(expected_data));                \
        args.serializeTo(expected, Fw::Endianness::BIG);                                \
        receiver.numMessages = 0;                                                       \
        receiver.recv.resetSer();                                                       \
        auto out = sender.portName##_out(portNum);                                      \
        ASSERT_EQ(retVal, out);                                                         \
        ASSERT_EQ(expected, FppTest::receiver.recv);                                    \
        ASSERT_EQ(FppTest::receiver.lastPortNum, portNum);                              \
        ASSERT_EQ(FppTest::receiver.numMessages, 1);                                    \
    }

#define SYNC_PORT_CALL_RETURN(sender, portName, receiver, argsType, retVal, portNum, ...) \
    {                                                                                     \
        auto args = argsType();                                                           \
        U8 expected_data[1024];                                                           \
        Fw::SerialBuffer expected(expected_data, sizeof(expected_data));                  \
        args.serializeTo(expected, Fw::Endianness::BIG);                                  \
        receiver.numMessages = 0;                                                         \
        receiver.recv.resetSer();                                                         \
        auto out = sender.portName##_out(portNum, __VA_ARGS__);                           \
        ASSERT_EQ(retVal, out);                                                           \
        ASSERT_EQ(expected, FppTest::receiver.recv);                                      \
        ASSERT_EQ(FppTest::receiver.lastPortNum, portNum);                                \
        ASSERT_EQ(FppTest::receiver.numMessages, 1);                                      \
    }

namespace FppTest {
static TopologyState state;

class SenderTester : public testing::Test {
  public:
    static void SetUpTestSuite() {
        Os::init();
        setup(state);
    }

    static void TearDownTestSuite() { teardown(state); }

    static void testNoArgs() {
        // Sync -> Receiver
        SYNC_PORT_CALL_NO_ARGS(sender1Sync, noArgsOut, receiver1, 0);
        SYNC_PORT_CALL_NO_ARGS(sender1Sync, noArgsOut, receiver1, 1);
        SYNC_PORT_CALL_NO_ARGS(sender2Sync, noArgsOut, receiver2, 0);
        SYNC_PORT_CALL_NO_ARGS(sender2Sync, noArgsOut, receiver2, 1);
        SYNC_PORT_CALL_NO_ARGS(sender1Guarded, noArgsOut, receiver1, 0);
        SYNC_PORT_CALL_NO_ARGS(sender1Guarded, noArgsOut, receiver1, 1);
        SYNC_PORT_CALL_NO_ARGS(sender2Guarded, noArgsOut, receiver2, 0);
        SYNC_PORT_CALL_NO_ARGS(sender2Guarded, noArgsOut, receiver2, 1);
    }

    static void testPrimitiveArgs() {
        // Sync -> Receiver
        SYNC_PORT_CALL(sender1Sync, primitiveArgsOut, receiver1, Types::PrimitiveTypes, 0, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender1Sync, primitiveArgsOut, receiver1, Types::PrimitiveTypes, 1, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Sync, primitiveArgsOut, receiver2, Types::PrimitiveTypes, 0, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Sync, primitiveArgsOut, receiver2, Types::PrimitiveTypes, 1, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        // Guarded -> Receiver
        SYNC_PORT_CALL(sender1Guarded, primitiveArgsOut, receiver1, Types::PrimitiveTypes, 0, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender1Guarded, primitiveArgsOut, receiver1, Types::PrimitiveTypes, 1, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Guarded, primitiveArgsOut, receiver2, Types::PrimitiveTypes, 0, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Guarded, primitiveArgsOut, receiver2, Types::PrimitiveTypes, 1, args.val1, args.val2,
                       args.val3, args.val4, args.val5, args.val6);
    }

    static void testStringArgs() {
        // Sync -> Receiver
        SYNC_PORT_CALL(sender1Sync, stringArgsOut, receiver1, Types::PortStringTypes, 0, args.val1, args.val2,
                       args.val3, args.val4);

        SYNC_PORT_CALL(sender1Sync, stringArgsOut, receiver1, Types::PortStringTypes, 1, args.val1, args.val2,
                       args.val3, args.val4);

        SYNC_PORT_CALL(sender2Sync, stringArgsOut, receiver2, Types::PortStringTypes, 0, args.val1, args.val2,
                       args.val3, args.val4);

        SYNC_PORT_CALL(sender2Sync, stringArgsOut, receiver2, Types::PortStringTypes, 1, args.val1, args.val2,
                       args.val3, args.val4);

        // Guarded -> Receiver
        SYNC_PORT_CALL(sender1Guarded, stringArgsOut, receiver1, Types::PortStringTypes, 0, args.val1, args.val2,
                       args.val3, args.val4);

        SYNC_PORT_CALL(sender1Guarded, stringArgsOut, receiver1, Types::PortStringTypes, 1, args.val1, args.val2,
                       args.val3, args.val4);

        SYNC_PORT_CALL(sender2Guarded, stringArgsOut, receiver2, Types::PortStringTypes, 0, args.val1, args.val2,
                       args.val3, args.val4);

        SYNC_PORT_CALL(sender2Guarded, stringArgsOut, receiver2, Types::PortStringTypes, 1, args.val1, args.val2,
                       args.val3, args.val4);
    }

    static void testEnumArgs() {
        // Sync -> Receiver
        SYNC_PORT_CALL(sender1Sync, enumArgsOut, receiver1, Types::EnumTypes, 0, args.val1, args.val2, args.val3,
                       args.val4);

        SYNC_PORT_CALL(sender1Sync, enumArgsOut, receiver1, Types::EnumTypes, 1, args.val1, args.val2, args.val3,
                       args.val4);

        SYNC_PORT_CALL(sender2Sync, enumArgsOut, receiver2, Types::EnumTypes, 0, args.val1, args.val2, args.val3,
                       args.val4);

        SYNC_PORT_CALL(sender2Sync, enumArgsOut, receiver2, Types::EnumTypes, 1, args.val1, args.val2, args.val3,
                       args.val4);

        // Guarded -> Receiver
        SYNC_PORT_CALL(sender1Guarded, enumArgsOut, receiver1, Types::EnumTypes, 0, args.val1, args.val2, args.val3,
                       args.val4);

        SYNC_PORT_CALL(sender1Guarded, enumArgsOut, receiver1, Types::EnumTypes, 1, args.val1, args.val2, args.val3,
                       args.val4);

        SYNC_PORT_CALL(sender2Guarded, enumArgsOut, receiver2, Types::EnumTypes, 0, args.val1, args.val2, args.val3,
                       args.val4);

        SYNC_PORT_CALL(sender2Guarded, enumArgsOut, receiver2, Types::EnumTypes, 1, args.val1, args.val2, args.val3,
                       args.val4);
    }

    static void testArrayArgs() {
        // Sync -> Receiver
        SYNC_PORT_CALL(sender1Sync, arrayArgsOut, receiver1, Types::ArrayTypes, 0, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender1Sync, arrayArgsOut, receiver1, Types::ArrayTypes, 1, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Sync, arrayArgsOut, receiver2, Types::ArrayTypes, 0, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Sync, arrayArgsOut, receiver2, Types::ArrayTypes, 1, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        // Guarded -> Receiver
        SYNC_PORT_CALL(sender1Guarded, arrayArgsOut, receiver1, Types::ArrayTypes, 0, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender1Guarded, arrayArgsOut, receiver1, Types::ArrayTypes, 1, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Guarded, arrayArgsOut, receiver2, Types::ArrayTypes, 0, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);

        SYNC_PORT_CALL(sender2Guarded, arrayArgsOut, receiver2, Types::ArrayTypes, 1, args.val1, args.val2, args.val3,
                       args.val4, args.val5, args.val6);
    }

    static void testStructArgs() {
        // Sync -> Receiver
        SYNC_PORT_CALL(sender1Sync, structArgsOut, receiver1, Types::StructTypes, 0, args.val1, args.val2);

        SYNC_PORT_CALL(sender1Sync, structArgsOut, receiver1, Types::StructTypes, 1, args.val1, args.val2);

        SYNC_PORT_CALL(sender2Sync, structArgsOut, receiver2, Types::StructTypes, 0, args.val1, args.val2);

        SYNC_PORT_CALL(sender2Sync, structArgsOut, receiver2, Types::StructTypes, 1, args.val1, args.val2);

        // Guarded -> Receiver
        SYNC_PORT_CALL(sender1Guarded, structArgsOut, receiver1, Types::StructTypes, 0, args.val1, args.val2);

        SYNC_PORT_CALL(sender1Guarded, structArgsOut, receiver1, Types::StructTypes, 1, args.val1, args.val2);

        SYNC_PORT_CALL(sender2Guarded, structArgsOut, receiver2, Types::StructTypes, 0, args.val1, args.val2);

        SYNC_PORT_CALL(sender2Guarded, structArgsOut, receiver2, Types::StructTypes, 1, args.val1, args.val2);
    }

    static void testNoArgsReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_NO_ARGS_RETURN(sender1Sync, noArgsReturnOut, receiver1, true, 0);

        SYNC_PORT_CALL_NO_ARGS_RETURN(sender1Sync, noArgsReturnOut, receiver1, true, 1);

        SYNC_PORT_CALL_NO_ARGS_RETURN(sender2Sync, noArgsReturnOut, receiver2, true, 0);

        SYNC_PORT_CALL_NO_ARGS_RETURN(sender2Sync, noArgsReturnOut, receiver2, true, 1);

        // Guarded -> Receiver
        SYNC_PORT_CALL_NO_ARGS_RETURN(sender1Guarded, noArgsReturnOut, receiver1, true, 0);

        SYNC_PORT_CALL_NO_ARGS_RETURN(sender1Guarded, noArgsReturnOut, receiver1, true, 1);

        SYNC_PORT_CALL_NO_ARGS_RETURN(sender2Guarded, noArgsReturnOut, receiver2, true, 0);

        SYNC_PORT_CALL_NO_ARGS_RETURN(sender2Guarded, noArgsReturnOut, receiver2, true, 1);
    }

    static void testPrimitiveReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, primitiveReturnOut, receiver1, Types::PrimitiveTypes, args.val1, 0,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL_RETURN(sender1Sync, primitiveReturnOut, receiver1, Types::PrimitiveTypes, args.val1, 1,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL_RETURN(sender2Sync, primitiveReturnOut, receiver2, Types::PrimitiveTypes, args.val1, 0,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL_RETURN(sender2Sync, primitiveReturnOut, receiver2, Types::PrimitiveTypes, args.val1, 1,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, primitiveReturnOut, receiver1, Types::PrimitiveTypes, args.val1, 0,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL_RETURN(sender1Guarded, primitiveReturnOut, receiver1, Types::PrimitiveTypes, args.val1, 1,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL_RETURN(sender2Guarded, primitiveReturnOut, receiver2, Types::PrimitiveTypes, args.val1, 0,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);

        SYNC_PORT_CALL_RETURN(sender2Guarded, primitiveReturnOut, receiver2, Types::PrimitiveTypes, args.val1, 1,
                              args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);
    }

    static void testStringReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, stringReturnOut, receiver1, Types::StringTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Sync, stringReturnOut, receiver1, Types::StringTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, stringReturnOut, receiver2, Types::StringTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, stringReturnOut, receiver2, Types::StringTypes, args.val1, 1, args.val1,
                              args.val2);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, stringReturnOut, receiver1, Types::StringTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Guarded, stringReturnOut, receiver1, Types::StringTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, stringReturnOut, receiver2, Types::StringTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, stringReturnOut, receiver2, Types::StringTypes, args.val1, 1, args.val1,
                              args.val2);
    }

    static void testStringAliasReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, stringAliasReturnOut, receiver1, Types::StringTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Sync, stringAliasReturnOut, receiver1, Types::StringTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, stringAliasReturnOut, receiver2, Types::StringTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, stringAliasReturnOut, receiver2, Types::StringTypes, args.val1, 1, args.val1,
                              args.val2);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, stringAliasReturnOut, receiver1, Types::StringTypes, args.val1, 0,
                              args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender1Guarded, stringAliasReturnOut, receiver1, Types::StringTypes, args.val1, 1,
                              args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, stringAliasReturnOut, receiver2, Types::StringTypes, args.val1, 0,
                              args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, stringAliasReturnOut, receiver2, Types::StringTypes, args.val1, 1,
                              args.val1, args.val2);
    }

    static void testEnumReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, enumReturnOut, receiver1, Types::EnumTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Sync, enumReturnOut, receiver1, Types::EnumTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, enumReturnOut, receiver2, Types::EnumTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, enumReturnOut, receiver2, Types::EnumTypes, args.val1, 1, args.val1,
                              args.val2);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, enumReturnOut, receiver1, Types::EnumTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Guarded, enumReturnOut, receiver1, Types::EnumTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, enumReturnOut, receiver2, Types::EnumTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, enumReturnOut, receiver2, Types::EnumTypes, args.val1, 1, args.val1,
                              args.val2);
    }

    static void testArrayReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, arrayReturnOut, receiver1, Types::ArrayTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Sync, arrayReturnOut, receiver1, Types::ArrayTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, arrayReturnOut, receiver2, Types::ArrayTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, arrayReturnOut, receiver2, Types::ArrayTypes, args.val1, 1, args.val1,
                              args.val2);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, arrayReturnOut, receiver1, Types::ArrayTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender1Guarded, arrayReturnOut, receiver1, Types::ArrayTypes, args.val1, 1, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, arrayReturnOut, receiver2, Types::ArrayTypes, args.val1, 0, args.val1,
                              args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, arrayReturnOut, receiver2, Types::ArrayTypes, args.val1, 1, args.val1,
                              args.val2);
    }

    static void testArrayStringAliasReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, arrayStringAliasReturnOut, receiver1, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 0, args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender1Sync, arrayStringAliasReturnOut, receiver1, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 1, args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, arrayStringAliasReturnOut, receiver2, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 0, args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender2Sync, arrayStringAliasReturnOut, receiver2, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 1, args.val1, args.val2);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, arrayStringAliasReturnOut, receiver1, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 0, args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender1Guarded, arrayStringAliasReturnOut, receiver1, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 1, args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, arrayStringAliasReturnOut, receiver2, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 0, args.val1, args.val2);

        SYNC_PORT_CALL_RETURN(sender2Guarded, arrayStringAliasReturnOut, receiver2, Types::ArrayTypesShort,
                              FormalAliasStringArray({"a", "b", "c"}), 1, args.val1, args.val2);
    }

    static void testStructReturn() {
        // Sync -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Sync, structReturnOut, receiver1, Types::StructType, args.val, 0, args.val,
                              args.val);

        SYNC_PORT_CALL_RETURN(sender1Sync, structReturnOut, receiver1, Types::StructType, args.val, 1, args.val,
                              args.val);

        SYNC_PORT_CALL_RETURN(sender2Sync, structReturnOut, receiver2, Types::StructType, args.val, 0, args.val,
                              args.val);

        SYNC_PORT_CALL_RETURN(sender2Sync, structReturnOut, receiver2, Types::StructType, args.val, 1, args.val,
                              args.val);

        // Guarded -> Receiver
        SYNC_PORT_CALL_RETURN(sender1Guarded, structReturnOut, receiver1, Types::StructType, args.val, 0, args.val,
                              args.val);

        SYNC_PORT_CALL_RETURN(sender1Guarded, structReturnOut, receiver1, Types::StructType, args.val, 1, args.val,
                              args.val);

        SYNC_PORT_CALL_RETURN(sender2Guarded, structReturnOut, receiver2, Types::StructType, args.val, 0, args.val,
                              args.val);

        SYNC_PORT_CALL_RETURN(sender2Guarded, structReturnOut, receiver2, Types::StructType, args.val, 1, args.val,
                              args.val);
    }
};

TEST_F(SenderTester, NoArgs) {
    testNoArgs();
}

TEST_F(SenderTester, PrimitiveArgs) {
    testPrimitiveArgs();
}

TEST_F(SenderTester, StringArgs) {
    testStringArgs();
}

TEST_F(SenderTester, EnumArgs) {
    testEnumArgs();
}

TEST_F(SenderTester, ArrayArgs) {
    testArrayArgs();
}

TEST_F(SenderTester, StructArgs) {
    testStructArgs();
}

TEST_F(SenderTester, NoArgsReturn) {
    testNoArgsReturn();
}

TEST_F(SenderTester, PrimitiveReturn) {
    testPrimitiveReturn();
}

TEST_F(SenderTester, StringReturn) {
    testStringReturn();
}

TEST_F(SenderTester, StringAliasReturn) {
    testStringAliasReturn();
}

TEST_F(SenderTester, ArrayStringAliasReturn) {
    testArrayStringAliasReturn();
}

TEST_F(SenderTester, StructReturn) {
    testStructReturn();
}

}  // namespace FppTest
