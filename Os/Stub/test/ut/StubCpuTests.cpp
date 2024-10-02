// ======================================================================
// \title Os/Stub/test/ut/StubCpuTests.cpp
// \brief tests using stub implementation for Os::Cpu interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Cpu.hpp"
#include "Os/Stub/test/Cpu.hpp"
#include "STest/Pick/Pick.hpp"

TEST(Interface, Construction) {
    Os::Cpu cpu;
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.lastCalled, Os::Stub::Cpu::Test::StaticData::CONSTRUCT_FN);
}

TEST(Interface, Destruction) {
    delete (new Os::Cpu);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.lastCalled, Os::Stub::Cpu::Test::StaticData::DESTRUCT_FN);
}

TEST(Interface, Count) {
    Os::Cpu cpu;
    FwSizeType count = STest::Pick::lowerUpper(0, 10000);
    FwSizeType count_copy = count;
    Os::CpuInterface::Status status = Os::CpuInterface::Status::ERROR;
    Os::Stub::Cpu::Test::StaticData::data.status_out = status;
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.lastCalled, Os::Stub::Cpu::Test::StaticData::CONSTRUCT_FN);
    ASSERT_EQ(cpu.getCount(count), status);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.lastCalled, Os::Stub::Cpu::Test::StaticData::COUNT_FN);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.count, count_copy);
    ASSERT_EQ(count, count_copy);
}

TEST(Interface, Ticks) {
    Os::Cpu cpu;
    FwSizeType index = STest::Pick::lowerUpper(0, 10000);
    Os::Cpu::Ticks ticks;
    ticks.used = STest::Pick::lowerUpper(0, 10000);
    ticks.total = STest::Pick::lowerUpper(0, 10000);
    Os::Cpu::Ticks ticks_copy = ticks;
    Os::CpuInterface::Status status = Os::CpuInterface::Status::ERROR;
    Os::Stub::Cpu::Test::StaticData::data.status_out = status;
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.lastCalled, Os::Stub::Cpu::Test::StaticData::CONSTRUCT_FN);
    ASSERT_EQ(cpu.getTicks(ticks, index), status);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.lastCalled, Os::Stub::Cpu::Test::StaticData::TICKS_FN);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.index, index);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.ticks.total, ticks.total);
    ASSERT_EQ(Os::Stub::Cpu::Test::StaticData::data.ticks.used, ticks.used);
    ASSERT_EQ(ticks_copy.total, ticks.total);
    ASSERT_EQ(ticks_copy.used, ticks.used);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
