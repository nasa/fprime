/*
 * PrmDbTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/PrmDb/test/ut/PrmDbImplTester.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>

#include <gtest/gtest.h>

#include <Fw/Test/UnitTest.hpp>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::PrmDbImpl& impl, Svc::PrmDbImplTester& tester) {

    // command ports
    tester.connect_to_CmdDisp(0,impl.get_CmdDisp_InputPort(0));
    impl.set_CmdStatus_OutputPort(0,tester.get_from_CmdStatus(0));

    // telemetry ports
    impl.set_Time_OutputPort(0,tester.get_from_Time(0));
    impl.set_Log_OutputPort(0,tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0,tester.get_from_LogText(0));

    // parameter ports
    tester.connect_to_getPrm(0,impl.get_getPrm_InputPort(0));
    tester.connect_to_setPrm(0,impl.get_setPrm_InputPort(0));

#if FW_PORT_TRACING
    //Fw::PortBase::setTrace(true);
#endif

    //simpleReg.dump();
}

TEST(ParameterDbTest,NominalPopulateTest) {

    TEST_CASE(105.1.1,"Nominal populate test");
    COMMENT("Write values to the parameter database and verify that they were written correctly");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10,0);
    impl.configure("TestFile.prm");
    Svc::PrmDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run nominal tests
    tester.runNominalPopulate();

}

TEST(ParameterDbTest,NominalFileSaveTest) {

    TEST_CASE(105.1.2,"Nominal file save test");
    COMMENT("Write values to the parameter database and save them to a file.");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10,0);
    impl.configure("TestFile.prm");

    Svc::PrmDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run nominal save file tests
    tester.runNominalSaveFile();

}

TEST(ParameterDbTest,NominalFileLoadTest) {

    TEST_CASE(105.1.3,"Nominal file load test");
    COMMENT("Read values from the created file and verify they are correct.");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10,0);
    impl.configure("TestFile.prm");

    Svc::PrmDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run nominal load file tests
    tester.runNominalLoadFile();

}

//TEST(ParameterDbTest,RefPrmFile) {
//
//    Svc::PrmDbImpl impl("PrmDbImpl");
//
//    impl.init(10);
//
//    Svc::PrmDbImplTester tester(impl);
//
//    tester.init();
//
//    // connect ports
//    connectPorts(impl,tester);
//
//    // run test to generate parameter file for reference example
//    tester.runRefPrmFile();
//
//}

TEST(ParameterDbTest,PrmMissingExtraParamsTest) {

    TEST_CASE(105.2.1,"Missing and too many parameters test");
    COMMENT("Attempt to read a nonexistent parameter and write too many parameters");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10,0);
    impl.configure("TestFile.prm");

    Svc::PrmDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test with file errors
    tester.runMissingExtraParams();

}

TEST(ParameterDbTest,PrmFileReadError) {

    TEST_CASE(105.2.2,"File read errors");
    COMMENT("Induce errors at various stages of reading the file");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10,0);
    impl.configure("TestFile.prm");

    Svc::PrmDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test with file errors
    tester.runFileReadError();

}

TEST(ParameterDbTest,PrmFileWriteError) {

    TEST_CASE(105.2.3,"File write errors");
    COMMENT("Induce errors at various stages of writing the file");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10,0);
    impl.configure("TestFile.prm");

    Svc::PrmDbImplTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl,tester);

    // run test with file errors
    tester.runFileWriteError();

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



