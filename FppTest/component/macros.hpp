// ----------------------------------------------------------------------
// Telemetry tests
// ----------------------------------------------------------------------

#define TEST_TLM_DECL(type) \
  void testTelemetry( \
      NATIVE_INT_TYPE portNum, \
      FppTest::Types::type##Param& data \
  );

#define TEST_TLM_DECLS \
  TEST_TLM_DECL(U32) \
  TEST_TLM_DECL(F32) \
  TEST_TLM_DECL(TlmString) \
  TEST_TLM_DECL(Enum) \
  TEST_TLM_DECL(Array) \
  TEST_TLM_DECL(Struct)

#define TEST_TLM_DEF(type1, type2, type3, type4) \
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

#define TEST_TLM_DEFS \
  TEST_TLM_DEF(U32, U32, U32, U32) \
  TEST_TLM_DEF(F32, F32, F32, F32) \
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
  TEST_TLM_DEF(Enum, Enum, Enum, Enum) \
  TEST_TLM_DEF(Array, Array, Array, Array) \
  TEST_TLM_DEF(Struct, Struct, Struct, Struct)
