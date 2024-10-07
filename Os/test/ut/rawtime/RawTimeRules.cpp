// ======================================================================
// \title Os/test/ut/rawtime/RawTimeRules.cpp
// \brief rule implementations for common testing of RawTime
// ======================================================================

#include "RulesHeaders.hpp"
#include "RawTimeRules.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Buffer/Buffer.hpp"


// ------------------------------------------------------------------------------------------------------
// Rule SelfDiffIsZero
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::SelfDiffIsZero::SelfDiffIsZero() :
    STest::Rule<Os::Test::RawTime::Tester>("SelfDiffIsZero") {}

bool Os::Test::RawTime::Tester::SelfDiffIsZero::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::SelfDiffIsZero::action(Os::Test::RawTime::Tester &state) {
    U32 result;
    FwIndexType index = state.pick_random_index();
    Os::RawTime& raw_time = state.m_times[index];
    Os::RawTime::Status status = raw_time.getDiffUsec(raw_time, result);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);
    ASSERT_EQ(result, 0);
}

// ------------------------------------------------------------------------------------------------------
// Rule GetRawTime
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::GetRawTime::GetRawTime() :
    STest::Rule<Os::Test::RawTime::Tester>("GetRawTime") {}

bool Os::Test::RawTime::Tester::GetRawTime::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::GetRawTime::action(Os::Test::RawTime::Tester &state) {
    FwIndexType index = state.pick_random_index();

    Os::RawTime::Status status = state.m_times[index].getRawTime();
    state.shadow_getRawTime(index);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule GetTimeDiffU32
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::GetTimeDiffU32::GetTimeDiffU32() :
    STest::Rule<Os::Test::RawTime::Tester>("GetTimeDiffU32") {}

bool Os::Test::RawTime::Tester::GetTimeDiffU32::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::GetTimeDiffU32::action(Os::Test::RawTime::Tester &state) {
    U32 result;
    FwIndexType index1 = state.pick_random_index();
    FwIndexType index2 = state.pick_random_index();

    Os::RawTime::Status status = state.m_times[index1].getDiffUsec(state.m_times[index2], result);
    U32 shadow_result = state.shadow_getDiffUsec(state.m_shadow_times[index1], state.m_shadow_times[index2]);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);
    I64 result_diff = static_cast<I64>(result) - static_cast<I64>(shadow_result);
    ASSERT_LT(result_diff, 10);
}

// ------------------------------------------------------------------------------------------------------
// Rule GetTimeInterval
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::GetTimeInterval::GetTimeInterval() :
    STest::Rule<Os::Test::RawTime::Tester>("GetTimeInterval") {}

bool Os::Test::RawTime::Tester::GetTimeInterval::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::GetTimeInterval::action(Os::Test::RawTime::Tester &state) {
    Fw::TimeInterval interval;
    FwIndexType index1 = state.pick_random_index();
    FwIndexType index2 = state.pick_random_index();

    Os::RawTime::Status status = state.m_times[index1].getTimeInterval(state.m_times[index2], interval);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);

    ASSERT_TRUE(state.shadow_validate_interval_result(index1, index2, interval));
    // Fw::TimeInterval result = Fw::TimeInterval::sub(interval, shadow_interval); // find another solution here
    // ASSERT_TRUE(result <= Fw::TimeInterval(0, 50));
    // ASSERT_LT(result_diff, 10);
}

// ------------------------------------------------------------------------------------------------------
// Rule Serialization
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::Serialization::Serialization() :
    STest::Rule<Os::Test::RawTime::Tester>("Serialization") {}

bool Os::Test::RawTime::Tester::Serialization::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::Serialization::action(Os::Test::RawTime::Tester &state) {
    U8 data[100] = {0}; // TODO: use OS_RAWTIME_SERIALIZED_SIZE instead, somehow
    Fw::Buffer buffer(data, sizeof(data));

    FwIndexType index = state.pick_random_index();

    state.m_times[index].serialize(buffer.getSerializeRepr());

    Os::RawTime raw_time;
    raw_time.deserialize(buffer.getSerializeRepr());

    U32 result = 1;
    Os::RawTime::Status status = state.m_times[index].getDiffUsec(raw_time, result);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);
    ASSERT_EQ(result, 0);
}
