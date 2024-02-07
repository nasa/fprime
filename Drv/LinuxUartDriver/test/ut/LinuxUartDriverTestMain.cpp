// ======================================================================
// \title  LinuxUartDriverTestMain.cpp
// \author tcanham
// \brief  cpp file for LinuxUartDriver component test main function
// ======================================================================

#include "LinuxUartDriverTester.hpp"

/**
 * Put a placeholder test here to pass CI. Tests are run manually on hardware.
*/
TEST(Nominal, SkipTest) {
    GTEST_SKIP() << "Linux UART tests are run manually.";
}

int main(int argc, char** argv) {

    if (1 == argc) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    } else {

    }

}
