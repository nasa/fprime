/*
 * PolyDbTester.cpp
 *
 *  Created on: Sept 14, 2015
 *      Author: tcanham
 */

#include <Svc/PolyDb/test/ut/PolyDbImplTester.hpp>
#include <Svc/PolyDb/PolyDbImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>

void connectPorts(Svc::PolyDbImpl& impl, Svc::PolyDbImplTester& tester) {

    //Fw::SimpleObjRegistry simpleReg;

    // command ports
    tester.set_getValue_OutputPort(0,impl.get_getValue_InputPort(0));
    tester.set_setValue_OutputPort(0,impl.get_setValue_InputPort(0));

#if FW_PORT_TRACING
    //Fw::PortBase::setTrace(true);
#endif

    //simpleReg.dump();

}

TEST(CmdDispTestNominal,NominalReadWrite) {

    TEST_CASE(104.1.1, "PolyDb Nominal Read/Write Test");

    COMMENT(
            "Read and write values to the database while varying"
            "the measurement statuses."
            );

    Svc::PolyDbImpl impl("PolyDbImpl");

    impl.init(0);

    Svc::PolyDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    tester.runNominalReadWrite();

}


#ifndef TGT_OS_TYPE_VXWORKS
int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif

