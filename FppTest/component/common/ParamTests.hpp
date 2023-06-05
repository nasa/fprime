// ----------------------------------------------------------------------
// Parameter test declarations
// ----------------------------------------------------------------------

#define PARAM_CMD_TEST_DECL(TYPE) \
    void testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& data);

#define PARAM_CMD_TEST_DECLS \
  PARAM_CMD_TEST_DECL(BoolParam) \
  PARAM_CMD_TEST_DECL(U32Param) \
  PARAM_CMD_TEST_DECL(PrmStringParam) \
  PARAM_CMD_TEST_DECL(EnumParam) \
  PARAM_CMD_TEST_DECL(ArrayParam) \
  PARAM_CMD_TEST_DECL(StructParam)
