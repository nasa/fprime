// ======================================================================
// \title  AsyncTests.cpp
// \author T. Chieu
// \brief  cpp file for async component tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/component/types/FormalParamTypes.hpp"
#include "FppTest/typed_tests/ComponentTest.hpp"
#include "FppTest/typed_tests/PortTest.hpp"

// Typed async port tests
using TypedAsyncPortTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                           FppTest::Types::PrimitiveParams,
                                                           FppTest::Types::PortStringParams,
                                                           FppTest::Types::EnumParams,
                                                           FppTest::Types::ArrayParams,
                                                           FppTest::Types::StructParams>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, TypedAsyncPortTest, TypedAsyncPortTestImplementations);

// Serial async port tests
using SerialAsyncPortTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                            FppTest::Types::PrimitiveParams,
                                                            FppTest::Types::PortStringParams,
                                                            FppTest::Types::EnumParams,
                                                            FppTest::Types::ArrayParams,
                                                            FppTest::Types::StructParams>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, SerialAsyncPortTest, SerialAsyncPortTestImplementations);

// Async command tests
using AsyncCommandTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                         FppTest::Types::PrimitiveParams,
                                                         FppTest::Types::CmdStringParams,
                                                         FppTest::Types::EnumParam,
                                                         FppTest::Types::ArrayParam,
                                                         FppTest::Types::StructParam>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ComponentAsyncCommandTest, AsyncCommandTestImplementations);

// Internal interface tests
using InternalInterfaceTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                              FppTest::Types::PrimitiveParams,
                                                              FppTest::Types::InternalInterfaceStringParams,
                                                              FppTest::Types::EnumParam,
                                                              FppTest::Types::ArrayParam,
                                                              FppTest::Types::StructParam>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ComponentInternalInterfaceTest, InternalInterfaceTestImplementations);
