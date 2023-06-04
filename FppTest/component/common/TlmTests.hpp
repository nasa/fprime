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
        component.tlmWrite_Channel##TYPE(data.args.val);                                      \
                                                                                              \
        ASSERT_TLM_SIZE(1);                                                                   \
        ASSERT_TLM_Channel##TYPE##_SIZE(1);                                                   \
        ASSERT_TLM_Channel##TYPE(portNum, data.args.val);                                     \
    }

#define TLM_TEST_DEFS                                                                            \
    TLM_TEST_DEF(U32)                                                                            \
    TLM_TEST_DEF(F32)                                                                            \
                                                                                                 \
    void Tester ::testTelemetry(NATIVE_INT_TYPE portNum, FppTest::Types::TlmStringParam& data) { \
        component.tlmWrite_ChannelString(data.args.val);                                         \
                                                                                                 \
        ASSERT_TLM_SIZE(1);                                                                      \
        ASSERT_TLM_ChannelString_SIZE(1);                                                        \
        ASSERT_TLM_ChannelString(portNum, data.args.val.toChar());                               \
    }                                                                                            \
                                                                                                 \
    TLM_TEST_DEF(Enum)                                                                           \
    TLM_TEST_DEF(Array)                                                                          \
    TLM_TEST_DEF(Struct)
