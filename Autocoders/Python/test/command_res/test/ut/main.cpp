// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "command_resTester.hpp"

TEST(Test, CmdResidual) {
  Cmd::command_resTester tester;
  tester.residualTest();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
