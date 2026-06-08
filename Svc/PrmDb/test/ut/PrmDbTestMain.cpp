/*
 * PrmDbTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Svc/PrmDb/test/ut/PrmDbTester.hpp>

#include <gtest/gtest.h>

#include <Fw/Test/UnitTest.hpp>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::PrmDbImpl& impl, Svc::PrmDbTester& tester) {
    // command ports
    tester.connect_to_CmdDisp(0, impl.get_CmdDisp_InputPort(0));
    impl.set_CmdStatus_OutputPort(0, tester.get_from_CmdStatus(0));

    // telemetry ports
    impl.set_Time_OutputPort(0, tester.get_from_Time(0));
    impl.set_Log_OutputPort(0, tester.get_from_Log(0));
    impl.set_LogText_OutputPort(0, tester.get_from_LogText(0));

    // parameter ports
    tester.connect_to_getPrm(0, impl.get_getPrm_InputPort(0));
    tester.connect_to_setPrm(0, impl.get_setPrm_InputPort(0));

#if FW_PORT_TRACING
    // Fw::PortBase::setTrace(true);
#endif

    // simpleReg.dump();
}

TEST(ParameterDbTest, NominalPopulateTest) {
    TEST_CASE(105.1.1, "Nominal populate test");
    COMMENT("Write values to the parameter database and verify that they were written correctly");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");
    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // run nominal tests
    tester.runNominalPopulate();
}

TEST(ParameterDbTest, NominalFileSaveTest) {
    TEST_CASE(105.1.2, "Nominal file save test");
    COMMENT("Write values to the parameter database and save them to a file.");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // run nominal save file tests
    tester.runNominalSaveFile();
}

TEST(ParameterDbTest, NominalFileLoadTest) {
    TEST_CASE(105.1.3, "Nominal file load test");
    COMMENT("Read values from the created file and verify they are correct.");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // run nominal load file tests
    tester.runNominalLoadFile();
}

// TEST(ParameterDbTest,RefPrmFile) {
//
//     Svc::PrmDbImpl impl("PrmDbImpl");
//
//     impl.init(10);
//
//     Svc::PrmDbTester tester(impl);
//
//     tester.init();
//
//     // connect ports
//     connectPorts(impl,tester);
//
//     // run test to generate parameter file for reference example
//     tester.runRefPrmFile();
//
// }

TEST(ParameterDbTest, PrmMissingExtraParamsTest) {
    TEST_CASE(105.2.1, "Missing and too many parameters test");
    COMMENT("Attempt to read a nonexistent parameter and write too many parameters");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // run test with file errors
    tester.runMissingExtraParams();
}

TEST(ParameterDbTest, PrmFileReadError) {
    TEST_CASE(105.2.2, "File read errors");
    COMMENT("Induce errors at various stages of reading the file");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // run test with file errors
    tester.runFileReadError();
}

TEST(ParameterDbTest, PrmFileWriteError) {
    TEST_CASE(105.2.3, "File write errors");
    COMMENT("Induce errors at various stages of writing the file");

    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    // run test with file errors
    tester.runFileWriteError();
}

TEST(ParameterDbTest, PrmDbEqualTest) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runDbEqualTest();
}

TEST(ParameterDbTest, PrmDbCopyTest) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runDbCopyTest();
}

TEST(ParameterDbTest, PrmDbCommitTest) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runDbCommitTest();
}

TEST(ParameterDbTest, PrmDbFileLoadNominal) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runPrmFileLoadNominal();
}

TEST(ParameterDbTest, PrmDbFileLoadWithErrors) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runPrmFileLoadWithErrors();
}

TEST(ParameterDbTest, PrmFileLoadIllegalActions) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runPrmFileLoadIllegal();
}

TEST(ParameterDbTest, PrmShorterSaveDoesNotCorrupt) {
    Svc::PrmDbImpl impl("PrmDbImpl");

    impl.init(10, 0);
    impl.configure("TestFile.prm");

    Svc::PrmDbTester tester(impl);

    tester.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runShorterSaveDoesNotCorrupt();
}

// ======================================================================
// Hardcoded CRC32 value tests
//
// PrmDb uses raw CRC32 accumulation (init 0xFFFFFFFF, no final ones'
// complement). These tests call PrmDbImpl::computeCrc (via friend access)
// and verify results against hardcoded expected values.
// ======================================================================

TEST(ParameterDbCrcTest, HardcodedValue_123456789) {
    Svc::PrmDbImpl impl("PrmDbImpl");
    impl.init(10, 0);
    impl.configure("TestFile.prm");
    Svc::PrmDbTester tester(impl);
    tester.init();
    connectPorts(impl, tester);

    const BYTE data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    U32 crc = tester.testComputeCrc(0xFFFFFFFF, data, sizeof(data));
    ASSERT_EQ(crc, static_cast<U32>(0x340BC6D9))
        << "PrmDb CRC of \"123456789\" must equal 0x340BC6D9 (raw accumulator)";
}

TEST(ParameterDbCrcTest, HardcodedValue_DEADBEEF) {
    Svc::PrmDbImpl impl("PrmDbImpl");
    impl.init(10, 0);
    impl.configure("TestFile.prm");
    Svc::PrmDbTester tester(impl);
    tester.init();
    connectPorts(impl, tester);

    const BYTE data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    U32 crc = tester.testComputeCrc(0xFFFFFFFF, data, sizeof(data));
    ASSERT_EQ(crc, static_cast<U32>(0x83635CA5)) << "PrmDb CRC of {0xDE,0xAD,0xBE,0xEF} must equal 0x83635CA5";
}

TEST(ParameterDbCrcTest, HardcodedValue_SingleByte) {
    Svc::PrmDbImpl impl("PrmDbImpl");
    impl.init(10, 0);
    impl.configure("TestFile.prm");
    Svc::PrmDbTester tester(impl);
    tester.init();
    connectPorts(impl, tester);

    const BYTE data[] = {0x00};
    U32 crc = tester.testComputeCrc(0xFFFFFFFF, data, sizeof(data));
    ASSERT_EQ(crc, static_cast<U32>(0x2DFD1072)) << "PrmDb CRC of {0x00} must equal 0x2DFD1072";
}

TEST(ParameterDbCrcTest, HardcodedValue_PrmRecord) {
    Svc::PrmDbImpl impl("PrmDbImpl");
    impl.init(10, 0);
    impl.configure("TestFile.prm");
    Svc::PrmDbTester tester(impl);
    tester.init();
    connectPorts(impl, tester);

    // Simulates PrmDb CRC over a single parameter record:
    // delimiter(0xA5) + recordSize(big-endian U32=8) + id(big-endian U32=100) + value(big-endian U32=42)
    U32 crc = 0xFFFFFFFF;
    const BYTE delim = 0xA5;
    crc = tester.testComputeCrc(crc, &delim, sizeof(delim));
    const BYTE recSize[] = {0x00, 0x00, 0x00, 0x08};
    crc = tester.testComputeCrc(crc, recSize, sizeof(recSize));
    const BYTE id[] = {0x00, 0x00, 0x00, 0x64};
    crc = tester.testComputeCrc(crc, id, sizeof(id));
    const BYTE val[] = {0x00, 0x00, 0x00, 0x2A};
    crc = tester.testComputeCrc(crc, val, sizeof(val));

    ASSERT_EQ(crc, static_cast<U32>(0xF261DECF)) << "PrmDb CRC of param record (id=100, val=42) must equal 0xF261DECF";
}

TEST(ParameterDbCrcTest, HardcodedValue_Incremental) {
    Svc::PrmDbImpl impl("PrmDbImpl");
    impl.init(10, 0);
    impl.configure("TestFile.prm");
    Svc::PrmDbTester tester(impl);
    tester.init();
    connectPorts(impl, tester);

    const BYTE data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    U32 crc = 0xFFFFFFFF;
    crc = tester.testComputeCrc(crc, data, 4);
    crc = tester.testComputeCrc(crc, data + 4, 5);
    ASSERT_EQ(crc, static_cast<U32>(0x340BC6D9)) << "Incremental PrmDb CRC of \"123456789\" must equal 0x340BC6D9";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
