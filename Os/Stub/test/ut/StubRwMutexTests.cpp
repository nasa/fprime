// ======================================================================
// \title Os/Stub/test/ut/StubRwMutexTests.cpp
// \brief stub implementation for Os::RwMutexInterface testing
// This ensures the delegation of function calls happens properly
// ======================================================================
#include <gtest/gtest.h>
#include "Os/Stub/test/RwMutex.hpp"
#include "Os/test/ut/rwmutex/CommonTests.hpp"
#include "Os/test/ut/rwmutex/RulesHeaders.hpp"

using namespace Os::Stub::RwMutex::Test;

// Basic file tests
class Interface : public ::testing::Test {
  public:
    //! Setup function delegating to UT setUp function
    void SetUp() override { StaticData::data = StaticData(); }

    //! Setup function delegating to UT tearDown function
    void TearDown() override { StaticData::data = StaticData(); }
};

// Ensure that Os::RwMutex properly calls the implementation constructor
TEST_F(Interface, Construction) {
    Os::RwMutex mutex;
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CONSTRUCT_FN);
}

// Ensure that Os::RwMutex properly calls the implementation destructor
TEST_F(Interface, Destruction) {
    delete (new Os::RwMutex);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESTRUCT_FN);
}

// Ensure that Os::RwMutex properly calls the implementation take()
TEST_F(Interface, Take) {
    Os::RwMutex mutex;
    StaticData::data.takeStatus = Os::RwMutex::Status::ERROR_OTHER;
    ASSERT_EQ(mutex.take(), StaticData::data.takeStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::TAKE_FN);
}

// Ensure that Os::RwMutex properly calls the implementation release()
TEST_F(Interface, Release) {
    Os::RwMutex mutex;
    StaticData::data.releaseStatus = Os::RwMutex::Status::ERROR_OTHER;
    ASSERT_EQ(mutex.release(), StaticData::data.releaseStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RELEASE_FN);
}

// Ensure that Os::RwMutex properly calls the implementation lock()
TEST_F(Interface, Lock) {
    Os::RwMutex mutex;
    mutex.lock();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::TAKE_FN);
}

// Ensure that Os::RwMutex properly calls the implementation unLock()
TEST_F(Interface, UnLock) {
    Os::RwMutex mutex;
    mutex.unLock();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RELEASE_FN);
}

// Ensure that Os::RwMutex properly calls the implementation unlock()
TEST_F(Interface, UnlockAlias) {
    Os::RwMutex mutex;
    mutex.unlock();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RELEASE_FN);
}

// Ensure that Os::RwMutex properly calls the implementation takeRead()
TEST_F(Interface, TakeRead) {
    Os::RwMutex rwMutex;
    
    StaticData::data.takeStatus = Os::RwMutex::Status::ERROR_BUSY;
    
    ASSERT_EQ(rwMutex.takeRead(), StaticData::data.takeStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::TAKE_READ_FN);
}

// Ensure that Os::RwMutex properly calls the implementation releaseRead()
TEST_F(Interface, ReleaseRead) {
    Os::RwMutex rwMutex;
    
    StaticData::data.releaseStatus = Os::RwMutex::Status::ERROR_OTHER;
    
    ASSERT_EQ(rwMutex.releaseRead(), StaticData::data.releaseStatus);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RELEASE_READ_FN);
}

// Ensure that Os::RwMutex properly calls the implementation lockRead()
TEST_F(Interface, LockRead) {
    Os::RwMutex rwMutex;
    
    rwMutex.lockRead();
    
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::TAKE_READ_FN);
}

// Ensure that Os::RwMutex properly calls the implementation unLockRead()
TEST_F(Interface, UnLockRead) {
    Os::RwMutex rwMutex;
    
    rwMutex.unLockRead();
    
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::RELEASE_READ_FN);
}

// Ensure that Os::RwMutex properly calls the implementation getHandle()
TEST_F(Interface, GetHandle) {
    Os::RwMutex mutex;
    ASSERT_EQ(mutex.getHandle(), nullptr);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_HANDLE_FN);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
