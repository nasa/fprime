// ======================================================================
// \title  PolyDbTestMain.cpp
// \author tcanham
// \brief  cpp file for PolyDb component test main function
// ======================================================================

#include "PolyDbTester.hpp"
#include "Fw/Test/UnitTest.hpp"

TEST(CmdDispTestNominal,NominalReadWrite) {

    TEST_CASE(104.1.1, "PolyDb Nominal Read/Write Test");

    COMMENT(
            "Read and write values to the database while varying"
            "the measurement statuses."
            );

    Svc::PolyDbTester tester;

    tester.runNominalReadWrite();

}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
