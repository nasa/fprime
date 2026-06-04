// ======================================================================
// \title Os/Stub/test/CountingSemaphore.hpp
// \brief definitions for TestCountingSemaphore stubs for interface testing
// ======================================================================
#include "Os/CountingSemaphore.hpp"

#ifndef OS_STUB_COUNTINGSEMAPHORE_TEST_HPP
#define OS_STUB_COUNTINGSEMAPHORE_TEST_HPP
namespace Os {
namespace Stub {
namespace CountingSemaphore {
namespace Test {

//! Data that supports the stubbed CountingSemaphore implementation.
//!
struct StaticData {
    //! Enumeration of last function called
    //!
    enum LastFn { NONE_FN, CONSTRUCT_FN, DESTRUCT_FN, WAIT_FN, WAIT_TIMEOUT_FN, TRY_WAIT_FN, POST_FN, GET_HANDLE_FN };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = NONE_FN;

    Os::CountingSemaphoreInterface::Status waitStatus = Os::CountingSemaphoreInterface::Status::OP_OK;
    Os::CountingSemaphoreInterface::Status waitTimeoutStatus = Os::CountingSemaphoreInterface::Status::OP_OK;
    Os::CountingSemaphoreInterface::Status tryWaitStatus = Os::CountingSemaphoreInterface::Status::OP_OK;
    Os::CountingSemaphoreInterface::Status postStatus = Os::CountingSemaphoreInterface::Status::OP_OK;

    // Singleton data
    static StaticData data;
};

//! Test counting semaphore handle
class TestCountingSemaphoreHandle : public CountingSemaphoreHandle {};

//! Implementation of counting semaphore
class TestCountingSemaphore : public CountingSemaphoreInterface {
  public:
    //! Constructor
    explicit TestCountingSemaphore(U32 initial_count);

    //! Destructor
    ~TestCountingSemaphore() override;

    //! \brief wait on semaphore and return status
    Status wait() override;

    //! \brief wait on semaphore with timeout and return status
    Status waitTimeout(const Fw::TimeInterval& interval) override;

    //! \brief try to wait on semaphore and return status
    Status tryWait() override;

    //! \brief post to semaphore and return status
    Status post() override;

    //! \brief return the underlying semaphore handle (implementation specific)
    //! \return internal semaphore handle representation
    CountingSemaphoreHandle* getHandle() override;
};

}  // namespace Test
}  // namespace CountingSemaphore
}  // namespace Stub
}  // namespace Os
#endif  // End OS_STUB_COUNTINGSEMAPHORE_TEST_HPP
