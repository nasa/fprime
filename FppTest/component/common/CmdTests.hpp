// ----------------------------------------------------------------------
// Command test declarations
// ----------------------------------------------------------------------

#define CMD_TEST_DECL(TYPE) \
    void testCommand(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& data);

#define CMD_TEST_DECLS \
  CMD_TEST_DECL(NoParams) \
  CMD_TEST_DECL(PrimitiveParams) \
  CMD_TEST_DECL(CmdStringParams) \
  CMD_TEST_DECL(EnumParam) \
  CMD_TEST_DECL(ArrayParam) \
  CMD_TEST_DECL(StructParam)
