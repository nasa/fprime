/*
 * ActiveTextLoggerTester.cpp
 *
 *  Created on: May 10, 2017
 *      Author: Gorang Gandhi
 */

#include <Svc/ActiveTextLogger/test/ut/Tester.hpp>
#include <Svc/ActiveTextLogger/ActiveTextLoggerImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>


TEST(NominalTest,Nominal) {

    TEST_CASE(1,"Nominal Test");

    Svc::Tester tester;
    tester.run_nominal_test();

}

TEST(OffNominalTest,OffNominal) {

    TEST_CASE(1,"Off-Nominal Test");

    Svc::Tester tester;
    tester.run_off_nominal_test();

}


#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}

#endif

