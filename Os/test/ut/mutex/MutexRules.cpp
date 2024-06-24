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
    // this now returns a thing with take()
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
// Rule:  LockBusyMutex: Lock a mutex that is already locked
// ------------------------------------------------------------------------------------------------------
Os::Test::Mutex::Tester::LockBusyMutex::LockBusyMutex() :
    STest::Rule<Os::Test::Mutex::Tester>("LockBusyMutex") {}

bool Os::Test::Mutex::Tester::LockBusyMutex::precondition(const Os::Test::Mutex::Tester &state) {
    return state.m_state == Os::Test::Mutex::Tester::MutexState::LOCKED;
}

void Os::Test::Mutex::Tester::LockBusyMutex::action(Os::Test::Mutex::Tester &state) {
    state.m_mutex.lock();
}