// ======================================================================
// \title  PortTests.hpp
// \author T. Chieu
// \brief  hpp file for port tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/component/active/SerialPortIndexEnumAc.hpp"
#include "FppTest/component/active/TypedPortIndexEnumAc.hpp"
#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Port test declarations
// ----------------------------------------------------------------------

#define PORT_TEST_INVOKE_DECL(PORT_KIND, TYPE) \
    void test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& port);

#define PORT_TEST_INVOKE_DECLS(PORT_KIND)              \
    PORT_TEST_INVOKE_DECL(PORT_KIND, NoParams)         \
    PORT_TEST_INVOKE_DECL(PORT_KIND, PrimitiveParams)  \
    PORT_TEST_INVOKE_DECL(PORT_KIND, PortStringParams) \
    PORT_TEST_INVOKE_DECL(PORT_KIND, EnumParams)       \
    PORT_TEST_INVOKE_DECL(PORT_KIND, ArrayParams)      \
    PORT_TEST_INVOKE_DECL(PORT_KIND, StructParams)     \
    PORT_TEST_INVOKE_DECL(PORT_KIND, NoParamReturn)    \
    PORT_TEST_INVOKE_DECL(PORT_KIND, PrimitiveReturn)  \
    PORT_TEST_INVOKE_DECL(PORT_KIND, EnumReturn)       \
    PORT_TEST_INVOKE_DECL(PORT_KIND, ArrayReturn)      \
    PORT_TEST_INVOKE_DECL(PORT_KIND, StructReturn)

#define PORT_TEST_INVOKE_SERIAL_HELPER_DECL(PORT_KIND) \
    void invoke##PORT_KIND##SerialPort(NATIVE_INT_TYPE portNum, Fw::SerialBuffer& buf);

#define PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, TYPE) \
    void test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& port);

#define PORT_TEST_INVOKE_SERIAL_DECLS(PORT_KIND)              \
    PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, NoParams)         \
    PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, PrimitiveParams)  \
    PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, PortStringParams) \
    PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, EnumParams)       \
    PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, ArrayParams)      \
    PORT_TEST_INVOKE_SERIAL_DECL(PORT_KIND, StructParams)

#define PORT_TEST_CHECK_DECL(PORT_KIND, TYPE) void test##PORT_KIND##PortCheck(FppTest::Types::TYPE& port);

#define PORT_TEST_CHECK_DECLS(PORT_KIND)              \
    PORT_TEST_CHECK_DECL(PORT_KIND, NoParams)         \
    PORT_TEST_CHECK_DECL(PORT_KIND, PrimitiveParams)  \
    PORT_TEST_CHECK_DECL(PORT_KIND, PortStringParams) \
    PORT_TEST_CHECK_DECL(PORT_KIND, EnumParams)       \
    PORT_TEST_CHECK_DECL(PORT_KIND, ArrayParams)      \
    PORT_TEST_CHECK_DECL(PORT_KIND, StructParams)     \
    PORT_TEST_CHECK_DECL(PORT_KIND, NoParamReturn)    \
    PORT_TEST_CHECK_DECL(PORT_KIND, PrimitiveReturn)  \
    PORT_TEST_CHECK_DECL(PORT_KIND, EnumReturn)       \
    PORT_TEST_CHECK_DECL(PORT_KIND, ArrayReturn)      \
    PORT_TEST_CHECK_DECL(PORT_KIND, StructReturn)

#define PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, TYPE) void test##PORT_KIND##PortCheckSerial(FppTest::Types::TYPE& port);

#define PORT_TEST_CHECK_SERIAL_DECLS(PORT_KIND)              \
    PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, NoParams)         \
    PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, PrimitiveParams)  \
    PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, PortStringParams) \
    PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, EnumParams)       \
    PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, ArrayParams)      \
    PORT_TEST_CHECK_SERIAL_DECL(PORT_KIND, StructParams)

#define PORT_TEST_DECLS_KIND(PORT_KIND)            \
    PORT_TEST_INVOKE_DECLS(PORT_KIND)              \
    PORT_TEST_INVOKE_SERIAL_HELPER_DECL(PORT_KIND) \
    PORT_TEST_INVOKE_SERIAL_DECLS(PORT_KIND)       \
    PORT_TEST_CHECK_DECLS(PORT_KIND)               \
    PORT_TEST_CHECK_SERIAL_DECLS(PORT_KIND)

#define PORT_TEST_DECLS        \
    PORT_TEST_DECLS_KIND(Sync) \
    PORT_TEST_DECLS_KIND(Guarded)

#define PORT_TEST_DECLS_ASYNC PORT_TEST_DECLS_KIND(Async)

// ----------------------------------------------------------------------
// Invoke typed input ports
// ----------------------------------------------------------------------

#define PORT_TEST_INVOKE_DEFS(PORT_KIND)                                                                         \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::NoParams& port) {         \
        ASSERT_TRUE(component.isConnected_noArgsOut_OutputPort(portNum));                                        \
        ASSERT_TRUE(this->isConnected_to_noArgs##PORT_KIND(portNum));                                            \
                                                                                                                 \
        this->invoke_to_noArgs##PORT_KIND(portNum);                                                              \
    }                                                                                                            \
                                                                                                                 \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveParams& port) {  \
        ASSERT_TRUE(component.isConnected_primitiveArgsOut_OutputPort(portNum));                                 \
        ASSERT_TRUE(this->isConnected_to_primitiveArgs##PORT_KIND(portNum));                                     \
                                                                                                                 \
        this->invoke_to_primitiveArgs##PORT_KIND(portNum, port.args.val1, port.args.val2, port.args.val3,        \
                                                 port.args.val4, port.args.val5, port.args.val6);                \
    }                                                                                                            \
                                                                                                                 \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::PortStringParams& port) { \
        ASSERT_TRUE(component.isConnected_stringArgsOut_OutputPort(portNum));                                    \
        ASSERT_TRUE(this->isConnected_to_stringArgs##PORT_KIND(portNum));                                        \
                                                                                                                 \
        this->invoke_to_stringArgs##PORT_KIND(portNum, port.args.val1, port.args.val2, port.args.val3,           \
                                              port.args.val4);                                                   \
    }                                                                                                            \
                                                                                                                 \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::EnumParams& port) {       \
        ASSERT_TRUE(component.isConnected_enumArgsOut_OutputPort(portNum));                                      \
        ASSERT_TRUE(this->isConnected_to_enumArgs##PORT_KIND(portNum));                                          \
                                                                                                                 \
        this->invoke_to_enumArgs##PORT_KIND(portNum, port.args.val1, port.args.val2);                            \
    }                                                                                                            \
                                                                                                                 \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParams& port) {      \
        ASSERT_TRUE(component.isConnected_arrayArgsOut_OutputPort(portNum));                                     \
        ASSERT_TRUE(this->isConnected_to_arrayArgs##PORT_KIND(portNum));                                         \
                                                                                                                 \
        this->invoke_to_arrayArgs##PORT_KIND(portNum, port.args.val1, port.args.val2);                           \
    }                                                                                                            \
                                                                                                                 \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::StructParams& port) {     \
        ASSERT_TRUE(component.isConnected_structArgsOut_OutputPort(portNum));                                    \
        ASSERT_TRUE(this->isConnected_to_structArgs##PORT_KIND(portNum));                                        \
                                                                                                                 \
        this->invoke_to_structArgs##PORT_KIND(portNum, port.args.val1, port.args.val2);                          \
    }

#define PORT_TEST_INVOKE_RETURN_DEFS(PORT_KIND)                                                                       \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::NoParamReturn& port) {         \
        ASSERT_TRUE(component.isConnected_noArgsReturnOut_OutputPort(portNum));                                       \
        ASSERT_TRUE(this->isConnected_to_noArgsReturn##PORT_KIND(portNum));                                           \
                                                                                                                      \
        bool returnVal = this->invoke_to_noArgsReturn##PORT_KIND(portNum);                                            \
                                                                                                                      \
        ASSERT_EQ(returnVal, this->noParamReturnVal.val);                                                             \
    }                                                                                                                 \
                                                                                                                      \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveReturn& port) {       \
        ASSERT_TRUE(component.isConnected_primitiveReturnOut_OutputPort(portNum));                                    \
        ASSERT_TRUE(this->isConnected_to_primitiveReturn##PORT_KIND(portNum));                                        \
                                                                                                                      \
        U32 returnVal = this->invoke_to_primitiveReturn##PORT_KIND(                                                   \
            portNum, port.args.val1, port.args.val2, port.args.val3, port.args.val4, port.args.val5, port.args.val6); \
                                                                                                                      \
        ASSERT_EQ(returnVal, this->primitiveReturnVal.val);                                                           \
    }                                                                                                                 \
                                                                                                                      \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::EnumReturn& port) {            \
        ASSERT_TRUE(component.isConnected_enumReturnOut_OutputPort(portNum));                                         \
        ASSERT_TRUE(this->isConnected_to_enumReturn##PORT_KIND(portNum));                                             \
                                                                                                                      \
        FormalParamEnum returnVal = this->invoke_to_enumReturn##PORT_KIND(portNum, port.args.val1, port.args.val2);   \
                                                                                                                      \
        ASSERT_EQ(returnVal, this->enumReturnVal.val);                                                                \
    }                                                                                                                 \
                                                                                                                      \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayReturn& port) {           \
        ASSERT_TRUE(component.isConnected_arrayReturnOut_OutputPort(portNum));                                        \
        ASSERT_TRUE(this->isConnected_to_arrayReturn##PORT_KIND(portNum));                                            \
                                                                                                                      \
        FormalParamArray returnVal = this->invoke_to_arrayReturn##PORT_KIND(portNum, port.args.val1, port.args.val2); \
                                                                                                                      \
        ASSERT_EQ(returnVal, this->arrayReturnVal.val);                                                               \
    }                                                                                                                 \
                                                                                                                      \
    void Tester ::test##PORT_KIND##PortInvoke(NATIVE_INT_TYPE portNum, FppTest::Types::StructReturn& port) {          \
        ASSERT_TRUE(component.isConnected_structReturnOut_OutputPort(portNum));                                       \
        ASSERT_TRUE(this->isConnected_to_structReturn##PORT_KIND(portNum));                                           \
                                                                                                                      \
        FormalParamStruct returnVal =                                                                                 \
            this->invoke_to_structReturn##PORT_KIND(portNum, port.args.val1, port.args.val2);                         \
                                                                                                                      \
        ASSERT_EQ(returnVal, this->structReturnVal.val);                                                              \
    }

// ----------------------------------------------------------------------
// Invoke serial input ports
// ----------------------------------------------------------------------

#define PORT_TEST_INVOKE_SERIAL_HELPER_DEF(PORT_KIND)                                             \
    void Tester ::invoke##PORT_KIND##SerialPort(NATIVE_INT_TYPE portNum, Fw::SerialBuffer& buf) { \
        ASSERT_TRUE(this->isConnected_to_serial##PORT_KIND(portNum));                             \
        this->invoke_to_serial##PORT_KIND(portNum, buf);                                          \
    }

#define PORT_TEST_INVOKE_SERIAL_HELPER_DEF_ASYNC                                          \
    void Tester ::invokeAsyncSerialPort(NATIVE_INT_TYPE portNum, Fw::SerialBuffer& buf) { \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                \
                                                                                          \
        switch (portNum) {                                                                \
            case SerialPortIndex::NO_ARGS:                                                \
            case SerialPortIndex::PRIMITIVE:                                              \
            case SerialPortIndex::STRING:                                                 \
                ASSERT_TRUE(this->isConnected_to_serialAsync(portNum));                   \
                this->invoke_to_serialAsync(portNum, buf);                                \
                break;                                                                    \
                                                                                          \
            case SerialPortIndex::ENUM:                                                   \
                ASSERT_TRUE(this->isConnected_to_serialAsyncAssert(0));                   \
                this->invoke_to_serialAsyncAssert(0, buf);                                \
                break;                                                                    \
                                                                                          \
            case SerialPortIndex::ARRAY:                                                  \
                ASSERT_TRUE(this->isConnected_to_serialAsyncBlockPriority(0));            \
                this->invoke_to_serialAsyncBlockPriority(0, buf);                         \
                break;                                                                    \
                                                                                          \
            case SerialPortIndex::STRUCT:                                                 \
                ASSERT_TRUE(this->isConnected_to_serialAsyncDropPriority(0));             \
                this->invoke_to_serialAsyncDropPriority(0, buf);                          \
                break;                                                                    \
        }                                                                                 \
                                                                                          \
        status = this->doDispatch();                                                      \
                                                                                          \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);   \
    }

#define PORT_TEST_INVOKE_SERIAL_DEFS(PORT_KIND)                                                                        \
    void Tester ::test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::NoParams& port) {         \
        ASSERT_TRUE(component.isConnected_serialOut_OutputPort(portNum));                                              \
                                                                                                                       \
        U8 data[0];                                                                                                    \
        Fw::SerialBuffer buf(data, sizeof(data));                                                                      \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::NO_ARGS, buf);                                            \
    }                                                                                                                  \
                                                                                                                       \
    void Tester ::test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveParams& port) {  \
        ASSERT_TRUE(component.isConnected_serialOut_OutputPort(portNum));                                              \
                                                                                                                       \
        Fw::SerializeStatus status;                                                                                    \
                                                                                                                       \
        /* Check unsuccessful deserialization of first parameter */                                                    \
        U8 invalidData1[0];                                                                                            \
        Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));                                              \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, invalidBuf1);                                  \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of second parameter */                                                   \
        U8 invalidData2[sizeof(U32)];                                                                                  \
        Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));                                              \
                                                                                                                       \
        status = invalidBuf2.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, invalidBuf2);                                  \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of third parameter */                                                    \
        U8 invalidData3[sizeof(U32) * 2];                                                                              \
        Fw::SerialBuffer invalidBuf3(invalidData3, sizeof(invalidData3));                                              \
                                                                                                                       \
        status = invalidBuf3.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf3.serialize(port.args.val2);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, invalidBuf3);                                  \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of fourth parameter */                                                   \
        U8 invalidData4[(sizeof(U32) * 2) + sizeof(F32)];                                                              \
        Fw::SerialBuffer invalidBuf4(invalidData4, sizeof(invalidData4));                                              \
                                                                                                                       \
        status = invalidBuf4.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf4.serialize(port.args.val2);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf4.serialize(port.args.val3);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, invalidBuf4);                                  \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of fifth parameter */                                                    \
        U8 invalidData5[(sizeof(U32) * 2) + (sizeof(F32) * 2)];                                                        \
        Fw::SerialBuffer invalidBuf5(invalidData5, sizeof(invalidData5));                                              \
                                                                                                                       \
        status = invalidBuf5.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf5.serialize(port.args.val2);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf5.serialize(port.args.val3);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf5.serialize(port.args.val4);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, invalidBuf5);                                  \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of sixth parameter */                                                    \
        U8 invalidData6[(sizeof(U32) * 2) + (sizeof(F32) * 2) + sizeof(U8)];                                           \
        Fw::SerialBuffer invalidBuf6(invalidData6, sizeof(invalidData6));                                              \
                                                                                                                       \
        status = invalidBuf6.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf6.serialize(port.args.val2);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf6.serialize(port.args.val3);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf6.serialize(port.args.val4);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf6.serialize(port.args.val5);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, invalidBuf6);                                  \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check successful serialization */                                                                           \
        U8 data[InputPrimitiveArgsPort::SERIALIZED_SIZE];                                                              \
        Fw::SerialBuffer buf(data, sizeof(data));                                                                      \
                                                                                                                       \
        status = buf.serialize(port.args.val1);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val2);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val3);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val4);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val5);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val6);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::PRIMITIVE, buf);                                          \
                                                                                                                       \
        this->checkSerializeStatusSuccess();                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    void Tester ::test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::PortStringParams& port) { \
        ASSERT_TRUE(component.isConnected_serialOut_OutputPort(portNum));                                              \
                                                                                                                       \
        Fw::SerializeStatus status;                                                                                    \
                                                                                                                       \
        /* Check unsuccessful deserialization of first parameter */                                                    \
        U8 invalidData1[0];                                                                                            \
        Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));                                              \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRING, invalidBuf1);                                     \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of second parameter */                                                   \
        U8 invalidData2[StringArgsPortStrings::StringSize80::SERIALIZED_SIZE];                                         \
        Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));                                              \
                                                                                                                       \
        status = invalidBuf2.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRING, invalidBuf2);                                     \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of third parameter */                                                    \
        U8 invalidData3[StringArgsPortStrings::StringSize80::SERIALIZED_SIZE * 2];                                     \
        Fw::SerialBuffer invalidBuf3(invalidData3, sizeof(invalidData3));                                              \
                                                                                                                       \
        status = invalidBuf3.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf3.serialize(port.args.val2);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRING, invalidBuf3);                                     \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of fourth parameter */                                                   \
        U8 invalidData4[(StringArgsPortStrings::StringSize80::SERIALIZED_SIZE * 2) +                                   \
                        StringArgsPortStrings::StringSize100::SERIALIZED_SIZE];                                        \
        Fw::SerialBuffer invalidBuf4(invalidData4, sizeof(invalidData4));                                              \
                                                                                                                       \
        status = invalidBuf4.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf4.serialize(port.args.val2);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = invalidBuf4.serialize(port.args.val3);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRING, invalidBuf4);                                     \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check successful serialization */                                                                           \
        U8 data[InputStringArgsPort::SERIALIZED_SIZE];                                                                 \
        Fw::SerialBuffer buf(data, sizeof(data));                                                                      \
                                                                                                                       \
        status = buf.serialize(port.args.val1);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val2);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val3);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val4);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRING, buf);                                             \
                                                                                                                       \
        this->checkSerializeStatusSuccess();                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    void Tester ::test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::EnumParams& port) {       \
        ASSERT_TRUE(component.isConnected_serialOut_OutputPort(portNum));                                              \
                                                                                                                       \
        Fw::SerializeStatus status;                                                                                    \
                                                                                                                       \
        /* Check unsuccessful deserialization of first parameter */                                                    \
        U8 invalidData1[0];                                                                                            \
        Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));                                              \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::ENUM, invalidBuf1);                                       \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of second parameter */                                                   \
        U8 invalidData2[FormalParamEnum::SERIALIZED_SIZE];                                                             \
        Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));                                              \
                                                                                                                       \
        status = invalidBuf2.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::ENUM, invalidBuf2);                                       \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check successful serialization */                                                                           \
        U8 data[InputEnumArgsPort::SERIALIZED_SIZE];                                                                   \
        Fw::SerialBuffer buf(data, sizeof(data));                                                                      \
                                                                                                                       \
        status = buf.serialize(port.args.val1);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val2);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::ENUM, buf);                                               \
                                                                                                                       \
        this->checkSerializeStatusSuccess();                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    void Tester ::test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParams& port) {      \
        ASSERT_TRUE(component.isConnected_serialOut_OutputPort(portNum));                                              \
                                                                                                                       \
        Fw::SerializeStatus status;                                                                                    \
                                                                                                                       \
        /* Check unsuccessful deserialization of first parameter */                                                    \
        U8 invalidData1[0];                                                                                            \
        Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));                                              \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::ARRAY, invalidBuf1);                                      \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of second parameter */                                                   \
        U8 invalidData2[FormalParamArray::SERIALIZED_SIZE];                                                            \
        Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));                                              \
                                                                                                                       \
        status = invalidBuf2.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::ARRAY, invalidBuf2);                                      \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        U8 data[InputArrayArgsPort::SERIALIZED_SIZE];                                                                  \
        Fw::SerialBuffer buf(data, sizeof(data));                                                                      \
                                                                                                                       \
        status = buf.serialize(port.args.val1);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val2);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::ARRAY, buf);                                              \
                                                                                                                       \
        this->checkSerializeStatusSuccess();                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    void Tester ::test##PORT_KIND##PortInvokeSerial(NATIVE_INT_TYPE portNum, FppTest::Types::StructParams& port) {     \
        ASSERT_TRUE(component.isConnected_serialOut_OutputPort(portNum));                                              \
                                                                                                                       \
        Fw::SerializeStatus status;                                                                                    \
                                                                                                                       \
        /* Check unsuccessful deserialization of first parameter */                                                    \
        U8 invalidData1[0];                                                                                            \
        Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));                                              \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRUCT, invalidBuf1);                                     \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        /* Check unsuccessful deserialization of second parameter */                                                   \
        U8 invalidData2[FormalParamStruct::SERIALIZED_SIZE];                                                           \
        Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));                                              \
                                                                                                                       \
        status = invalidBuf2.serialize(port.args.val1);                                                                \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRUCT, invalidBuf2);                                     \
                                                                                                                       \
        this->checkSerializeStatusBufferEmpty();                                                                       \
                                                                                                                       \
        U8 data[InputStructArgsPort::SERIALIZED_SIZE];                                                                 \
        Fw::SerialBuffer buf(data, sizeof(data));                                                                      \
                                                                                                                       \
        status = buf.serialize(port.args.val1);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        status = buf.serialize(port.args.val2);                                                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                                                        \
                                                                                                                       \
        this->invoke##PORT_KIND##SerialPort(SerialPortIndex::STRUCT, buf);                                             \
                                                                                                                       \
        this->checkSerializeStatusSuccess();                                                                           \
    }

// ----------------------------------------------------------------------
// Check history of typed output ports
// ----------------------------------------------------------------------

#define PORT_TEST_CHECK_DEFS(PORT_KIND)                                                                 \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::NoParams& port) {                          \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                               \
        ASSERT_from_noArgsOut_SIZE(1);                                                                  \
    }                                                                                                   \
                                                                                                        \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::PrimitiveParams& port) {                   \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                               \
        ASSERT_from_primitiveArgsOut_SIZE(1);                                                           \
        ASSERT_from_primitiveArgsOut(0, port.args.val1, port.args.val2, port.args.val3, port.args.val4, \
                                     port.args.val5, port.args.val6);                                   \
    }                                                                                                   \
                                                                                                        \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::PortStringParams& port) {                  \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                               \
        ASSERT_from_stringArgsOut_SIZE(1);                                                              \
        ASSERT_from_stringArgsOut(0, port.args.val1, port.args.val2, port.args.val3, port.args.val4);   \
    }                                                                                                   \
                                                                                                        \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::EnumParams& port) {                        \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                               \
        ASSERT_from_enumArgsOut_SIZE(1);                                                                \
        ASSERT_from_enumArgsOut(0, port.args.val1, port.args.val2);                                     \
    }                                                                                                   \
                                                                                                        \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::ArrayParams& port) {                       \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                               \
        ASSERT_from_arrayArgsOut_SIZE(1);                                                               \
        ASSERT_from_arrayArgsOut(0, port.args.val1, port.args.val2);                                    \
    }                                                                                                   \
                                                                                                        \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::StructParams& port) {                      \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                               \
        ASSERT_from_structArgsOut_SIZE(1);                                                              \
        ASSERT_from_structArgsOut(0, port.args.val1, port.args.val2);                                   \
    }

#define PORT_TEST_CHECK_RETURN_DEFS(PORT_KIND)                                                            \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::NoParamReturn& port) {                       \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                                 \
        ASSERT_from_noArgsReturnOut_SIZE(1);                                                              \
    }                                                                                                     \
                                                                                                          \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::PrimitiveReturn& port) {                     \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                                 \
        ASSERT_from_primitiveReturnOut_SIZE(1);                                                           \
        ASSERT_from_primitiveReturnOut(0, port.args.val1, port.args.val2, port.args.val3, port.args.val4, \
                                       port.args.val5, port.args.val6);                                   \
    }                                                                                                     \
                                                                                                          \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::EnumReturn& port) {                          \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                                 \
        ASSERT_from_enumReturnOut_SIZE(1);                                                                \
        ASSERT_from_enumReturnOut(0, port.args.val1, port.args.val2);                                     \
    }                                                                                                     \
                                                                                                          \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::ArrayReturn& port) {                         \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                                 \
        ASSERT_from_arrayReturnOut_SIZE(1);                                                               \
        ASSERT_from_arrayReturnOut(0, port.args.val1, port.args.val2);                                    \
    }                                                                                                     \
                                                                                                          \
    void Tester ::test##PORT_KIND##PortCheck(FppTest::Types::StructReturn& port) {                        \
        ASSERT_FROM_PORT_HISTORY_SIZE(1);                                                                 \
        ASSERT_from_structReturnOut_SIZE(1);                                                              \
        ASSERT_from_structReturnOut(0, port.args.val1, port.args.val2);                                   \
    }

// ----------------------------------------------------------------------
// Check serial output ports
// ----------------------------------------------------------------------

#define PORT_TEST_CHECK_SERIAL_DEFS(PORT_KIND)                                               \
    void Tester ::test##PORT_KIND##PortCheckSerial(FppTest::Types::NoParams& port) {}        \
                                                                                             \
    void Tester ::test##PORT_KIND##PortCheckSerial(FppTest::Types::PrimitiveParams& port) {  \
        Fw::SerializeStatus status;                                                          \
        U32 u32, u32Ref;                                                                     \
        F32 f32, f32Ref;                                                                     \
        bool b, bRef;                                                                        \
                                                                                             \
        status = this->primitiveBuf.deserialize(u32);                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->primitiveBuf.deserialize(u32Ref);                                     \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->primitiveBuf.deserialize(f32);                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->primitiveBuf.deserialize(f32Ref);                                     \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->primitiveBuf.deserialize(b);                                          \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->primitiveBuf.deserialize(bRef);                                       \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        ASSERT_EQ(u32, port.args.val1);                                                      \
        ASSERT_EQ(u32Ref, port.args.val2);                                                   \
        ASSERT_EQ(f32, port.args.val3);                                                      \
        ASSERT_EQ(f32Ref, port.args.val4);                                                   \
        ASSERT_EQ(b, port.args.val5);                                                        \
        ASSERT_EQ(bRef, port.args.val6);                                                     \
    }                                                                                        \
                                                                                             \
    void Tester ::test##PORT_KIND##PortCheckSerial(FppTest::Types::PortStringParams& port) { \
        Fw::SerializeStatus status;                                                          \
        StringArgsPortStrings::StringSize80 str80, str80Ref;                                 \
        StringArgsPortStrings::StringSize100 str100, str100Ref;                              \
                                                                                             \
        status = this->stringBuf.deserialize(str80);                                         \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->stringBuf.deserialize(str80Ref);                                      \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->stringBuf.deserialize(str100);                                        \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->stringBuf.deserialize(str100Ref);                                     \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        ASSERT_EQ(str80, port.args.val1);                                                    \
        ASSERT_EQ(str80Ref, port.args.val2);                                                 \
        ASSERT_EQ(str100, port.args.val3);                                                   \
        ASSERT_EQ(str100Ref, port.args.val4);                                                \
    }                                                                                        \
                                                                                             \
    void Tester ::test##PORT_KIND##PortCheckSerial(FppTest::Types::EnumParams& port) {       \
        Fw::SerializeStatus status;                                                          \
        FormalParamEnum en, enRef;                                                           \
                                                                                             \
        status = this->enumBuf.deserialize(en);                                              \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->enumBuf.deserialize(enRef);                                           \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        ASSERT_EQ(en, port.args.val1);                                                       \
        ASSERT_EQ(enRef, port.args.val2);                                                    \
    }                                                                                        \
                                                                                             \
    void Tester ::test##PORT_KIND##PortCheckSerial(FppTest::Types::ArrayParams& port) {      \
        Fw::SerializeStatus status;                                                          \
        FormalParamArray a, aRef;                                                            \
                                                                                             \
        status = this->arrayBuf.deserialize(a);                                              \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->arrayBuf.deserialize(aRef);                                           \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        ASSERT_EQ(a, port.args.val1);                                                        \
        ASSERT_EQ(aRef, port.args.val2);                                                     \
    }                                                                                        \
                                                                                             \
    void Tester ::test##PORT_KIND##PortCheckSerial(FppTest::Types::StructParams& port) {     \
        Fw::SerializeStatus status;                                                          \
        FormalParamStruct s, sRef;                                                           \
                                                                                             \
        status = this->structBuf.deserialize(s);                                             \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        status = this->structBuf.deserialize(sRef);                                          \
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);                                              \
                                                                                             \
        ASSERT_EQ(s, port.args.val1);                                                        \
        ASSERT_EQ(sRef, port.args.val2);                                                     \
    }

#define PORT_TEST_DEFS(PORT_KIND)                 \
    PORT_TEST_INVOKE_DEFS(PORT_KIND)              \
    PORT_TEST_INVOKE_RETURN_DEFS(PORT_KIND)       \
    PORT_TEST_INVOKE_SERIAL_HELPER_DEF(PORT_KIND) \
    PORT_TEST_INVOKE_SERIAL_DEFS(PORT_KIND)       \
    PORT_TEST_CHECK_DEFS(PORT_KIND)               \
    PORT_TEST_CHECK_RETURN_DEFS(PORT_KIND)        \
    PORT_TEST_CHECK_SERIAL_DEFS(PORT_KIND)

#define PORT_TEST_DEFS_ASYNC                 \
    PORT_TEST_INVOKE_DEFS(Async)             \
    PORT_TEST_INVOKE_SERIAL_HELPER_DEF_ASYNC \
    PORT_TEST_INVOKE_SERIAL_DEFS(Async)      \
    PORT_TEST_CHECK_DEFS(Async)              \
    PORT_TEST_CHECK_SERIAL_DEFS(Async)
