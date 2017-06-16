// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <Fw/Test/UnitTest.hpp>

TEST(Nominal, InitTest) {
    TEST_CASE(103.1.1,"Nominal Initialization");
    Svc::Tester tester;
    tester.initializeTest();
}

TEST(OffNominal, MissingFile) {
    TEST_CASE(103.2.1,"Off-Nominal Missing File");
    Svc::Tester tester;
    tester.missingFile();
}

TEST(OffNominal, BadFileCrc) {
    TEST_CASE(103.2.2,"Off-Nominal Bad File CRC");
    Svc::Tester tester;
    tester.badFileCrc();
}

TEST(OffNominal, EmptySequenceFile) {
    TEST_CASE(103.2.3,"Off-Nominal Empty Sequence File");
    Svc::Tester tester;
    tester.emptyFile();
}

TEST(OffNominal, InvalidRecord) {
    TEST_CASE(103.2.4,"Off-Nominal Invalid Records");
    Svc::Tester tester;
    tester.invalidRecord();
}

TEST(OffNominal, FileTooLarge) {
    TEST_CASE(103.2.5,"Off-Nominal File Too Large");
    Svc::Tester tester;
    tester.fileTooLarge();
}

TEST(Nominal, NominalImmediate) {
    TEST_CASE(103.1.2,"Nominal Immediate Commands");
    Svc::Tester tester;
    tester.nominalImmediate();
}

TEST(Nominal, NominalRelative) {
    TEST_CASE(103.1.3,"Nominal Relative Commands");
    Svc::Tester tester;
    tester.nominalRelative();
}

TEST(Nominal, NominalTimedRelative) {
    TEST_CASE(103.1.4,"Nominal Timed Relative Commands");
    Svc::Tester tester;
    tester.nominalTimedRelative();
}

TEST(Nominal, NominalImmediatePort) {
    TEST_CASE(103.1.5,"Nominal Immediate Port Sequence");
    Svc::Tester tester;
    tester.nominalImmediatePort();
}

TEST(Nominal, CancelCommand) {
    TEST_CASE(103.1.6,"Nominal Sequence Cancel");
    Svc::Tester tester;
    tester.cancelCommand();
}

TEST(Nominal, ManualImmediate) {
    TEST_CASE(103.1.7,"Nominal Manual Sequence Stepping");
    Svc::Tester tester;
    tester.manualImmediate();
}

TEST(Nominal, ManualImmediate2) {
    TEST_CASE(103.1.8,"Nominal Manual Sequence Stepping - No end of sequence marker");
    Svc::Tester tester;
    tester.manualImmediate2();
}

TEST(OffNominal, FailedCommand) {
    TEST_CASE(103.2.6,"Off-Nominal failed command");
    Svc::Tester tester;
    tester.failedCommand();
}

TEST(Nominal, PingTest) {
    TEST_CASE(103.1.9,"Nominal ping test");
    Svc::Tester tester;
    tester.pingTest();
}

TEST(OffNominal, InvalidMode) {
    TEST_CASE(103.2.7,"Off-Nominal invalid modes");
    Svc::Tester tester;
    tester.invalidMode();
}

TEST(OffNominal, NoSequenceActive) {
    TEST_CASE(103.2.8,"Off-Nominal no active sequence");
    Svc::Tester tester;
    tester.noSequence();
}

TEST(OffNominal, InvalidRecordEntries) {
    TEST_CASE(103.2.9,"Invalid Record entries");
    Svc::Tester tester;
    tester.invalidRecordEntries();
}

TEST(OffNominal, InvalidSequenceTime) {
    TEST_CASE(103.2.10,"Invalid sequence time");
    Svc::Tester tester;
    tester.invalidSequenceTime();
}

TEST(OffNominal, UnexpectedCompletion) {
    TEST_CASE(103.2.11,"Unexpected completion after completed sequence");
    Svc::Tester tester;
    tester.unexpectedCompletion();
}

TEST(OffNominal, SequenceTimeout) {
    TEST_CASE(103.2.12,"Sequence timeout");
    Svc::Tester tester;
    tester.sequenceTimeout();
}

TEST(OffNominal, InvalidManualModes) {
    TEST_CASE(103.2.13,"Invalid Manual Modes");
    Svc::Tester tester;
    tester.invalidManualModes();
}

TEST(OffNominal, FileLoadErrors) {
    TEST_CASE(103.2.14,"File Load errors");
    Svc::Tester tester;
    tester.fileLoadErrors();
}

TEST(OffNominal, ExtraData) {
    TEST_CASE(103.2.15,"Extra Data after records");
    Svc::Tester tester;
    tester.dataAfterRecords();
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
