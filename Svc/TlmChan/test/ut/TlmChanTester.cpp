/*
 * PrmDbTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/TlmChan/test/ut/TlmChanImplTester.hpp>
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::TlmChanImpl& impl, Svc::TlmChanImplTester& tester) {

    // connect ports
    tester.connect_to_TlmRecv(0,impl.get_TlmRecv_InputPort(0));
    tester.connect_to_TlmGet(0,impl.get_TlmGet_InputPort(0));
    tester.connect_to_Run(0,impl.get_Run_InputPort(0));
    impl.set_PktSend_OutputPort(0,tester.get_from_PktSend(0));

#if FW_PORT_TRACING
    //Fw::PortBase::setTrace(true);
#endif

    //simpleReg.dump();
}


TEST(TlmChanTest,NominalChannelTest) {

    TEST_CASE(107.1.1,"Nominal channelized telemetry");
    COMMENT("Write a single channel and verify it is read back and pushed correctly.");

    Svc::TlmChanImpl impl("TlmChanImpl");

    impl.init(10,0);

    Svc::TlmChanImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test
    tester.runNominalChannel();

}

TEST(TlmChanTest,MultiChannelTest) {

    TEST_CASE(107.1.2,"Nominal Multi-channel channelized telemetry");
    COMMENT("Write multiple channels and verify they are read back and pushed correctly.");

    Svc::TlmChanImpl impl("TlmChanImpl");

    impl.init(10,0);

    Svc::TlmChanImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test
    tester.runMultiChannel();

}


TEST(TlmChanTest,OffNominal) {

    TEST_CASE(107.2.1,"Off-nominal channelized telemetry");
    COMMENT("Attempt to read a channel that hasn't been written.");

    Svc::TlmChanImpl impl("TlmChanImpl");

    impl.init(10,0);

    Svc::TlmChanImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test
    tester.runOffNominal();

}

TEST(TlmChanTest,TooManyChannels) {

    COMMENT("Too Many Channel Test");

    Svc::TlmChanImpl impl("TlmChanImpl");

    impl.init(10,0);

    Svc::TlmChanImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test
    tester.runTooManyChannels();

}

#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif

