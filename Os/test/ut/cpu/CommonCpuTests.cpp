// ======================================================================
// \title Os/Stub/test/ut/StubCpuTests.cpp
// \brief tests using stub implementation for Os::Cpu interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Cpu.hpp"
#include "Os/Darwin/Cpu.hpp"

TEST(Basic, Count) {
    FwSizeType count = 0;
    ASSERT_EQ(Os::Cpu::getCount(count), Os::Cpu::Status::OP_OK);
    ASSERT_GT(count, 0);
}

TEST(Basic, Ticks) {
    // Get the count
    FwSizeType count = 0;
    Os::Cpu::getCount(count);
    ASSERT_GT(count, 0);

    Os::Cpu::Ticks ticks_output;
    for (FwSizeType i = 0; i < count; i++) {
        ASSERT_EQ(Os::Cpu::getTicks(ticks_output, i), Os::Cpu::Status::OP_OK);
        ASSERT_GT(ticks_output.used, 0);
        ASSERT_GE(ticks_output.total, 0) << "No total on CPU: " << i;
    }
}
