// ======================================================================
// \title Os/DelegateCountingSemaphore.hpp
// \brief defines a delegate-based implementation of CountingSemaphoreInterface
// ======================================================================
#ifndef OS_DELEGATECOUNTINGSEMAPHORE_HPP_
#define OS_DELEGATECOUNTINGSEMAPHORE_HPP_

#include <Os/CountingSemaphoreInterface.hpp>

namespace Os {

class DelegateCountingSemaphore final : public CountingSemaphoreInterface {
  public:
    explicit DelegateCountingSemaphore(U32 initial_count);  //!< Constructor with initial count

    ~DelegateCountingSemaphore() final;  //!< Destructor

    DelegateCountingSemaphore(const CountingSemaphoreInterface& other) = delete;

    DelegateCountingSemaphore(const DelegateCountingSemaphore& other) = delete;
    DelegateCountingSemaphore& operator=(const DelegateCountingSemaphore& other) = delete;
    DelegateCountingSemaphore(DelegateCountingSemaphore&& other) = delete;
    DelegateCountingSemaphore& operator=(DelegateCountingSemaphore&& other) = delete;

    Status wait() override;  //!< wait (decrement), blocking if count is zero

    Status waitTimeout(const Fw::TimeInterval& interval) override;  //!< wait with timeout

    Status tryWait() override;

    Status post() override;

    CountingSemaphoreHandle* getHandle() override;

  private:
    alignas(FW_HANDLE_ALIGNMENT) CountingSemaphoreHandleStorage m_handle_storage;
    CountingSemaphoreInterface& m_delegate;
};
}  // namespace Os
#endif  // OS_DELEGATECOUNTINGSEMAPHORE_HPP_
