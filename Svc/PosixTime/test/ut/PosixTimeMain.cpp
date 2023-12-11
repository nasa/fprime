// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "PosixTimeTester.hpp"

TEST(Test, GetTime) {
  Svc::PosixTimeTester tester("Tester");
  tester.getTime();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
