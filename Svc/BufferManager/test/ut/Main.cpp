// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Test, allocateAndFreeOne) {
  Svc::Tester tester;
  tester.allocateAndFreeOne();
}

TEST(Test, allocationQueueEmpty) {
  Svc::Tester tester;
  tester.allocationQueueEmpty();
}

TEST(Test, allocationQueueFull) {
  Svc::Tester tester;
  tester.allocationQueueFull();
}

TEST(Test, idMismatch) {
  Svc::Tester tester;
  tester.idMismatch();
}

TEST(Test, storeSizeExceeded) {
  Svc::Tester tester;
  tester.storeSizeExceeded();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
