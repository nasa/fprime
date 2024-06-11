// ======================================================================
// \title  TlmTests.hpp
// \author T. Chieu
// \brief  hpp file for telemetry tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

// ----------------------------------------------------------------------
// Telemetry test declarations
// ----------------------------------------------------------------------

#define TLM_TEST_DECL(TYPE) void testTelemetry(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE##Param& data);

#define TLM_TEST_DECLS       \
    TLM_TEST_DECL(U32)       \
    TLM_TEST_DECL(F32)       \
    TLM_TEST_DECL(TlmString) \
    TLM_TEST_DECL(Enum)      \
    TLM_TEST_DECL(Array)     \
    TLM_TEST_DECL(Struct)

// ----------------------------------------------------------------------
// Telemetry test definitions
// ----------------------------------------------------------------------

#define TLM_TEST_DEF(TYPE)                                                                    \
    void Tester ::testTelemetry(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE##Param& data) { \
        ASSERT_TRUE(component.isConnected_tlmOut_OutputPort(portNum));                        \
                                                                                              \
        component.tlmWrite_Channel##TYPE(data.args.val);                                      \
                                                                                              \
        ASSERT_TLM_SIZE(1);                                                                   \
        ASSERT_TLM_Channel##TYPE##_SIZE(1);                                                   \
        ASSERT_TLM_Channel##TYPE(0, data.args.val);                                           \
                                                                                              \
        Fw::Time time = Fw::ZERO_TIME;                                                        \
        component.tlmWrite_Channel##TYPE(data.args.val, time);                                \
                                                                                              \
        ASSERT_TLM_SIZE(2);                                                                   \
        ASSERT_TLM_Channel##TYPE##_SIZE(2);                                                   \
        ASSERT_TLM_Channel##TYPE(0, data.args.val);                                           \
    }

#define TLM_TEST_DEFS                                                                            \
    TLM_TEST_DEF(U32)                                                                            \
    TLM_TEST_DEF(F32)                                                                            \
                                                                                                 \
    void Tester ::testTelemetry(NATIVE_INT_TYPE portNum, FppTest::Types::TlmStringParam& data) { \
        ASSERT_TRUE(component.isConnected_tlmOut_OutputPort(portNum));                           \
                                                                                                 \
        component.tlmWrite_ChannelString(data.args.val);                                         \
                                                                                                 \
        ASSERT_TLM_SIZE(1);                                                                      \
        ASSERT_TLM_ChannelString_SIZE(1);                                                        \
        ASSERT_TLM_ChannelString(0, data.args.val.toChar());                                     \
                                                                                                 \
        /* Test unchanged value */                                                               \
        component.tlmWrite_ChannelString(data.args.val);                                         \
                                                                                                 \
        ASSERT_TLM_SIZE(1);                                                                      \
        ASSERT_TLM_ChannelString_SIZE(1);                                                        \
                                                                                                 \
        FppTest::Types::TlmStringParam data2;                                                    \
        while (data2.args.val == data.args.val) {                                                \
            data2 = FppTest::Types::TlmStringParam();                                            \
        }                                                                                        \
                                                                                                 \
        Fw::Time time = Fw::ZERO_TIME;                                                           \
        component.tlmWrite_ChannelString(data2.args.val, time);                                  \
                                                                                                 \
        ASSERT_TLM_SIZE(2);                                                                      \
        ASSERT_TLM_ChannelString_SIZE(2);                                                        \
        ASSERT_TLM_ChannelString(1, data2.args.val.toChar());                                    \
    }                                                                                            \
                                                                                                 \
    TLM_TEST_DEF(Enum)                                                                           \
    TLM_TEST_DEF(Array)                                                                          \
    TLM_TEST_DEF(Struct)
