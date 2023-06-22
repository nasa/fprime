// ======================================================================
// \title  ParamTests.hpp
// \author T. Chieu
// \brief  hpp file for parameter tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

// ----------------------------------------------------------------------
// Parameter test declarations
// ----------------------------------------------------------------------

#define PARAM_CMD_TEST_DECL(TYPE) void testParamCommand(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& data);

#define PARAM_CMD_TEST_DECLS            \
    PARAM_CMD_TEST_DECL(BoolParam)      \
    PARAM_CMD_TEST_DECL(U32Param)       \
    PARAM_CMD_TEST_DECL(PrmStringParam) \
    PARAM_CMD_TEST_DECL(EnumParam)      \
    PARAM_CMD_TEST_DECL(ArrayParam)     \
    PARAM_CMD_TEST_DECL(StructParam)
