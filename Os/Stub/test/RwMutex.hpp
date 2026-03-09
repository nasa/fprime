// ======================================================================
// \title Os/Stub/test/RwMutex.cpp
// \brief definitions for TestRwMutex stubs for interface testing
// ======================================================================
#include "Os/RwMutex.hpp"

#ifndef OS_STUB_RWMUTEX_TEST_HPP
#define OS_STUB_RWMUTEX_TEST_HPP
namespace Os {
namespace Stub {
namespace RwMutex {
namespace Test {

//! Data that supports the stubbed RwMutex implementation.
//!/
struct StaticData {
    //! Enumeration of last function called
    //!
    enum class LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        TAKE_FN,          // Exclusive (write) lock via take()
        RELEASE_FN,       // Exclusive (write) unlock via release()
        TAKE_READ_FN,     // Shared (read) lock via takeRead()
        RELEASE_READ_FN,  // Shared (read) unlock via releaseRead()
        LOCK_FN,          // Exclusive lock via lock() [assert variant]
        UNLOCK_FN,        // Exclusive unlock via unLock() [assert variant]
        LOCK_READ_FN,     // Shared lock via lockRead() [assert variant]
        UNLOCK_READ_FN,   // Shared unlock via unlockRead() [assert variant]
        GET_HANDLE_FN
    };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = LastFn::NONE_FN;

    Os::RwMutex::Status takeStatus = Os::RwMutex::Status::OP_OK;
    Os::RwMutex::Status releaseStatus = Os::RwMutex::Status::OP_OK;

    // Singleton data
    static StaticData data;
};

//! Test task handle
class TestRwMutexHandle : public MutexHandle {};

//! Implementation of task
class TestRwMutex : public RwMutexInterface {
  public:
    //! Constructor
    TestRwMutex();

    //! Destructor
    ~TestRwMutex() override;

    //! \brief Lock mutex exclusively and return status
    Status take() override;

    //! \brief Unlock mutex exclusively and return status
    Status release() override;

    //! \brief Lock mutex shared (read) and return status
    Status takeRead() override;

    //! \brief Unlock mutex shared (read) and return status
    Status releaseRead() override;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal task handle representation
    MutexHandle* getHandle() override;
};

}  // namespace Test
}  // namespace RwMutex
}  // namespace Stub
}  // namespace Os
#endif  // End OS_STUB_RWMUTEX_TEST_HPP
