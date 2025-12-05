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

namespace FppTest {
static TopologyState state;

class SenderTester : public testing::Test {
  public:
    static void SetUpTestSuite() {
        Os::init();
        setup(state);
    }

    static void TearDownTestSuite() { teardown(state); }
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

TEST_F(SenderTester, StructReturn) {
    sender1Sync.testStructReturn(TestDeploymentPort::STRUCT_RETURN_SYNC);
    sender2Sync.testStructReturn(TestDeploymentPort::STRUCT_RETURN_SYNC);
    sender1Guarded.testStructReturn(TestDeploymentPort::STRUCT_RETURN_GUARDED);
    sender2Guarded.testStructReturn(TestDeploymentPort::STRUCT_RETURN_GUARDED);
}

}  // namespace FppTest
