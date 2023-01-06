/*
<<<<<<< HEAD
 * PrmDbTester.cpp
=======
 * Main.cpp
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
 *
 *  Created on: Mar 18, 2015
 *  Updated: 6/22/2022
 *      Author: tcanham
 */

<<<<<<< HEAD
#include <Svc/TlmChan/test/ut/Tester.hpp>
#include <Svc/TlmChan/TlmChan.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>


TEST(TlmChanTest,InitTest) {
    Svc::Tester tester;
}

TEST(TlmChanTest,NominalChannelTest) {

    TEST_CASE(107.1.1,"Nominal channelized telemetry");
=======
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Svc/TlmChan/TlmChan.hpp>
#include <Svc/TlmChan/test/ut/Tester.hpp>

TEST(TlmChanTest, InitTest) {
    Svc::Tester tester;
}

TEST(TlmChanTest, NominalChannelTest) {
    TEST_CASE(107.1.1, "Nominal channelized telemetry");
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
    COMMENT("Write a single channel and verify it is read back and pushed correctly.");

    Svc::Tester tester;
    // run test
    tester.runNominalChannel();
<<<<<<< HEAD

}

TEST(TlmChanTest,MultiChannelTest) {

    TEST_CASE(107.1.2,"Nominal Multi-channel channelized telemetry");
=======
}

TEST(TlmChanTest, MultiChannelTest) {
    TEST_CASE(107.1.2, "Nominal Multi-channel channelized telemetry");
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
    COMMENT("Write multiple channels and verify they are read back and pushed correctly.");

    Svc::Tester tester;

    // run test
    tester.runMultiChannel();
<<<<<<< HEAD

}


TEST(TlmChanTest,OffNominal) {

    TEST_CASE(107.2.1,"Off-nominal channelized telemetry");
=======
}

TEST(TlmChanTest, OffNominal) {
    TEST_CASE(107.2.1, "Off-nominal channelized telemetry");
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
    COMMENT("Attempt to read a channel that hasn't been written.");

    Svc::Tester tester;

    // run test
    tester.runOffNominal();
<<<<<<< HEAD

=======
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
}

// TEST(TlmChanTest,TooManyChannels) {

//     COMMENT("Too Many Channel Test");

//     Svc::TlmChanImpl impl("TlmChanImpl");

//     impl.init(10,0);

//     Svc::TlmChanImplTester tester(impl);

//     tester.init();

//     // connect ports
//     connectPorts(impl,tester);

//     // run test
//     tester.runTooManyChannels();

// }

<<<<<<< HEAD


=======
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
<<<<<<< HEAD

=======
>>>>>>> d60a606905dd70cc9b8024b95ca6eeab605976e8
