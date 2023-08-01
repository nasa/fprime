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

#include "test/ut/Tester.hpp"

// ----------------------------------------------------------------------
// Command test declarations
// ----------------------------------------------------------------------

#define CMD_TEST_INVOKE_DECL(TYPE) void invoke##TYPE##Command(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf);

#define CMD_TEST_INVOKE_DECLS       \
    CMD_TEST_INVOKE_DECL(NoArgs)    \
    CMD_TEST_INVOKE_DECL(Primitive) \
    CMD_TEST_INVOKE_DECL(String)    \
    CMD_TEST_INVOKE_DECL(Enum)      \
    CMD_TEST_INVOKE_DECL(Array)     \
    CMD_TEST_INVOKE_DECL(Struct)

#define CMD_TEST_INVOKE_DECLS_ASYNC      \
    CMD_TEST_INVOKE_DECL(AsyncNoArgs)    \
    CMD_TEST_INVOKE_DECL(AsyncPrimitive) \
    CMD_TEST_INVOKE_DECL(AsyncString)    \
    CMD_TEST_INVOKE_DECL(AsyncEnum)      \
    CMD_TEST_INVOKE_DECL(AsyncArray)     \
    CMD_TEST_INVOKE_DECL(AsyncStruct)

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

#define CMD_TEST_INVOKE_DEFS                                                               \
    void Tester ::invokeNoArgsCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {    \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_NO_ARGS, 1, buf);             \
    }                                                                                      \
                                                                                           \
    void Tester ::invokePrimitiveCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) { \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_PRIMITIVE, 1, buf);           \
    }                                                                                      \
                                                                                           \
    void Tester ::invokeStringCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {    \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_STRINGS, 1, buf);             \
    }                                                                                      \
                                                                                           \
    void Tester ::invokeEnumCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {      \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ENUM, 1, buf);                \
    }                                                                                      \
                                                                                           \
    void Tester ::invokeArrayCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {     \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ARRAY, 1, buf);               \
    }                                                                                      \
                                                                                           \
    void Tester ::invokeStructCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {    \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_STRUCT, 1, buf);              \
    }

#define CMD_TEST_INVOKE_DEFS_ASYNC                                                              \
    void Tester ::invokeAsyncNoArgsCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {    \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                      \
                                                                                                \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ASYNC_NO_ARGS, 1, buf);            \
        status = this->doDispatch();                                                            \
                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);         \
    }                                                                                           \
                                                                                                \
    void Tester ::invokeAsyncPrimitiveCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) { \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                      \
                                                                                                \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ASYNC_PRIMITIVE, 1, buf);          \
        status = this->doDispatch();                                                            \
                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);         \
    }                                                                                           \
                                                                                                \
    void Tester ::invokeAsyncStringCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {    \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                      \
                                                                                                \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ASYNC_STRINGS, 1, buf);            \
        status = this->doDispatch();                                                            \
                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);         \
    }                                                                                           \
                                                                                                \
    void Tester ::invokeAsyncEnumCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {      \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                      \
                                                                                                \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ASYNC_ENUM, 1, buf);               \
        status = this->doDispatch();                                                            \
                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);         \
    }                                                                                           \
                                                                                                \
    void Tester ::invokeAsyncArrayCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {     \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                      \
                                                                                                \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ASYNC_ARRAY, 1, buf);              \
        status = this->doDispatch();                                                            \
                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);         \
    }                                                                                           \
                                                                                                \
    void Tester ::invokeAsyncStructCommand(NATIVE_INT_TYPE portNum, Fw::CmdArgBuffer& buf) {    \
        Fw::QueuedComponentBase::MsgDispatchStatus status;                                      \
                                                                                                \
        this->invoke_to_cmdOut(portNum, component.OPCODE_CMD_ASYNC_STRUCT, 1, buf);             \
        status = this->doDispatch();                                                            \
                                                                                                \
        ASSERT_EQ(status, Fw::QueuedComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);         \
    }

#define CMD_TEST_DEFS(ASYNC)                                                                             \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::NoParams& data) {        \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                           \
                                                                                                         \
        component.regCommands();                                                                         \
                                                                                                         \
        Fw::CmdArgBuffer buf;                                                                            \
                                                                                                         \
        /* Test success */                                                                               \
        this->invoke##ASYNC##NoArgsCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);                                                         \
                                                                                                         \
        /* Test too many arguments */                                                                    \
        buf.serialize(0);                                                                                \
        this->invoke##ASYNC##NoArgsCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
    }                                                                                                    \
                                                                                                         \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::PrimitiveParams& data) { \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                           \
                                                                                                         \
        component.regCommands();                                                                         \
                                                                                                         \
        Fw::CmdArgBuffer buf;                                                                            \
                                                                                                         \
        /* Test incorrect deserialization of first argument */                                           \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test incorrect deserialization of second argument */                                          \
        buf.serialize(data.args.val1);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test incorrect deserialization of third argument */                                           \
        buf.serialize(data.args.val2);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test incorrect deserialization of fourth argument */                                          \
        buf.serialize(data.args.val3);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test incorrect deserialization of fifth argument */                                           \
        buf.serialize(data.args.val4);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test incorrect deserialization of sixth argument */                                           \
        buf.serialize(data.args.val5);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test success */                                                                               \
        buf.serialize(data.args.val6);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
                                                                                                         \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);                                                         \
        ASSERT_EQ(component.primitiveCmd.args.val1, data.args.val1);                                     \
        ASSERT_EQ(component.primitiveCmd.args.val2, data.args.val2);                                     \
        ASSERT_EQ(component.primitiveCmd.args.val3, data.args.val3);                                     \
        ASSERT_EQ(component.primitiveCmd.args.val4, data.args.val4);                                     \
        ASSERT_EQ(component.primitiveCmd.args.val5, data.args.val5);                                     \
        ASSERT_EQ(component.primitiveCmd.args.val6, data.args.val6);                                     \
                                                                                                         \
        /* Test too many arguments */                                                                    \
        buf.serialize(data.args.val5);                                                                   \
        this->invoke##ASYNC##PrimitiveCommand(portNum, buf);                                             \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
    }                                                                                                    \
                                                                                                         \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::CmdStringParams& data) { \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                           \
                                                                                                         \
        component.regCommands();                                                                         \
                                                                                                         \
        Fw::CmdArgBuffer buf;                                                                            \
                                                                                                         \
        /* Test incorrect serialization of first argument */                                             \
        this->invoke##ASYNC##StringCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test incorrect serialization of second argument */                                            \
        buf.serialize(data.args.val1);                                                                   \
        this->invoke##ASYNC##StringCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test success */                                                                               \
        buf.serialize(data.args.val2);                                                                   \
        this->invoke##ASYNC##StringCommand(portNum, buf);                                                \
                                                                                                         \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);                                                         \
        ASSERT_EQ(component.stringCmd.args.val1, data.args.val1);                                        \
        ASSERT_EQ(component.stringCmd.args.val2, data.args.val2);                                        \
                                                                                                         \
        /* Test too many arguments */                                                                    \
        buf.serialize(data.args.val1);                                                                   \
        this->invoke##ASYNC##StringCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
    }                                                                                                    \
                                                                                                         \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::EnumParam& data) {       \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                           \
                                                                                                         \
        component.regCommands();                                                                         \
                                                                                                         \
        Fw::CmdArgBuffer buf;                                                                            \
                                                                                                         \
        /* Test incorrect serialization of first argument */                                             \
        this->invoke##ASYNC##EnumCommand(portNum, buf);                                                  \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test success */                                                                               \
        buf.serialize(data.args.val);                                                                    \
        this->invoke##ASYNC##EnumCommand(portNum, buf);                                                  \
                                                                                                         \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);                                                         \
        ASSERT_EQ(component.enumCmd.args.val, data.args.val);                                            \
                                                                                                         \
        /* Test too many arguments */                                                                    \
        buf.serialize(data.args.val);                                                                    \
        this->invoke##ASYNC##EnumCommand(portNum, buf);                                                  \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
    }                                                                                                    \
                                                                                                         \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::ArrayParam& data) {      \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                           \
                                                                                                         \
        component.regCommands();                                                                         \
                                                                                                         \
        Fw::CmdArgBuffer buf;                                                                            \
                                                                                                         \
        /* Test incorrect serialization of first argument */                                             \
        this->invoke##ASYNC##ArrayCommand(portNum, buf);                                                 \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test success */                                                                               \
        buf.serialize(data.args.val);                                                                    \
        this->invoke##ASYNC##ArrayCommand(portNum, buf);                                                 \
                                                                                                         \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);                                                         \
        ASSERT_EQ(component.arrayCmd.args.val, data.args.val);                                           \
                                                                                                         \
        /* Test too many arguments */                                                                    \
        buf.serialize(data.args.val);                                                                    \
        this->invoke##ASYNC##ArrayCommand(portNum, buf);                                                 \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
    }                                                                                                    \
                                                                                                         \
    void Tester ::test##ASYNC##Command(NATIVE_INT_TYPE portNum, FppTest::Types::StructParam& data) {     \
        ASSERT_TRUE(component.isConnected_cmdRegOut_OutputPort(portNum));                                \
        ASSERT_TRUE(component.isConnected_cmdResponseOut_OutputPort(portNum));                           \
                                                                                                         \
        component.regCommands();                                                                         \
                                                                                                         \
        Fw::CmdArgBuffer buf;                                                                            \
                                                                                                         \
        /* Test incorrect serialization of first argument */                                             \
        this->invoke##ASYNC##StructCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
                                                                                                         \
        /* Test success */                                                                               \
        buf.serialize(data.args.val);                                                                    \
        this->invoke##ASYNC##StructCommand(portNum, buf);                                                \
                                                                                                         \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::OK);                                                         \
        ASSERT_EQ(component.structCmd.args.val, data.args.val);                                          \
                                                                                                         \
        /* Test too many arguments */                                                                    \
        buf.serialize(data.args.val);                                                                    \
        this->invoke##ASYNC##StructCommand(portNum, buf);                                                \
        ASSERT_EQ(cmdResp, Fw::CmdResponse::FORMAT_ERROR);                                               \
    }
