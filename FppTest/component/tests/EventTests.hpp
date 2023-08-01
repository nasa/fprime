// ======================================================================
// \title  EventTests.hpp
// \author T. Chieu
// \brief  hpp file for event tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

// ----------------------------------------------------------------------
// Event test declarations
// ----------------------------------------------------------------------

#define EVENT_TEST_DECL(TYPE) void testEvent(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& data);

#define EVENT_TEST_HELPER_DECL(TYPE) \
    void testEventHelper(NATIVE_INT_TYPE portNum, FppTest::Types::TYPE& data, NATIVE_UINT_TYPE size);

#define EVENT_TEST_DECLS                    \
    EVENT_TEST_DECL(NoParams)               \
    EVENT_TEST_HELPER_DECL(PrimitiveParams) \
    EVENT_TEST_DECL(PrimitiveParams)        \
    EVENT_TEST_DECL(LogStringParams)        \
    EVENT_TEST_DECL(EnumParam)              \
    EVENT_TEST_HELPER_DECL(ArrayParam)      \
    EVENT_TEST_DECL(ArrayParam)             \
    EVENT_TEST_DECL(StructParam)            \
    EVENT_TEST_HELPER_DECL(BoolParam)       \
    EVENT_TEST_DECL(BoolParam)
