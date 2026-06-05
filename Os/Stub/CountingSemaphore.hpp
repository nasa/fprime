// ======================================================================
// \title Os/Stub/CountingSemaphore.hpp
// \brief Stub definitions for Os::CountingSemaphore
// ======================================================================
#ifndef OS_STUB_COUNTING_SEMAPHORE_HPP
#define OS_STUB_COUNTING_SEMAPHORE_HPP
#include <Os/CountingSemaphore.hpp>

namespace Os {
namespace Stub {
namespace Semaphore {

struct StubCountingSemaphoreHandle : public CountingSemaphoreHandle {};

class StubCountingSemaphore : public CountingSemaphoreInterface {
  public:
    explicit StubCountingSemaphore(U32 initial_count);

    ~StubCountingSemaphore() override = default;

    StubCountingSemaphore(const StubCountingSemaphore& other) = delete;

    CountingSemaphoreInterface& operator=(const CountingSemaphoreInterface& other) = delete;

    StubCountingSemaphore::Status wait() override;

    StubCountingSemaphore::Status waitTimeout(const Fw::TimeInterval& interval) override;

    StubCountingSemaphore::Status tryWait() override;

    StubCountingSemaphore::Status post() override;

    CountingSemaphoreHandle* getHandle() override;

  private:
    StubCountingSemaphoreHandle m_handle;
};

}  // namespace Semaphore
}  // namespace Stub
}  // namespace Os
#endif  // OS_STUB_COUNTING_SEMAPHORE_HPP
