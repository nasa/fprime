// ======================================================================
// \title  main.cpp
// \author bocchino, tumbar
// \brief  Main program for FppTest deployment
// ======================================================================

#include "FppTest/component/types/FormalParamTypes.hpp"
#include "gtest/gtest.h"

#include "FppTest/topology/main/FppTestTopologyAc.hpp"
#include "FppTest/topology/ports/SenderIdEnumAc.hpp"
#include "FppTestTopologyDefs.hpp"
#include "Os/Os.hpp"

namespace FppTest {
static TopologyState state;

class SenderTester : public testing::Test {
  public:
    static void SetUpTestSuite() {
        Os::init();
        setup(state);
    }

    static void TearDownTestSuite() { teardown(state); }

    static void testIsConnected() {
#define CHECK_IS_CONNECTED_TYPE(kind, portName)                        \
    ASSERT_TRUE(sender1##kind.isConnected_##portName##_OutputPort(0)); \
    ASSERT_TRUE(sender1##kind.isConnected_##portName##_OutputPort(1)); \
    ASSERT_TRUE(sender2##kind.isConnected_##portName##_OutputPort(0)); \
    ASSERT_TRUE(sender2##kind.isConnected_##portName##_OutputPort(1));

#define CHECK_IS_CONNECTED_SG(portName)     \
    CHECK_IS_CONNECTED_TYPE(Sync, portName) \
    CHECK_IS_CONNECTED_TYPE(Guarded, portName)

#define CHECK_IS_CONNECTED_SGA(portName)       \
    CHECK_IS_CONNECTED_TYPE(Sync, portName)    \
    CHECK_IS_CONNECTED_TYPE(Guarded, portName) \
    CHECK_IS_CONNECTED_TYPE(Async, portName)

        CHECK_IS_CONNECTED_SGA(noArgsOut)
        CHECK_IS_CONNECTED_SGA(primitiveArgsOut)
        CHECK_IS_CONNECTED_SGA(stringArgsOut)
        CHECK_IS_CONNECTED_SGA(enumArgsOut)
        CHECK_IS_CONNECTED_SGA(arrayArgsOut)
        CHECK_IS_CONNECTED_SGA(structArgsOut)

        CHECK_IS_CONNECTED_SG(noArgsReturnOut)
        CHECK_IS_CONNECTED_SG(primitiveReturnOut)
        CHECK_IS_CONNECTED_SG(stringReturnOut)
        CHECK_IS_CONNECTED_SG(stringAliasReturnOut)
        CHECK_IS_CONNECTED_SG(enumReturnOut)
        CHECK_IS_CONNECTED_SG(arrayReturnOut)
        CHECK_IS_CONNECTED_SG(arrayStringAliasReturnOut)
        CHECK_IS_CONNECTED_SG(structReturnOut)
    }
};

class ReceiverTester {
  public:
    virtual ~ReceiverTester() = default;

    static void testIsConnected() {
        ASSERT_TRUE(receiver1.isConnected_replyOut_OutputPort(SenderId::SYNC));
        ASSERT_TRUE(receiver2.isConnected_replyOut_OutputPort(SenderId::SYNC));
        ASSERT_TRUE(receiver1.isConnected_replyOut_OutputPort(SenderId::GUARDED));
        ASSERT_TRUE(receiver2.isConnected_replyOut_OutputPort(SenderId::GUARDED));
        ASSERT_TRUE(receiver1.isConnected_replyOut_OutputPort(SenderId::ASYNC));
        ASSERT_TRUE(receiver2.isConnected_replyOut_OutputPort(SenderId::ASYNC));
    }
};

TEST_F(SenderTester, NoArgs) {
    sender1Sync.testNoArgs(TestDeploymentPort::NO_ARGS_SYNC);
    sender2Sync.testNoArgs(TestDeploymentPort::NO_ARGS_SYNC);
    sender1Guarded.testNoArgs(TestDeploymentPort::NO_ARGS_GUARDED);
    sender2Guarded.testNoArgs(TestDeploymentPort::NO_ARGS_GUARDED);
    sender1Async.testNoArgs(TestDeploymentPort::NO_ARGS_ASYNC);
    sender2Async.testNoArgs(TestDeploymentPort::NO_ARGS_ASYNC);
}

TEST_F(SenderTester, PrimitiveArgs) {
    sender1Sync.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
    sender2Sync.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
    sender1Guarded.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_GUARDED);
    sender2Guarded.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_GUARDED);
    sender1Async.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_ASYNC);
    sender2Async.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_ASYNC);
}

TEST_F(SenderTester, StringArgs) {
    sender1Sync.testStringArgs(TestDeploymentPort::STRING_ARGS_SYNC);
    sender2Sync.testStringArgs(TestDeploymentPort::STRING_ARGS_SYNC);
    sender1Guarded.testStringArgs(TestDeploymentPort::STRING_ARGS_GUARDED);
    sender2Guarded.testStringArgs(TestDeploymentPort::STRING_ARGS_GUARDED);
    sender1Async.testStringArgs(TestDeploymentPort::STRING_ARGS_ASYNC);
    sender2Async.testStringArgs(TestDeploymentPort::STRING_ARGS_ASYNC);
}

TEST_F(SenderTester, EnumArgs) {
    sender1Sync.testEnumArgs(TestDeploymentPort::ENUM_ARGS_SYNC);
    sender2Sync.testEnumArgs(TestDeploymentPort::ENUM_ARGS_SYNC);
    sender1Guarded.testEnumArgs(TestDeploymentPort::ENUM_ARGS_GUARDED);
    sender2Guarded.testEnumArgs(TestDeploymentPort::ENUM_ARGS_GUARDED);
    sender1Async.testEnumArgs(TestDeploymentPort::ENUM_ARGS_ASYNC);
    sender2Async.testEnumArgs(TestDeploymentPort::ENUM_ARGS_ASYNC);
}

TEST_F(SenderTester, ArrayArgs) {
    sender1Sync.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_SYNC);
    sender2Sync.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_SYNC);
    sender1Guarded.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_GUARDED);
    sender2Guarded.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_GUARDED);
    sender1Async.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_ASYNC);
    sender2Async.testArrayArgs(TestDeploymentPort::ARRAY_ARGS_ASYNC);
}

TEST_F(SenderTester, StructArgs) {
    sender1Sync.testStructArgs(TestDeploymentPort::STRUCT_ARGS_SYNC);
    sender2Sync.testStructArgs(TestDeploymentPort::STRUCT_ARGS_SYNC);
    sender1Guarded.testStructArgs(TestDeploymentPort::STRUCT_ARGS_GUARDED);
    sender2Guarded.testStructArgs(TestDeploymentPort::STRUCT_ARGS_GUARDED);
    sender1Async.testStructArgs(TestDeploymentPort::STRUCT_ARGS_ASYNC);
    sender2Async.testStructArgs(TestDeploymentPort::STRUCT_ARGS_ASYNC);
}

TEST_F(SenderTester, NoArgsReturn) {
    sender1Sync.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_SYNC);
    sender2Sync.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_SYNC);
    sender1Guarded.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_GUARDED);
    sender2Guarded.testNoArgsReturn(TestDeploymentPort::NO_ARGS_RETURN_GUARDED);
}

TEST_F(SenderTester, PrimitiveReturn) {
    sender1Sync.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_SYNC);
    sender2Sync.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_SYNC);
    sender1Guarded.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_GUARDED);
    sender2Guarded.testPrimitiveReturn(TestDeploymentPort::PRIMITIVE_RETURN_GUARDED);
}

TEST_F(SenderTester, EnumReturn) {
    sender1Sync.testEnumReturn(TestDeploymentPort::ENUM_RETURN_SYNC);
    sender2Sync.testEnumReturn(TestDeploymentPort::ENUM_RETURN_SYNC);
    sender1Guarded.testEnumReturn(TestDeploymentPort::ENUM_RETURN_GUARDED);
    sender2Guarded.testEnumReturn(TestDeploymentPort::ENUM_RETURN_GUARDED);
}

TEST_F(SenderTester, StringReturn) {
    sender1Sync.testStringReturn(TestDeploymentPort::STRING_RETURN_SYNC);
    sender2Sync.testStringReturn(TestDeploymentPort::STRING_RETURN_SYNC);
    sender1Guarded.testStringReturn(TestDeploymentPort::STRING_RETURN_GUARDED);
    sender2Guarded.testStringReturn(TestDeploymentPort::STRING_RETURN_GUARDED);
}

TEST_F(SenderTester, StringAliasReturn) {
    sender1Sync.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_SYNC);
    sender2Sync.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_SYNC);
    sender1Guarded.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_GUARDED);
    sender2Guarded.testStringAliasReturn(TestDeploymentPort::STRING_ALIAS_RETURN_GUARDED);
}

TEST_F(SenderTester, ArrayStringAliasReturn) {
    sender1Sync.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_SYNC);
    sender2Sync.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_SYNC);
    sender1Guarded.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_GUARDED);
    sender2Guarded.testArrayStringAliasReturn(TestDeploymentPort::ARRAY_STRING_ALIAS_RETURN_GUARDED);
}

TEST_F(SenderTester, ArrayReturn) {
    sender1Sync.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_SYNC);
    sender2Sync.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_SYNC);
    sender1Guarded.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_GUARDED);
    sender2Guarded.testArrayReturn(TestDeploymentPort::ARRAY_RETURN_GUARDED);
}

TEST_F(SenderTester, StructReturn) {
    sender1Sync.testStructReturn(TestDeploymentPort::STRUCT_RETURN_SYNC);
    sender2Sync.testStructReturn(TestDeploymentPort::STRUCT_RETURN_SYNC);
    sender1Guarded.testStructReturn(TestDeploymentPort::STRUCT_RETURN_GUARDED);
    sender2Guarded.testStructReturn(TestDeploymentPort::STRUCT_RETURN_GUARDED);
}

TEST_F(SenderTester, SenderTop) {
    sender1Top.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
    sender2Top.testPrimitiveArgs(TestDeploymentPort::PRIMITIVE_ARGS_SYNC);
}

TEST_F(SenderTester, IsReceiverConnected) {
    ReceiverTester::testIsConnected();
}

TEST_F(SenderTester, IsSenderConnected) {
    testIsConnected();
}

}  // namespace FppTest
