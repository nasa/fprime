// ----------------------------------------------------------------------
// Event tests
// ----------------------------------------------------------------------

#define EVENT_TEST_DECL(TYPE) \
  void testEvent( \
      NATIVE_INT_TYPE portNum, \
      FppTest::Types::TYPE& data \
  );

#define EVENT_TEST_HELPER_DECL(TYPE) \
  void testEventHelper( \
      NATIVE_INT_TYPE portNum, \
      FppTest::Types::TYPE& data, \
      NATIVE_UINT_TYPE size \
  );

#define EVENT_TEST_DECLS \
  EVENT_TEST_DECL(NoParams) \
  EVENT_TEST_HELPER_DECL(PrimitiveParams) \
  EVENT_TEST_DECL(PrimitiveParams) \
  EVENT_TEST_DECL(LogStringParams) \
  EVENT_TEST_DECL(EnumParam) \
  EVENT_TEST_HELPER_DECL(ArrayParam) \
  EVENT_TEST_DECL(ArrayParam) \
  EVENT_TEST_DECL(StructParam) \
  EVENT_TEST_HELPER_DECL(BoolParam) \
  EVENT_TEST_DECL(BoolParam)

// ----------------------------------------------------------------------
// Telemetry tests
// ----------------------------------------------------------------------

#define TLM_TEST_DECL(TYPE) \
  void testTelemetry( \
      NATIVE_INT_TYPE portNum, \
      FppTest::Types::TYPE##Param& data \
  );

#define TLM_TEST_DECLS \
  TLM_TEST_DECL(U32) \
  TLM_TEST_DECL(F32) \
  TLM_TEST_DECL(TlmString) \
  TLM_TEST_DECL(Enum) \
  TLM_TEST_DECL(Array) \
  TLM_TEST_DECL(Struct)

#define TLM_TEST_DEF(TYPE) \
  void Tester :: \
    testTelemetry( \
        NATIVE_INT_TYPE portNum, \
        FppTest::Types::TYPE##Param& data \
  ) \
  { \
    component.tlmWrite_Channel##TYPE(data.args.val); \
\
    ASSERT_TLM_SIZE(1); \
    ASSERT_TLM_Channel##TYPE##_SIZE(1); \
    ASSERT_TLM_Channel##TYPE(portNum, data.args.val); \
  }

#define TLM_TEST_DEFS \
  TLM_TEST_DEF(U32) \
  TLM_TEST_DEF(F32) \
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
  TLM_TEST_DEF(Enum) \
  TLM_TEST_DEF(Array) \
  TLM_TEST_DEF(Struct)
