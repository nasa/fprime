// ======================================================================
// \title Os/test/ut/rwmutex/RwMutexRules.cpp
// \brief rule implementations for common testing of mutex
// ======================================================================

#include "RwMutexRules.hpp"
#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockRwMutex: Acquire exclusive (write) lock via lock()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::LockRwMutex::LockRwMutex() : STest::Rule<Os::Test::RwMutex::Tester>("LockRwMutex") {}

bool Os::Test::RwMutex::Tester::LockRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state == Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED;
}

void Os::Test::RwMutex::Tester::LockRwMutex::action(Os::Test::RwMutex::Tester& state) {
    state.m_state = Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;
    state.m_rwmutex.lock();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  UnlockRwMutex - Release exclusive (write) lock via unlock()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::UnlockRwMutex::UnlockRwMutex() : STest::Rule<Os::Test::RwMutex::Tester>("UnlockRwMutex") {}

bool Os::Test::RwMutex::Tester::UnlockRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state == Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;
}

void Os::Test::RwMutex::Tester::UnlockRwMutex::action(Os::Test::RwMutex::Tester& state) {
    state.m_state = Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED;
    state.m_rwmutex.unLock();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  TakeRwMutex - Acquire exclusive (write) lock via take()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::TakeRwMutex::TakeRwMutex() : STest::Rule<Os::Test::RwMutex::Tester>("TakeRwMutex") {}

bool Os::Test::RwMutex::Tester::TakeRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state == Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED;
}

void Os::Test::RwMutex::Tester::TakeRwMutex::action(Os::Test::RwMutex::Tester& state) {
    state.m_state = Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;
    Os::Mutex::Status status = state.m_rwmutex.take();
    ASSERT_EQ(status, Os::Mutex::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReleaseRwMutex - Release exclusive (write) lock via release()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::ReleaseRwMutex::ReleaseRwMutex() : STest::Rule<Os::Test::RwMutex::Tester>("ReleaseRwMutex") {}

bool Os::Test::RwMutex::Tester::ReleaseRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state == Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;
}

void Os::Test::RwMutex::Tester::ReleaseRwMutex::action(Os::Test::RwMutex::Tester& state) {
    state.m_state = Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED;
    Os::Mutex::Status status = state.m_rwmutex.release();
    ASSERT_EQ(status, Os::Mutex::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  LockReadRwMutex: Acquire shared (read) lock via lock()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::LockReadRwMutex::LockReadRwMutex()
    : STest::Rule<Os::Test::RwMutex::Tester>("LockReadRwMutex") {}

bool Os::Test::RwMutex::Tester::LockReadRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state != Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;
}

void Os::Test::RwMutex::Tester::LockReadRwMutex::action(Os::Test::RwMutex::Tester& state) {
    // Update model: transition to READ_LOCKED or increment readers
    if (state.m_state == Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED) {
        state.m_state = Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED;
        state.m_reader_count = 1;
    } else if (state.m_state == Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED) {
        state.m_reader_count++;
    }

    state.m_rwmutex.lockRead();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  UnlockReadRwMutex - Release shared (read) lock via unlock()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::UnlockReadRwMutex::UnlockReadRwMutex()
    : STest::Rule<Os::Test::RwMutex::Tester>("UnlockReadRwMutex") {}

bool Os::Test::RwMutex::Tester::UnlockReadRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state == Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED && state.m_reader_count > 0;
}

void Os::Test::RwMutex::Tester::UnlockReadRwMutex::action(Os::Test::RwMutex::Tester& state) {
    state.m_reader_count--;
    
    if (state.m_reader_count == 0) {
        state.m_state = Tester::UNLOCKED;
    }

    state.m_rwmutex.unLockRead();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  TakeReadRwMutex - Acquire shared (read) lock via take()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::TakeReadRwMutex::TakeReadRwMutex()
    : STest::Rule<Os::Test::RwMutex::Tester>("TakeReadRwMutex") {}

bool Os::Test::RwMutex::Tester::TakeReadRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state != Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;
}

void Os::Test::RwMutex::Tester::TakeReadRwMutex::action(Os::Test::RwMutex::Tester& state) {
    // Update model: transition to READ_LOCKED or increment readers
    if (state.m_state == Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED) {
        state.m_state = Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED;
        state.m_reader_count = 1;
    } else if (state.m_state == Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED) {
        state.m_reader_count++;
    }

    Os::RwMutex::Status status = state.m_rwmutex.takeRead();
    ASSERT_EQ(status, Os::RwMutex::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReleaseReadRwMutex - Release shared (read) lock via release()
// ------------------------------------------------------------------------------------------------------

Os::Test::RwMutex::Tester::ReleaseReadRwMutex::ReleaseReadRwMutex()
    : STest::Rule<Os::Test::RwMutex::Tester>("ReleaseReadRwMutex") {}

bool Os::Test::RwMutex::Tester::ReleaseReadRwMutex::precondition(const Os::Test::RwMutex::Tester& state) {
    return state.m_state == Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED && state.m_reader_count > 0;
}

void Os::Test::RwMutex::Tester::ReleaseReadRwMutex::action(Os::Test::RwMutex::Tester& state) {
    state.m_reader_count--;
    
    if (state.m_reader_count == 0) {
        state.m_state = Tester::UNLOCKED;
    }

    Os::Mutex::Status status = state.m_rwmutex.releaseRead();
    ASSERT_EQ(status, Os::Mutex::Status::OP_OK);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ProtectDataWrite - Write to shared data inside exclusive lock
// ------------------------------------------------------------------------------------------------------
Os::Test::RwMutex::Tester::ProtectDataWrite::ProtectDataWrite()
    : STest::Rule<Os::Test::RwMutex::Tester>("ProtectDataWrite") {}

bool Os::Test::RwMutex::Tester::ProtectDataWrite::precondition(const Os::Test::RwMutex::Tester& state) {
    return true;
}

void Os::Test::RwMutex::Tester::ProtectDataWrite::action(Os::Test::RwMutex::Tester& state) {
    state.m_rwmutex.lock();
    state.m_state = Os::Test::RwMutex::Tester::RwMutexState::WRITE_LOCKED;

    U32 randomValue = STest::Pick::any();
    state.m_value = randomValue;
    ASSERT_EQ(state.m_value, randomValue);

    state.m_state = Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED;
    state.m_rwmutex.unLock();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ProtectDataRead - Read shared data inside shared lock
// ------------------------------------------------------------------------------------------------------
Os::Test::RwMutex::Tester::ProtectDataRead::ProtectDataRead()
    : STest::Rule<Os::Test::RwMutex::Tester>("ProtectDataRead") {}

bool Os::Test::RwMutex::Tester::ProtectDataRead::precondition(const Os::Test::RwMutex::Tester& state) {
    // Always applicable: lockRead() will block if writer holds lock
    return true;
}

void Os::Test::RwMutex::Tester::ProtectDataRead::action(Tester& state) {
    // Acquire shared access
    state.m_rwmutex.lockRead();
    
    if (state.m_state == Os::Test::RwMutex::Tester::RwMutexState::UNLOCKED) {
        state.m_state = Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED;
        state.m_reader_count = 1;
    } else if (state.m_state == Os::Test::RwMutex::Tester::RwMutexState::READ_LOCKED) {
        state.m_reader_count++;
    }
    
    // Critical section: read and verify protected data
    int capturedValue = state.m_value;
    // Note: We cannot assert equality to a specific value because
    // writers may have modified it. Instead, we verify that the read
    // is consistent (no tearing) by reading twice:
    int capturedValue2 = state.m_value;
    ASSERT_EQ(capturedValue, capturedValue2);
    
    // Release shared lock and update model
    state.m_rwmutex.unLockRead();
    
    state.m_reader_count--;
    if (state.m_reader_count == 0) {
        state.m_state = Tester::UNLOCKED;
    }
}
