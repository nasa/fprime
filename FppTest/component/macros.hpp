// ----------------------------------------------------------------------
// Telemetry tests
// ----------------------------------------------------------------------

#define TLM_TEST_DECL(type) \
  void testTelemetry( \
      NATIVE_INT_TYPE portNum, \
      FppTest::Types::type##Param& data \
  );

#define TLM_TEST_DECLS \
  TLM_TEST_DECL(U32) \
  TLM_TEST_DECL(F32) \
  TLM_TEST_DECL(TlmString) \
  TLM_TEST_DECL(Enum) \
  TLM_TEST_DECL(Array) \
  TLM_TEST_DECL(Struct)

#define TLM_TEST_DEF(type1, type2, type3, type4) \
  void Tester :: \
    testTelemetry( \
        NATIVE_INT_TYPE portNum, \
        FppTest::Types::type1##Param& data \
  ) \
  { \
    component.tlmWrite_Channel##type2(data.args.val); \
\
    ASSERT_TLM_SIZE(1); \
    ASSERT_TLM_Channel##type3##_SIZE(1); \
    ASSERT_TLM_Channel##type4(portNum, data.args.val); \
  }

#define TLM_TEST_DEFS \
  TLM_TEST_DEF(U32, U32, U32, U32) \
  TLM_TEST_DEF(F32, F32, F32, F32) \
\
  void Tester :: \
    testTelemetry( \
        NATIVE_INT_TYPE portNum, \
        FppTest::Types::TlmStringParam& data \
  ) \
  { \
    component.tlmWrite_ChannelString(data.args.val); \
\
    ASSERT_TLM_SIZE(1); \
    ASSERT_TLM_ChannelString_SIZE(1); \
    ASSERT_TLM_ChannelString(portNum, data.args.val.toChar()); \
  } \
\
  TLM_TEST_DEF(Enum, Enum, Enum, Enum) \
  TLM_TEST_DEF(Array, Array, Array, Array) \
  TLM_TEST_DEF(Struct, Struct, Struct, Struct)
