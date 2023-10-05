// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "LinuxTimeTester.hpp"

TEST(Test, GetTime) {
  Svc::LinuxTimeTester tester("Tester");
  tester.getTime();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
