// ======================================================================
// \title Os/Stub/RwMutex.cpp
// \brief stub implementation for Os::RwMutex
// ======================================================================
#include "Os/Stub/RwMutex.hpp"

namespace Os {
namespace Stub {
namespace Mutex {

StubRwMutex::Status StubRwMutex::take() {
    // Register this thread as a waiting writer (writer-preference policy)
    this->m_handle.m_writers_waiting.fetch_add(1, std::memory_order_acquire);

    // Acquire mutex if it's free
    FwSignedSizeType expected = StubRwMutexHandle::LockState::FREE;
    while (!this->m_handle.m_lock_state.compare_exchange_weak(expected, StubRwMutexHandle::LockState::WRITER_LOCKED)) {
        expected = StubRwMutexHandle::LockState::FREE;
    }
    // Successfully acquired exclusive lock.
    return Status::OP_OK;
}

StubRwMutex::Status StubRwMutex::release() {
    // Free mutex
    FwSignedSizeType prev_state = this->m_handle.m_lock_state.exchange(StubRwMutexHandle::LockState::FREE);
    // Decrement writer-waiting counter
    this->m_handle.m_writers_waiting.fetch_sub(static_cast<FwSizeType>(1), std::memory_order_release);
    // Validate that we actually held the exclusive lock
    if (prev_state != StubRwMutexHandle::LockState::WRITER_LOCKED) {
        // Coding defect: release() called without holding the lock,
        // or double-release, or release by non-owner thread.
        return Status::ERROR_OTHER;
    }
    // Lock successfully released
    return Status::OP_OK;
}

StubRwMutex::Status StubRwMutex::takeRead() {
    FwSignedSizeType curr_state;
    do {
        curr_state = this->m_handle.m_lock_state.load(std::memory_order_acquire);
        if (curr_state == StubRwMutexHandle::LockState::WRITER_LOCKED) {
            // Spin until writer releases (m_lock_state != WRITER_LOCKED)
            while (this->m_handle.m_writers_waiting.load(std::memory_order_relaxed) > 0) {
                // Spin-lock if writer locked
            }
            // Retry the outer loop
            continue;
        }
        // Try to increment readers count
    } while (!this->m_handle.m_lock_state.compare_exchange_weak(curr_state, curr_state + 1));
    // Successfully acquired shared lock
    return Status::OP_OK;
}

StubRwMutex::Status StubRwMutex::releaseRead() {
    // Decrement readers count
    FwSignedSizeType prev_state =
        this->m_handle.m_lock_state.fetch_sub(static_cast<FwSizeType>(1), std::memory_order_release);
    // Validate that we actually held a shared lock
    if (prev_state >= 0) {
        // Coding defect: releaseRead() called without holding a read lock,
        // or double-release, or mismatched takeRead/releaseRead pairing.
        return Status::ERROR_OTHER;
    }
    // The mutex released shared.
    return Status::OP_OK;
}

MutexHandle* StubRwMutex::getHandle() {
    return &this->m_handle;
}
}  // namespace Mutex
}  // namespace Stub
}  // namespace Os
