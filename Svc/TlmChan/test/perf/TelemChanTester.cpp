/*
 * PrmDbTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/TlmChan/test/ut/TelemChanImplTester.hpp>
#include <Svc/TlmChan/TelemChanImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::TelemChanImpl& impl, Svc::TelemChanImplTester& tester) {

    // parameter ports
    tester.set_tlmRecv_OutputPort(0,impl.get_tlmRecv_InputPort(0));
    tester.set_tlmGet_OutputPort(0,impl.get_tlmGet_InputPort(0));

#if FW_PORT_TRACING
    //Fw::PortBase::setTrace(true);
#endif

    //simpleReg.dump();
}

void runTimingTest(U32 iterations) {

    Svc::TelemChanImpl impl("TlmChanImpl");

    impl.init(10);

    Svc::TelemChanImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run timing test
    tester.doPerfTest(iterations);

}

extern "C" {
    void runTest();
}

void runTest(U32 iterations) {
    runTimingTest(iterations);
}

#ifdef TGT_OS_TYPE_LINUX
int main(int argc, char* argv[]) {
    runTest(1000000);
}

#endif

