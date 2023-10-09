/*
 * Main.cpp
 *
 *  Created on: Mar 18, 2015
 *  Updated: 6/22/2022
 *      Author: tcanham
 */

#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Svc/TlmChan/TlmChan.hpp>
#include "TlmChanTester.hpp"

TEST(TlmChanTest, InitTest) {
    Svc::TlmChanTester tester;
}

TEST(TlmChanTest, NominalChannelTest) {
    TEST_CASE(107.1.1, "Nominal channelized telemetry");
    COMMENT("Write a single channel and verify it is read back and pushed correctly.");

    Svc::TlmChanTester tester;
    // run test
    tester.runNominalChannel();
}

TEST(TlmChanTest, MultiChannelTest) {
    TEST_CASE(107.1.2, "Nominal Multi-channel channelized telemetry");
    COMMENT("Write multiple channels and verify they are read back and pushed correctly.");

    Svc::TlmChanTester tester;

    // run test
    tester.runMultiChannel();
}

TEST(TlmChanTest, OffNominal) {
    TEST_CASE(107.2.1, "Off-nominal channelized telemetry");
    COMMENT("Attempt to read a channel that hasn't been written.");

    Svc::TlmChanTester tester;

    // run test
    tester.runOffNominal();
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
