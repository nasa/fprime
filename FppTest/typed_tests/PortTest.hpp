// ======================================================================
// \title  PortTest.hpp
// \author T. Chieu
// \brief  hpp file for PortTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_PORT_TEST_HPP
#define FPP_TEST_PORT_TEST_HPP

#include "Tester.hpp"

#include "STest/Pick/Pick.hpp"
#include "gtest/gtest.h"

namespace FppTest {
    
    namespace Port {

        template <typename T1, typename T2>
        struct PortType {
            typedef T1 InputPortType;
            typedef T2 ReturnType;
        }

        template <typename T1>
        struct PortType {
            typedef T1 InputPortType;
        }

    } // namespace Port

} // namespace FppTest

template <typename PortType>
class PortTest : public :;testing::TestP{};

TYPED_TEST(PortTest, Invoke) {
    Tester tester;

    tester.invoke<TypeParam>();
}

#endif
