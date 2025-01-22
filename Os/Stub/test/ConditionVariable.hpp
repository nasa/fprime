
#ifndef OS_STUB_TEST_CONDITION_VARIABLE_HPP
#define OS_STUB_TEST_CONDITION_VARIABLE_HPP
#include "Os/Condition.hpp"
#include <queue>
#include <deque>


namespace Os {
namespace Stub {
namespace ConditionVariable {
namespace Test {

//! Data that supports the stubbed File implementation.
//!
struct StaticData {
    //! Enumeration of last function called
    //!
    enum LastFn {
        NONE_FN,
        CONSTRUCT_FN,
        DESTRUCT_FN,
        WAIT_FN,
        NOTIFY_FN,
        NOTIFY_ALL_FN,
        HANDLE_FN
    };
    StaticData() = default;
    ~StaticData() = default;

    //! Last function called
    LastFn lastCalled = NONE_FN;
    Os::Mutex* passed = nullptr;
    ConditionVariableHandle* handle = nullptr;

    // Singleton data
    static StaticData data;
};


struct TestConditionVariableHandle : public ConditionVariableHandle {};

//! \brief interface testing condition variable
class TestConditionVariable : public ConditionVariableInterface {
  public:
    //! \brief default interface constructor
    TestConditionVariable();

    //! \brief default destructor
    ~TestConditionVariable() override;

    //! \brief copy constructor is forbidden
    TestConditionVariable(const ConditionVariableInterface& other) = delete;

    //! \brief copy constructor is forbidden
    TestConditionVariable(const ConditionVariableInterface* other) = delete;

    //! \brief assignment operator is forbidden
    ConditionVariableInterface& operator=(const ConditionVariableInterface& other) override = delete;

    //! \brief wait releasing mutex
    void wait(Os::Mutex& mutex) override;

    //! \brief notify a single waiter
    void notify() override;

    //! \brief notify all current waiters
    void notifyAll() override;

    ConditionVariableHandle* getHandle() override;

    TestConditionVariableHandle m_handle;
};

}  // namespace Test
}  // namespace Queue
}  // namespace Stub
}  // namespace Os

#endif  // OS_STUB_TEST_CONDITION_VARIABLE_HPP
