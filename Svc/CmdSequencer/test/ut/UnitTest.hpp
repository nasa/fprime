// ====================================================================== 
// \title  UnitTester.hpp
// \author Rob Bocchino
// \brief  Unit test macros
//
// \copyright
// Copyright (C) 2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

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
