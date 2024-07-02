// ======================================================================
// \title Os/Stub/test/ut/StubMutexTests.cpp
// \brief stub implementation for Os::MutexInterface testing
// This ensures the delegation of function calls happens properly
// ======================================================================
#include <gtest/gtest.h>
#include "Os/test/ut/mutex/CommonTests.hpp"
#include "Os/test/ut/mutex/RulesHeaders.hpp"
#include "Os/Stub/test/Mutex.hpp"

using namespace Os::Stub::Mutex::Test;


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


// Ensure that Os::Mutex properly calls the implementation constructor
TEST_F(Interface, Construction) {
    Os::Mutex mutex;
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CONSTRUCT_FN);
}

// Ensure that Os::Mutex properly calls the implementation destructor
TEST_F(Interface, Destruction) {
    delete (new Os::Mutex);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESTRUCT_FN);
}

// Ensure that Os::Mutex properly calls the implementation take()
TEST_F(Interface, Take) {
    Os::Mutex mutex;
    StaticData::data.takeStatus = Os::Mutex::Status::ERROR_OTHER;
    ASSERT_EQ(mutex.take(), StaticData::data.takeStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::TAKE_FN);
}

// Ensure that Os::Mutex properly calls the implementation release()
TEST_F(Interface, Release) {
    Os::Mutex mutex;
    StaticData::data.releaseStatus = Os::Mutex::Status::ERROR_OTHER;
    ASSERT_EQ(mutex.release(), StaticData::data.releaseStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RELEASE_FN);
}

// Ensure that Os::Mutex properly calls the implementation lock()
TEST_F(Interface, Lock) {
    Os::Mutex mutex;
    mutex.lock();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::LOCK_FN);

}

// Ensure that Os::Mutex properly calls the implementation unLock()
TEST_F(Interface, UnLock) {
    Os::Mutex mutex;
    mutex.unLock();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::UNLOCK_FN);
}

// Ensure that Os::Mutex properly calls the implementation unlock()
TEST_F(Interface, UnlockAlias) {
    Os::Mutex mutex;
    mutex.unlock();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::UNLOCK_FN);
}

// Ensure that Os::Mutex properly calls the implementation getHandle()
TEST_F(Interface, GetHandle) {
    Os::Mutex mutex;
    ASSERT_EQ(mutex.getHandle(), nullptr);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_HANDLE_FN);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
