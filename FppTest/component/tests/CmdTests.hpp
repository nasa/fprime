// ======================================================================
// \title  CmdTests.hpp
// \author T. Chieu
// \brief  hpp file for command tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

// ----------------------------------------------------------------------
// Command test declarations
// ----------------------------------------------------------------------

#define CMD_TEST_INVOKE_DECL(TYPE, ASYNC) void invoke##ASYNC##Command(FppTest::Types::TYPE& data);

#define CMD_TEST_INVOKE_DECLS                                       \
    void invokeCommand(FwOpcodeType opcode, Fw::CmdArgBuffer& buf); \
    CMD_TEST_INVOKE_DECL(NoParams, )                                \
    CMD_TEST_INVOKE_DECL(PrimitiveParams, )                         \
    CMD_TEST_INVOKE_DECL(CmdStringParams, )                         \
    CMD_TEST_INVOKE_DECL(EnumParam, )                               \
    CMD_TEST_INVOKE_DECL(ArrayParam, )                              \
    CMD_TEST_INVOKE_DECL(StructParam, )

#define CMD_TEST_INVOKE_DECLS_ASYNC                                      \
    void invokeAsyncCommand(FwOpcodeType opcode, Fw::CmdArgBuffer& buf); \
    CMD_TEST_INVOKE_DECL(NoParams, Async)                                \
    CMD_TEST_INVOKE_DECL(PrimitiveParams, Async)                         \
    CMD_TEST_INVOKE_DECL(CmdStringParams, Async)                         \
    CMD_TEST_INVOKE_DECL(EnumParam, Async)                               \
    CMD_TEST_INVOKE_DECL(ArrayParam, Async)                              \
    CMD_TEST_INVOKE_DECL(StructParam, Async)

#define CMD_TEST_DECL(TYPE, ASYNC) void test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& data);

#define CMD_TEST_DECLS               \
    CMD_TEST_INVOKE_DECLS            \
    CMD_TEST_DECL(NoParams, )        \
    CMD_TEST_DECL(PrimitiveParams, ) \
    CMD_TEST_DECL(CmdStringParams, ) \
    CMD_TEST_DECL(EnumParam, )       \
    CMD_TEST_DECL(ArrayParam, )      \
    CMD_TEST_DECL(StructParam, )

#define CMD_TEST_DECLS_ASYNC              \
    CMD_TEST_INVOKE_DECLS_ASYNC           \
    CMD_TEST_DECL(NoParams, Async)        \
    CMD_TEST_DECL(PrimitiveParams, Async) \
    CMD_TEST_DECL(CmdStringParams, Async) \
    CMD_TEST_DECL(EnumParam, Async)       \
    CMD_TEST_DECL(ArrayParam, Async)      \
    CMD_TEST_DECL(StructParam, Async)

// ----------------------------------------------------------------------
// Command test definitions
// ----------------------------------------------------------------------

#define CMD_TEST_INVOKE_DEFS                                                                              \
    void Tester ::invokeCommand(FwOpcodeType opcode, Fw::CmdArgBuffer& buf) {                             \
        this->sendRawCmd(opcode, 1, buf);                                                                 \
    }                                                                                                     \
                                                                                                          \
    void Tester ::invokeCommand(FppTest::Types::NoParams& data) {                                         \
        this->sendCmd_CMD_NO_ARGS(0, 1);                                                                  \
    }                                                                                                     \
                                                                                                          \
    void Tester ::invokeCommand(FppTest::Types::PrimitiveParams& data) {                                  \
        this->sendCmd_CMD_PRIMITIVE(0, 1, data.args.val1, data.args.val2, data.args.val3, data.args.val4, \
                                    data.args.val5, data.args.val6);                                      \
    }                                                                                                     \
                                                                                                          \
    void Tester ::invokeCommand(FppTest::Types::CmdStringParams& data) {                                  \
        this->sendCmd_CMD_STRINGS(0, 1, data.args.val1, data.args.val2);                                  \
    }                                                                                                     \
                                                                                                          \
    void Tester ::invokeCommand(FppTest::Types::EnumParam& data) {                                        \
        this->sendCmd_CMD_ENUM(0, 1, data.args.val);                                                      \
    }                                                                                                     \
                                                                                                          \
    void Tester ::invokeCommand(FppTest::Types::ArrayParam& data) {                                       \
        this->sendCmd_CMD_ARRAY(0, 1, data.args.val);                                                     \
    }                                                                                                     \
                                                                                                          \
    void Tester ::invokeCommand(FppTest::Types::StructParam& data) {                                      \
        this->sendCmd_CMD_STRUCT(0, 1, data.args.val);                                                    \
    }

#define CMD_TEST_INVOKE_DEFS_ASYNC                                                                              \
    void Tester ::invokeAsyncCommand(FwOpcodeType opcode, Fw::CmdArgBuffer& buf) {                              \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendRawCmd(opcode, 1, buf);                                                                       \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }                                                                                                           \
                                                                                                                \
    void Tester ::invokeAsyncCommand(FppTest::Types::NoParams& data) {                                          \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendCmd_CMD_ASYNC_NO_ARGS(0, 1);                                                                  \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }                                                                                                           \
                                                                                                                \
    void Tester ::invokeAsyncCommand(FppTest::Types::PrimitiveParams& data) {                                   \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendCmd_CMD_ASYNC_PRIMITIVE(0, 1, data.args.val1, data.args.val2, data.args.val3, data.args.val4, \
                                          data.args.val5, data.args.val6);                                      \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }                                                                                                           \
                                                                                                                \
    void Tester ::invokeAsyncCommand(FppTest::Types::CmdStringParams& data) {                                   \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendCmd_CMD_ASYNC_STRINGS(0, 1, data.args.val1, data.args.val2);                                  \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }                                                                                                           \
                                                                                                                \
    void Tester ::invokeAsyncCommand(FppTest::Types::EnumParam& data) {                                         \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendCmd_CMD_ASYNC_ENUM(0, 1, data.args.val);                                                      \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }                                                                                                           \
                                                                                                                \
    void Tester ::invokeAsyncCommand(FppTest::Types::ArrayParam& data) {                                        \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendCmd_CMD_ASYNC_ARRAY(0, 1, data.args.val);                                                     \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }                                                                                                           \
                                                                                                                \
    void Tester ::invokeAsyncCommand(FppTest::Types::StructParam& data) {                                       \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                                      \
                                                                                                                \
        this->sendCmd_CMD_ASYNC_STRUCT(0, 1, data.args.val);                                                    \
        status = this->doDispatch();                                                                            \
                                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);                         \
    }

#define CMD_TEST_DEFS(ASYNC, _ASYNC)                                                                        \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::NoParams& data) {           \
        ASSERT_TRUE(this->isConnected_to_cmdIn(portNum));                                                   \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                   \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                              \
                                                                                                            \
        component.regCommands();                                                                            \
                                                                                                            \
        Fw::CmdArgBuffer buf;                                                                               \
                                                                                                            \
        /* Test success */                                                                                  \
        this->invoke##ASYNC##Command(data);                                                                 \
        ASSERT_CMD_RESPONSE_SIZE(1);                                                                        \
        ASSERT_CMD_RESPONSE(0, component.OPCODE_CMD##_ASYNC##_NO_ARGS, 1, Fw::CmdResponse::OK);             \
                                                                                                            \
        /* Test too many arguments */                                                                       \
        buf.serialize(0);                                                                                   \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_NO_ARGS, buf);                          \
        ASSERT_CMD_RESPONSE_SIZE(2);                                                                        \
        ASSERT_CMD_RESPONSE(1, component.OPCODE_CMD##_ASYNC##_NO_ARGS, 1, Fw::CmdResponse::FORMAT_ERROR);   \
    }                                                                                                       \
                                                                                                            \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveParams& data) {    \
        ASSERT_TRUE(this->isConnected_to_cmdIn(portNum));                                                   \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                   \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                              \
                                                                                                            \
        component.regCommands();                                                                            \
                                                                                                            \
        Fw::CmdArgBuffer buf;                                                                               \
                                                                                                            \
        /* Test incorrect deserialization of first argument */                                              \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(1);                                                                        \
        ASSERT_CMD_RESPONSE(0, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
                                                                                                            \
        /* Test incorrect deserialization of second argument */                                             \
        buf.serialize(data.args.val1);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(2);                                                                        \
        ASSERT_CMD_RESPONSE(1, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
                                                                                                            \
        /* Test incorrect deserialization of third argument */                                              \
        buf.serialize(data.args.val2);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(3);                                                                        \
        ASSERT_CMD_RESPONSE(2, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
                                                                                                            \
        /* Test incorrect deserialization of fourth argument */                                             \
        buf.serialize(data.args.val3);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(4);                                                                        \
        ASSERT_CMD_RESPONSE(3, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
                                                                                                            \
        /* Test incorrect deserialization of fifth argument */                                              \
        buf.serialize(data.args.val4);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(5);                                                                        \
        ASSERT_CMD_RESPONSE(4, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
                                                                                                            \
        /* Test incorrect deserialization of sixth argument */                                              \
        buf.serialize(data.args.val5);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(6);                                                                        \
        ASSERT_CMD_RESPONSE(5, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
                                                                                                            \
        /* Test success */                                                                                  \
        buf.serialize(data.args.val6);                                                                      \
        this->invoke##ASYNC##Command(data);                                                                 \
                                                                                                            \
        ASSERT_CMD_RESPONSE_SIZE(7);                                                                        \
        ASSERT_CMD_RESPONSE(6, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::OK);           \
        ASSERT_EQ(component.primitiveCmd.args.val1, data.args.val1);                                        \
        ASSERT_EQ(component.primitiveCmd.args.val2, data.args.val2);                                        \
        ASSERT_EQ(component.primitiveCmd.args.val3, data.args.val3);                                        \
        ASSERT_EQ(component.primitiveCmd.args.val4, data.args.val4);                                        \
        ASSERT_EQ(component.primitiveCmd.args.val5, data.args.val5);                                        \
        ASSERT_EQ(component.primitiveCmd.args.val6, data.args.val6);                                        \
                                                                                                            \
        /* Test too many arguments */                                                                       \
        buf.serialize(data.args.val5);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_PRIMITIVE, buf);                        \
        ASSERT_CMD_RESPONSE_SIZE(8);                                                                        \
        ASSERT_CMD_RESPONSE(7, component.OPCODE_CMD##_ASYNC##_PRIMITIVE, 1, Fw::CmdResponse::FORMAT_ERROR); \
    }                                                                                                       \
                                                                                                            \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::CmdStringParams& data) {    \
        ASSERT_TRUE(this->isConnected_to_cmdIn(portNum));                                                   \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                   \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                              \
                                                                                                            \
        component.regCommands();                                                                            \
                                                                                                            \
        Fw::CmdArgBuffer buf;                                                                               \
                                                                                                            \
        /* Test incorrect serialization of first argument */                                                \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_STRINGS, buf);                          \
        ASSERT_CMD_RESPONSE_SIZE(1);                                                                        \
        ASSERT_CMD_RESPONSE(0, component.OPCODE_CMD##_ASYNC##_STRINGS, 1, Fw::CmdResponse::FORMAT_ERROR);   \
                                                                                                            \
        /* Test incorrect serialization of second argument */                                               \
        buf.serialize(data.args.val1);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_STRINGS, buf);                          \
        ASSERT_CMD_RESPONSE_SIZE(2);                                                                        \
        ASSERT_CMD_RESPONSE(1, component.OPCODE_CMD##_ASYNC##_STRINGS, 1, Fw::CmdResponse::FORMAT_ERROR);   \
                                                                                                            \
        /* Test success */                                                                                  \
        buf.serialize(data.args.val2);                                                                      \
        this->invoke##ASYNC##Command(data);                                                                 \
                                                                                                            \
        ASSERT_CMD_RESPONSE_SIZE(3);                                                                        \
        ASSERT_CMD_RESPONSE(2, component.OPCODE_CMD##_ASYNC##_STRINGS, 1, Fw::CmdResponse::OK);             \
        ASSERT_EQ(component.stringCmd.args.val1, data.args.val1);                                           \
        ASSERT_EQ(component.stringCmd.args.val2, data.args.val2);                                           \
                                                                                                            \
        /* Test too many arguments */                                                                       \
        buf.serialize(data.args.val1);                                                                      \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_STRINGS, buf);                          \
        ASSERT_CMD_RESPONSE_SIZE(4);                                                                        \
        ASSERT_CMD_RESPONSE(3, component.OPCODE_CMD##_ASYNC##_STRINGS, 1, Fw::CmdResponse::FORMAT_ERROR);   \
    }                                                                                                       \
                                                                                                            \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::EnumParam& data) {          \
        ASSERT_TRUE(this->isConnected_to_cmdIn(portNum));                                                   \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                   \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                              \
                                                                                                            \
        component.regCommands();                                                                            \
                                                                                                            \
        Fw::CmdArgBuffer buf;                                                                               \
                                                                                                            \
        /* Test incorrect serialization of first argument */                                                \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_ENUM, buf);                             \
        ASSERT_CMD_RESPONSE_SIZE(1);                                                                        \
        ASSERT_CMD_RESPONSE(0, component.OPCODE_CMD##_ASYNC##_ENUM, 1, Fw::CmdResponse::FORMAT_ERROR);      \
                                                                                                            \
        /* Test success */                                                                                  \
        buf.serialize(data.args.val);                                                                       \
        this->invoke##ASYNC##Command(data);                                                                 \
                                                                                                            \
        ASSERT_CMD_RESPONSE_SIZE(2);                                                                        \
        ASSERT_CMD_RESPONSE(1, component.OPCODE_CMD##_ASYNC##_ENUM, 1, Fw::CmdResponse::OK);                \
        ASSERT_EQ(component.enumCmd.args.val, data.args.val);                                               \
                                                                                                            \
        /* Test too many arguments */                                                                       \
        buf.serialize(data.args.val);                                                                       \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_ENUM, buf);                             \
        ASSERT_CMD_RESPONSE_SIZE(3);                                                                        \
        ASSERT_CMD_RESPONSE(2, component.OPCODE_CMD##_ASYNC##_ENUM, 1, Fw::CmdResponse::FORMAT_ERROR);      \
    }                                                                                                       \
                                                                                                            \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParam& data) {         \
        ASSERT_TRUE(this->isConnected_to_cmdIn(portNum));                                                   \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                   \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                              \
                                                                                                            \
        component.regCommands();                                                                            \
                                                                                                            \
        Fw::CmdArgBuffer buf;                                                                               \
                                                                                                            \
        /* Test incorrect serialization of first argument */                                                \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_ARRAY, buf);                            \
        ASSERT_CMD_RESPONSE_SIZE(1);                                                                        \
        ASSERT_CMD_RESPONSE(0, component.OPCODE_CMD##_ASYNC##_ARRAY, 1, Fw::CmdResponse::FORMAT_ERROR);     \
                                                                                                            \
        /* Test success */                                                                                  \
        buf.serialize(data.args.val);                                                                       \
        this->invoke##ASYNC##Command(data);                                                                 \
                                                                                                            \
        ASSERT_CMD_RESPONSE_SIZE(2);                                                                        \
        ASSERT_CMD_RESPONSE(1, component.OPCODE_CMD##_ASYNC##_ARRAY, 1, Fw::CmdResponse::OK);               \
        ASSERT_EQ(component.arrayCmd.args.val, data.args.val);                                              \
                                                                                                            \
        /* Test too many arguments */                                                                       \
        buf.serialize(data.args.val);                                                                       \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_ARRAY, buf);                            \
        ASSERT_CMD_RESPONSE_SIZE(3);                                                                        \
        ASSERT_CMD_RESPONSE(2, component.OPCODE_CMD##_ASYNC##_ARRAY, 1, Fw::CmdResponse::FORMAT_ERROR);     \
    }                                                                                                       \
                                                                                                            \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::StructParam& data) {        \
        ASSERT_TRUE(this->isConnected_to_cmdIn(portNum));                                                   \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                   \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                              \
                                                                                                            \
        component.regCommands();                                                                            \
                                                                                                            \
        Fw::CmdArgBuffer buf;                                                                               \
                                                                                                            \
        /* Test incorrect serialization of first argument */                                                \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_STRUCT, buf);                           \
        ASSERT_CMD_RESPONSE_SIZE(1);                                                                        \
        ASSERT_CMD_RESPONSE(0, component.OPCODE_CMD##_ASYNC##_STRUCT, 1, Fw::CmdResponse::FORMAT_ERROR);    \
                                                                                                            \
        /* Test success */                                                                                  \
        buf.serialize(data.args.val);                                                                       \
        this->invoke##ASYNC##Command(data);                                                                 \
                                                                                                            \
        ASSERT_CMD_RESPONSE_SIZE(2);                                                                        \
        ASSERT_CMD_RESPONSE(1, component.OPCODE_CMD##_ASYNC##_STRUCT, 1, Fw::CmdResponse::OK);              \
        ASSERT_EQ(component.structCmd.args.val, data.args.val);                                             \
                                                                                                            \
        /* Test too many arguments */                                                                       \
        buf.serialize(data.args.val);                                                                       \
        this->invoke##ASYNC##Command(component.OPCODE_CMD##_ASYNC##_STRUCT, buf);                           \
        ASSERT_CMD_RESPONSE_SIZE(3);                                                                        \
        ASSERT_CMD_RESPONSE(2, component.OPCODE_CMD##_ASYNC##_STRUCT, 1, Fw::CmdResponse::FORMAT_ERROR);    \
    }
