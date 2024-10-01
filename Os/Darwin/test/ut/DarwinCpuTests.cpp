// ======================================================================
// \title Os/Stub/test/ut/StubCpuTests.cpp
// \brief tests using stub implementation for Os::Cpu interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Cpu.hpp"
#include "Os/Darwin/Cpu.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
