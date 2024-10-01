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
// Rule GetTimeDiff
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::GetTimeDiff::GetTimeDiff() :
    STest::Rule<Os::Test::RawTime::Tester>("GetTimeDiff") {}

bool Os::Test::RawTime::Tester::GetTimeDiff::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::GetTimeDiff::action(Os::Test::RawTime::Tester &state) {
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
// Rule Serialization
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::Serialization::Serialization() :
    STest::Rule<Os::Test::RawTime::Tester>("Serialization") {}

bool Os::Test::RawTime::Tester::Serialization::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::Serialization::action(Os::Test::RawTime::Tester &state) {
    U8 data[100] = {0}; // use OS_RAWTIME_SERIALIZED_SIZE instead, somehow
    Fw::Buffer buffer(data, sizeof(data));

    FwIndexType index = state.pick_random_index();

    state.m_times[index].serialize(buffer.getSerializeRepr());
    state.m_times[index].serialize(buffer.getSerializeRepr());

    Os::RawTime raw_time;
    raw_time.deserialize(buffer.getSerializeRepr());

    U32 result = 1;
    Os::RawTime::Status status = state.m_times[index].getDiffUsec(raw_time, result);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);
    ASSERT_EQ(result, 0);
}
