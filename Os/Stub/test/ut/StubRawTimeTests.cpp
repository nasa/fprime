// ======================================================================
// \title Os/Stub/test/ut/StubRawTimeTests.cpp
// \brief stub implementation for Os::RawTimeInterface testing
// This ensures the delegation of function calls happens properly
// ======================================================================
#include <gtest/gtest.h>
#include "Os/test/ut/rawtime/CommonTests.hpp"
#include "Os/test/ut/rawtime/RulesHeaders.hpp"
#include "Os/Stub/test/RawTime.hpp"
#include "Fw/Buffer/Buffer.hpp"

using namespace Os::Stub::RawTime::Test;


// Basic file tests
class Interface : public ::testing::Test {
public:
    //! Setup function delegating to UT setUp function
    void SetUp() override {
        StaticData::data = StaticData();
    }

    //! Setup function delegating to UT tearDown function
    void TearDown() override {
        StaticData::data = StaticData();
    }
};


// Ensure that Os::RawTime properly calls the implementation constructor
TEST_F(Interface, Construction) {
    Os::RawTime rawtime;
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CONSTRUCT_FN);
}

// Ensure that Os::RawTime properly calls the implementation constructor
TEST_F(Interface, CopyConstruction) {
    Os::RawTime rawtime;
    Os::RawTime rawtime_copy(rawtime);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::COPY_CONSTRUCT_FN);
}

// Ensure that Os::RawTime properly calls the implementation destructor
TEST_F(Interface, Destruction) {
    delete (new Os::RawTime);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESTRUCT_FN);
}

// Ensure that Os::RawTime properly calls the following delegate function
TEST_F(Interface, Now) {
    Os::RawTime rawtime;
    ASSERT_EQ(rawtime.now(), Os::RawTime::Status::OP_OK);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_TIME_FN);
}

// Ensure that Os::RawTime properly calls the following delegate function
TEST_F(Interface, GetTimeInterval) {
    Os::RawTime rawtime;
    Fw::TimeInterval unused;
    ASSERT_EQ(rawtime.getTimeInterval(rawtime, unused), Os::RawTime::Status::OP_OK);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_INTERVAL_FN);
}

// Ensure that Os::RawTime properly calls the following delegate function
TEST_F(Interface, Serialize) {
    Os::RawTime rawtime;
    Fw::Buffer buffer;
    ASSERT_EQ(rawtime.serialize(buffer.getSerializeRepr()), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::SERIALIZE_FN);
}

// Ensure that Os::RawTime properly calls the following delegate function
TEST_F(Interface, Deserialize) {
    Os::RawTime rawtime;
    Fw::Buffer buffer;
    ASSERT_EQ(rawtime.deserialize(buffer.getSerializeRepr()), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESERIALIZE_FN);
}

// Ensure that Os::RawTime properly calls the implementation getHandle()
TEST_F(Interface, GetHandle) {
    Os::RawTime rawtime;
    rawtime.getHandle();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_HANDLE_FN);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
