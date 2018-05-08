// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include "Errors.hpp"
#include "Accumulate.hpp"
#include "Drain.hpp"
#include "Health.hpp"

TEST(Test, AccumNoAllocate) {
  Svc::Tester tester(false); // don't call allocateQueue for the user
  tester.AccumNoAllocate();
}

// ----------------------------------------------------------------------
// Test Errors
// ----------------------------------------------------------------------

TEST(TestErrors, QueueFull) {
  Svc::Errors::Tester tester;
  tester.QueueFull();
}

// ----------------------------------------------------------------------
// Test Accumulate
// ----------------------------------------------------------------------

TEST(TestAccumulate, OK) {
  Svc::Accumulate::Tester tester;
  tester.OK();
}

// ----------------------------------------------------------------------
// Test Drain
// ----------------------------------------------------------------------

TEST(TestDrain, OK) {
  Svc::Drain::Tester tester;
  tester.OK();
}

// ----------------------------------------------------------------------
// Test Health
// ----------------------------------------------------------------------

TEST(TestHealth, Ping) {
  Svc::Health::Tester tester;
  tester.Ping();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
