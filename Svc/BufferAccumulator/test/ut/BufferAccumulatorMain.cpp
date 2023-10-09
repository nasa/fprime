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
#include "BufferAccumulatorTester.hpp"

// ----------------------------------------------------------------------
// Test Errors
// ----------------------------------------------------------------------

TEST(TestErrors, QueueFull) {
  Svc::Errors::BufferAccumulatorTester tester;
  tester.QueueFull();
}

TEST(TestErrors, PartialDrain) {
  Svc::Errors::BufferAccumulatorTester tester;
  tester.PartialDrain();
}

// ----------------------------------------------------------------------
// Test Accumulate
// ----------------------------------------------------------------------

TEST(TestAccumulate, OK) {
  Svc::Accumulate::BufferAccumulatorTester tester;
  tester.OK();
}

// ----------------------------------------------------------------------
// Test Drain
// ----------------------------------------------------------------------

TEST(TestDrain, OK) {
  Svc::Drain::BufferAccumulatorTester tester;
  tester.OK();
}

TEST(TestPartialDrain, OK) {
  Svc::Drain::BufferAccumulatorTester tester;
  tester.PartialDrainOK();
}

// ----------------------------------------------------------------------
// Test Health
// ----------------------------------------------------------------------

TEST(TestHealth, Ping) {
  Svc::Health::BufferAccumulatorTester tester;
  tester.Ping();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
