// ======================================================================
// \title  TimeConverterTestMain.cpp
// \author devin
// \brief  cpp file for TimeConverter component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "TimeConverterTester.hpp"

TEST(Nominal, SameTimeBase) {
    COMMENT("Conversion within a single time base copies the time");
    REQUIREMENT("REQ-TIMECONVERTER-007");
    Svc::TimeConverterTester tester;
    tester.sameTimeBaseTest();
}

TEST(Nominal, ConvertBothDirections) {
    COMMENT("One stored pair serves both conversion directions");
    REQUIREMENT("REQ-TIMECONVERTER-005");
    Svc::TimeConverterTester tester;
    tester.convertBothDirectionsTest();
}

TEST(Nominal, ContextPreserved) {
    COMMENT("Conversion preserves the time context of the input");
    REQUIREMENT("REQ-TIMECONVERTER-006");
    Svc::TimeConverterTester tester;
    tester.contextPreservedTest();
}

TEST(Nominal, PortUpdate) {
    COMMENT("Offsets supplied over the port are stored and replaced");
    REQUIREMENT("REQ-TIMECONVERTER-002");
    Svc::TimeConverterTester tester;
    tester.portUpdateTest();
}

TEST(Nominal, ReplaceOffset) {
    COMMENT("A pair supplied in either order occupies one entry");
    REQUIREMENT("REQ-TIMECONVERTER-001");
    Svc::TimeConverterTester tester;
    tester.replaceOffsetTest();
}

TEST(Nominal, ClearOffsets) {
    COMMENT("CLEAR_OFFSETS discards every stored offset");
    REQUIREMENT("REQ-TIMECONVERTER-011");
    Svc::TimeConverterTester tester;
    tester.clearOffsetsTest();
}

TEST(Nominal, GetOffset) {
    COMMENT("GET_OFFSET reports a stored pair in both directions");
    REQUIREMENT("REQ-TIMECONVERTER-012");
    Svc::TimeConverterTester tester;
    tester.getOffsetTest();
}

TEST(Nominal, DumpOffsets) {
    COMMENT("DUMP_OFFSETS reports every stored offset");
    REQUIREMENT("REQ-TIMECONVERTER-013");
    Svc::TimeConverterTester tester;
    tester.dumpOffsetsTest();
}

TEST(Nominal, FillTable) {
    COMMENT("The table holds an offset for every pair up to its configured capacity");
    REQUIREMENT("REQ-TIMECONVERTER-003");
    Svc::TimeConverterTester tester;
    tester.fillTableTest();
}

TEST(Nominal, OffsetBoundaries) {
    COMMENT("Offsets at the limits of the representable range are accepted");
    REQUIREMENT("REQ-TIMECONVERTER-009");
    Svc::TimeConverterTester tester;
    tester.offsetBoundariesTest();
}

TEST(Nominal, ConversionRangeEnds) {
    COMMENT("A conversion landing on either end of the representable range succeeds");
    REQUIREMENT("REQ-TIMECONVERTER-009");
    Svc::TimeConverterTester tester;
    tester.conversionRangeEndsTest();
}

TEST(OffNominal, TableFull) {
    COMMENT("An offset beyond the table capacity is rejected");
    REQUIREMENT("REQ-TIMECONVERTER-004");
    Svc::TimeConverterTester tester;
    tester.tableFullTest();
}

TEST(OffNominal, PortUpdateRejected) {
    COMMENT("Offsets rejected on the port are reported by event and not stored");
    REQUIREMENT("REQ-TIMECONVERTER-010");
    Svc::TimeConverterTester tester;
    tester.portUpdateRejectedTest();
}

TEST(OffNominal, PortUpdateTableFull) {
    COMMENT("An offset supplied on the port beyond the table capacity is rejected");
    REQUIREMENT("REQ-TIMECONVERTER-004");
    Svc::TimeConverterTester tester;
    tester.portUpdateTableFullTest();
}

TEST(OffNominal, ThrottleReset) {
    COMMENT("Every throttled warning resumes after the offsets are cleared");
    REQUIREMENT("REQ-TIMECONVERTER-008");
    Svc::TimeConverterTester tester;
    tester.throttleResetTest();
}

TEST(OffNominal, UnknownTimeBase) {
    COMMENT("Conversion of an unknown pair leaves the output unchanged");
    REQUIREMENT("REQ-TIMECONVERTER-008");
    Svc::TimeConverterTester tester;
    tester.unknownTimeBaseTest();
}

TEST(OffNominal, InvalidTime) {
    COMMENT("Conversion outside the representable range leaves the output unchanged");
    REQUIREMENT("REQ-TIMECONVERTER-009");
    Svc::TimeConverterTester tester;
    tester.invalidTimeTest();
}

TEST(OffNominal, SetOffsetIdentical) {
    COMMENT("A single time base is rejected as a pair");
    REQUIREMENT("REQ-TIMECONVERTER-010");
    Svc::TimeConverterTester tester;
    tester.setOffsetIdenticalTest();
}

TEST(OffNominal, UnusableTimeBase) {
    COMMENT("A time base denoting no clock is rejected wherever it is supplied");
    REQUIREMENT("REQ-TIMECONVERTER-015");
    Svc::TimeConverterTester tester;
    tester.unusableTimeBaseTest();
}

TEST(OffNominal, SetOffsetOutOfRange) {
    COMMENT("An offset larger than the representable time range is rejected");
    REQUIREMENT("REQ-TIMECONVERTER-009");
    Svc::TimeConverterTester tester;
    tester.setOffsetOutOfRangeTest();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
