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
// Rule Now
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::Now::Now() :
    STest::Rule<Os::Test::RawTime::Tester>("Now") {}

bool Os::Test::RawTime::Tester::Now::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::Now::action(Os::Test::RawTime::Tester &state) {
    FwIndexType index = state.pick_random_index();

    Os::RawTime::Status status = state.m_times[index].now();
    state.shadow_now(index);
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

    state.shadow_validate_diff_result(result, shadow_result);
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

    state.shadow_validate_interval_result(index1, index2, interval);
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
    // In this test, we:
    //    1. Serialize a RawTime object in a buffer
    //    2. Deserialize the buffer into a new (stack-allocated) RawTime object
    //    3. Compare the deserialized RawTime object with the original RawTime object
    U8 data[Os::RawTime::SERIALIZED_SIZE] = {0};
    Fw::Buffer buffer(data, sizeof(data));

    FwIndexType index = state.pick_random_index();

    state.m_times[index].serialize(buffer.getSerializeRepr());

    Os::RawTime raw_time;
    raw_time.deserialize(buffer.getSerializeRepr());

    // We make sure that serialization and deserialization are successful by deserializing
    // into a new Os::RawTime object and comparing the difference between the original RawTime
    // test and the deserialized RawTime
    U32 result = 1; // initialize to non-zero value to ensure result is set by getDiffUsec
    Os::RawTime::Status status = state.m_times[index].getDiffUsec(raw_time, result);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OK);
    // We expect the result to be 0 since the deserialized RawTime should be the same as the original
    ASSERT_EQ(result, 0) << "Serialization test failed";
}

// ------------------------------------------------------------------------------------------------------
// Rule DiffU32Overflow
// ------------------------------------------------------------------------------------------------------
Os::Test::RawTime::Tester::DiffU32Overflow::DiffU32Overflow() :
    STest::Rule<Os::Test::RawTime::Tester>("DiffU32Overflow") {}

bool Os::Test::RawTime::Tester::DiffU32Overflow::precondition(const Os::Test::RawTime::Tester &state) {
    return true;
}

void Os::Test::RawTime::Tester::DiffU32Overflow::action(Os::Test::RawTime::Tester &state) {
    U32 result;
    Os::RawTime zero_time; // uninitialized time object is zero

    FwIndexType index = state.pick_random_index();
    Os::RawTime& now_time = state.m_times[index];

    Os::RawTime::Status status = zero_time.getDiffUsec(now_time, result);
    ASSERT_EQ(status, Os::RawTime::Status::OP_OVERFLOW);
}