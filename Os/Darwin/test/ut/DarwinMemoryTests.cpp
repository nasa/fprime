// ======================================================================
// \title Os/Stub/test/ut/StubMemoryTests.cpp
// \brief tests using stub implementation for Os::Memory interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Memory.hpp"
#include "Os/Darwin/Memory.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
