// ======================================================================
// \title Os/test/ut/mutex/MutexRules.cpp
// \brief rule implementations for common testing of mutex
// ======================================================================

#include "RulesHeaders.hpp"
#include "MutexRules.hpp"
#include "STest/Pick/Pick.hpp"

// NOTE:
// in stub test, we are testing the interface -> t
// posix test, posix correctness
// common test, functionality and feed it into both

// use ASSERT_DEATH_IF_SUPPORTED to test asserts


// ------------------------------------------------------------------------------------------------------
// Rule:  LockMutex -> Lock a mutex successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::Mutex::Tester::LockMutex::LockMutex() :
    STest::Rule<Os::Test::Mutex::Tester>("LockMutex") {}

bool Os::Test::Mutex::Tester::LockMutex::precondition(const Os::Test::Mutex::Tester &state) {
    return state.m_state == Os::Test::Mutex::Tester::MutexState::UNLOCKED;
}

void Os::Test::Mutex::Tester::LockMutex::action(Os::Test::Mutex::Tester &state) {
    state.m_state = Os::Test::Mutex::Tester::MutexState::LOCKED;
    state.m_mutex.lock();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  UnlockMutex -> Unlock a locked mutex successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::Mutex::Tester::UnlockMutex::UnlockMutex() :
    STest::Rule<Os::Test::Mutex::Tester>("UnlockMutex") {}

bool Os::Test::Mutex::Tester::UnlockMutex::precondition(const Os::Test::Mutex::Tester &state) {
    return state.m_state == Os::Test::Mutex::Tester::MutexState::LOCKED;
}

void Os::Test::Mutex::Tester::UnlockMutex::action(Os::Test::Mutex::Tester &state) {
    state.m_state = Os::Test::Mutex::Tester::MutexState::UNLOCKED;
    state.m_mutex.unLock();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  TakeMutex -> Lock a mutex successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::Mutex::Tester::TakeMutex::TakeMutex() :
    STest::Rule<Os::Test::Mutex::Tester>("TakeMutex") {}

bool Os::Test::Mutex::Tester::TakeMutex::precondition(const Os::Test::Mutex::Tester &state) {
    return state.m_state == Os::Test::Mutex::Tester::MutexState::UNLOCKED;
}

void Os::Test::Mutex::Tester::TakeMutex::action(Os::Test::Mutex::Tester &state) {
    state.m_state = Os::Test::Mutex::Tester::MutexState::LOCKED;
    Os::Mutex::Status status = state.m_mutex.take();
    ASSERT_EQ(status, Os::Mutex::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReleaseMutex -> Lock a mutex successfully
// ------------------------------------------------------------------------------------------------------

Os::Test::Mutex::Tester::ReleaseMutex::ReleaseMutex() :
    STest::Rule<Os::Test::Mutex::Tester>("ReleaseMutex") {}

bool Os::Test::Mutex::Tester::ReleaseMutex::precondition(const Os::Test::Mutex::Tester &state) {
    return state.m_state == Os::Test::Mutex::Tester::MutexState::LOCKED;
}

void Os::Test::Mutex::Tester::ReleaseMutex::action(Os::Test::Mutex::Tester &state) {
    state.m_state = Os::Test::Mutex::Tester::MutexState::UNLOCKED;
    Os::Mutex::Status status = state.m_mutex.release();
    ASSERT_EQ(status, Os::Mutex::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ProtectDataCheck: Lock a mutex, set data, assert data and unlock mutex
// By running this concurrently with another thread, we can test the mutex does protect data
// ------------------------------------------------------------------------------------------------------
Os::Test::Mutex::Tester::ProtectDataCheck::ProtectDataCheck() :
    STest::Rule<Os::Test::Mutex::Tester>("ProtectDataCheck") {}

bool Os::Test::Mutex::Tester::ProtectDataCheck::precondition(const Os::Test::Mutex::Tester &state) {
    return true;
}

void Os::Test::Mutex::Tester::ProtectDataCheck::action(Os::Test::Mutex::Tester &state) {
    state.m_mutex.lock();
    state.m_state = Os::Test::Mutex::Tester::MutexState::LOCKED;

    state.m_value = 42;
    ASSERT_EQ(state.m_value, 42);

    state.m_state = Os::Test::Mutex::Tester::MutexState::UNLOCKED;
    state.m_mutex.unLock();
}


