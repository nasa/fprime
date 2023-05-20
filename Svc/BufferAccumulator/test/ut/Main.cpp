// ======================================================================
// \title  Main.cpp
// \author bocchino, mereweth
// \brief  Test drain mode
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Accumulate.hpp"
#include "Drain.hpp"
#include "Errors.hpp"
#include "Health.hpp"
#include "Tester.hpp"

// ----------------------------------------------------------------------
// Test Errors
// ----------------------------------------------------------------------

TEST(TestErrors, QueueFull) {
  Svc::Errors::Tester tester;
  tester.QueueFull();
}

TEST(TestErrors, PartialDrain) {
  Svc::Errors::Tester tester;
  tester.PartialDrain();
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

TEST(TestPartialDrain, OK) {
  Svc::Drain::Tester tester;
  tester.PartialDrainOK();
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
