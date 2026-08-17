// ======================================================================
// \title Os/test/ut/memory/CommonMemoryTests.cpp
// \brief common tests for the Os::Memory interface
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Darwin/Memory.hpp"
#include "Os/Memory.hpp"

TEST(Basic, Usage) {
    Os::Memory::Usage usage_output;
    Os::Memory::getUsage(usage_output);

    ASSERT_GT(usage_output.used, 0);
    ASSERT_GT(usage_output.total, 0);
}
