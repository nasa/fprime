// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(FileDownlink, Downlink) {
  Svc::Tester tester;
  tester.downlink();
}

TEST(FileDownlink, FileOpenError) {
  Svc::Tester tester;
  tester.fileOpenError();
}

TEST(FileDownlink, CancelDownlink) {
  Svc::Tester tester;
  tester.cancelDownlink();
}

TEST(FileDownlink, CancelInIdleMode) {
  Svc::Tester tester;
  tester.cancelInIdleMode();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
