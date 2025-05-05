// ======================================================================
// \title Os/Stub/test/ut/StubMemoryTests.cpp
// \brief tests using stub implementation for Os::Memory interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Memory.hpp"
#include "Os/Darwin/Memory.hpp"

TEST(Basic, Usage) {
    Os::Memory::Usage usage_output;
    Os::Memory::getUsage(usage_output);

    ASSERT_GT(usage_output.used, 0);
    ASSERT_GT(usage_output.total, 0);
}
