// ======================================================================
// \title  ExternalParamTests.hpp
// \author B. Campuzano
// \brief  hpp file for external parameter tests
//
// \copyright
// Copyright (C) 2009-2025 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

// ----------------------------------------------------------------------
// Parameter test declarations
// ----------------------------------------------------------------------

#define EXTERNAL_PARAM_CMD_TEST_DECL(TYPE) \
    void testExternalParamCommand(FwIndexType portNum, FppTest::Types::TYPE& data);

#define EXTERNAL_PARAM_CMD_TEST_DECLS            \
    EXTERNAL_PARAM_CMD_TEST_DECL(BoolParam)      \
    EXTERNAL_PARAM_CMD_TEST_DECL(I32Param)       \
    EXTERNAL_PARAM_CMD_TEST_DECL(PrmStringParam) \
    EXTERNAL_PARAM_CMD_TEST_DECL(EnumParam)      \
    EXTERNAL_PARAM_CMD_TEST_DECL(ArrayParam)     \
    EXTERNAL_PARAM_CMD_TEST_DECL(StructParam)
