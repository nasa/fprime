// ====================================================================== 
// \title  UnitTester.hpp
// \author Rob Bocchino
// \brief  Unit test macros
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef Svc_UnitTest_HPP
#define Svc_UnitTest_HPP

#define VERBOSE 0

#if VERBOSE
#include "Fw/Test/UnitTest.hpp"
#define PRINT(s,...) printf(s, __VA_ARGS__)
#else
#define PRINT(s,...)
#define REQUIREMENT(x)
#define TEST_CASE(a, b)
#endif

#endif
