// ======================================================================
// \title Os/Stub/test/ut/StubMemoryTests.cpp
// \brief tests using stub implementation for Os::Memory interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Memory.hpp"
#include "Os/Stub/test/Memory.hpp"
#include "STest/Pick/Pick.hpp"

TEST(Interface, Construction) {
    Os::Memory memory;
    ASSERT_EQ(Os::Stub::Memory::Test::StaticData::data.lastCalled, Os::Stub::Memory::Test::StaticData::CONSTRUCT_FN);
}

TEST(Interface, Destruction) {
    delete (new Os::Memory);
    ASSERT_EQ(Os::Stub::Memory::Test::StaticData::data.lastCalled, Os::Stub::Memory::Test::StaticData::DESTRUCT_FN);
}

TEST(Interface, Usage) {
    Os::Memory memory;
    Os::Memory::Usage usage;
    usage.used = STest::Pick::lowerUpper(0, 10000);
    usage.total = STest::Pick::lowerUpper(0, 10000);
    Os::Memory::Usage usage_copy = usage;
    Os::MemoryInterface::Status status = Os::MemoryInterface::Status::ERROR;
    Os::Stub::Memory::Test::StaticData::data.status_out = status;
    ASSERT_EQ(Os::Stub::Memory::Test::StaticData::data.lastCalled, Os::Stub::Memory::Test::StaticData::CONSTRUCT_FN);
    ASSERT_EQ(Os::Memory::getUsage(usage), status);
    ASSERT_EQ(Os::Stub::Memory::Test::StaticData::data.lastCalled, Os::Stub::Memory::Test::StaticData::USAGE_FN);
    ASSERT_EQ(Os::Stub::Memory::Test::StaticData::data.usage.total, usage.total);
    ASSERT_EQ(Os::Stub::Memory::Test::StaticData::data.usage.used, usage.used);
    ASSERT_EQ(usage_copy.total, usage.total);
    ASSERT_EQ(usage_copy.used, usage.used);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
