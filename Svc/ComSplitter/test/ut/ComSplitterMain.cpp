/*
 * Main.cpp
 *
 *  Created on: March 9, 2017
 *      Author: Gorang Gandhi
 */

#include "ComSplitterTester.hpp"
#include <Svc/ComSplitter/ComSplitter.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>


TEST(TestNominal,Nominal) {

    Svc::ComSplitterTester tester;
    tester.test_nominal();

}


#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}

#endif

