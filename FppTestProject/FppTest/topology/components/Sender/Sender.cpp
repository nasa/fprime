// ======================================================================
// \title  Sender.cpp
// \author tumbar
// \brief  cpp file for Sender component implementation class
// ======================================================================

#include "FppTest/topology/components/Sender/Sender.hpp"
#include "FppTest/component/types/FormalParamTypes.hpp"
#include "gtest/gtest.h"

namespace FppTest {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Sender ::Sender(const char* const compName)
    : SenderComponentBase(compName),
      m_expected(m_expectedData, sizeof(m_expectedData)),
      m_expectedData{},
      m_expectedPortNum{-1},
      m_expectedPortId(TestDeploymentPort::INVALID) {}

Sender ::~Sender() = default;

template <typename ArgTy>
ArgTy Sender::initTestCase(FwIndexType portNum, const TestDeploymentPort& portId) {
    auto args = ArgTy();
    m_expectedPortNum = portNum;
    m_expectedPortId = portId;
    m_expected.resetSer();
    args.serializeTo(m_expected, Fw::Endianness::BIG);
    return args;
}

void Sender::testNoArgs(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::Empty>(i, portId);
        noArgsOut_out(m_expectedPortNum);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testPrimitiveArgs(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::PrimitiveTypes>(i, portId);
        primitiveArgsOut_out(m_expectedPortNum, args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testStringArgs(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::PortStringTypes>(i, portId);
        stringArgsOut_out(m_expectedPortNum, args.val1, args.val2, args.val3, args.val4);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testEnumArgs(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::EnumTypes>(i, portId);
        enumArgsOut_out(m_expectedPortNum, args.val1, args.val2, args.val3, args.val4);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testArrayArgs(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::ArrayTypes>(i, portId);
        arrayArgsOut_out(m_expectedPortNum, args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testStructArgs(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::StructTypes>(i, portId);
        structArgsOut_out(m_expectedPortNum, args.val1, args.val2);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testNoArgsReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::Empty>(i, portId);
        auto out = noArgsReturnOut_out(m_expectedPortNum);
        EXPECT_EQ(out, true);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testPrimitiveReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::PrimitiveTypes>(i, portId);
        auto out =
            primitiveReturnOut_out(m_expectedPortNum, args.val1, args.val2, args.val3, args.val4, args.val5, args.val6);
        EXPECT_EQ(out, args.val1);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testStringReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::StringTypes>(i, portId);
        auto out = stringReturnOut_out(m_expectedPortNum, args.val1, args.val2);
        ASSERT_EQ(out, args.val1);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testStringAliasReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::StringTypes>(i, portId);
        auto out = stringAliasReturnOut_out(m_expectedPortNum, args.val1, args.val2);
        ASSERT_EQ(out, args.val1);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testEnumReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::EnumTypesShort>(i, portId);
        auto out = enumReturnOut_out(m_expectedPortNum, args.val1, args.val2);
        ASSERT_EQ(out, args.val1);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testArrayReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::ArrayTypesShort>(i, portId);
        auto out = arrayReturnOut_out(m_expectedPortNum, args.val1, args.val2);
        ASSERT_EQ(out, args.val1);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testArrayStringAliasReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::ArrayTypesShort>(i, portId);
        auto out = arrayStringAliasReturnOut_out(m_expectedPortNum, args.val1, args.val2);
        ASSERT_EQ(out, FormalAliasStringArray({"a", "b", "c"}));
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::testStructReturn(const TestDeploymentPort& portId) {
    for (FwIndexType i = 0; i < 2; i++) {
        auto args = initTestCase<Types::StructType>(i, portId);
        auto out = structReturnOut_out(m_expectedPortNum, args.val, args.val);
        ASSERT_EQ(out, args.val);
        wait();
    }

    m_expectedPortNum = -1;
}

void Sender::replyIn_handlerImpl(FwIndexType portNum,
                                 FwIndexType handlerPortNum,
                                 const FppTest::TestDeploymentPort& portId,
                                 const Fw::Buffer& inputData) {
    EXPECT_EQ(m_expectedPortNum, handlerPortNum);
    EXPECT_EQ(m_expectedPortId, portId);

    Fw::ExternalSerializeBuffer inputDataSer(inputData.getData(), inputData.getSize());
    inputDataSer.moveSerToOffset(inputData.getSize());
    EXPECT_EQ(inputDataSer, m_expected);
}

void Sender::replyIn_handler(FwIndexType portNum,
                             FwIndexType handlerPortNum,
                             const TestDeploymentPort& portId,
                             const Fw::Buffer& inputData) {
    replyIn_handlerImpl(portNum, handlerPortNum, portId, inputData);
    done_internalInterfaceInvoke();
}
void Sender::done_internalInterfaceHandler() {
    // This message is just sent to unblock to internal queue to signal the test
    // to continue. This handler should never be called via `doDispatch`
    FW_ASSERT(0);
}

void Sender::wait() {
    U8 buf[32];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    m_queue.receive(buf, sizeof(buf), Os::QueueInterface::BLOCKING, actualSize, priority);
}

}  // namespace FppTest
