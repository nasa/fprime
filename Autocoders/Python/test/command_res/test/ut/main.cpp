// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Test, CmdResidual) {
  Cmd::Tester tester;
  tester.residualTest();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
