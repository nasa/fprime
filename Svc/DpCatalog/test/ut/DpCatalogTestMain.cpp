// ======================================================================
// \title  DpCatalogTestMain.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test main function
// ======================================================================

#include "DpCatalogTester.hpp"

TEST(Nominal, initTest) {
    Svc::DpCatalogTester tester;
    tester.doInit();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
