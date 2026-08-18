// ======================================================================
// \title Os/Darwin/CountingSemaphore.hpp
// \brief Darwin definitions for Os::CountingSemaphore
// ======================================================================
#ifndef OS_DARWIN_COUNTING_SEMAPHORE_HPP
#define OS_DARWIN_COUNTING_SEMAPHORE_HPP
#include <dispatch/dispatch.h>
#include <Os/CountingSemaphore.hpp>

namespace Os {
namespace Darwin {
namespace Semaphore {

struct DarwinCountingSemaphoreHandle : public CountingSemaphoreHandle {
    dispatch_semaphore_t m_semaphore = nullptr;
    long m_initial_count = 0;
};

class DarwinCountingSemaphore : public CountingSemaphoreInterface {
  public:
    explicit DarwinCountingSemaphore(U32 initial_count);

    ~DarwinCountingSemaphore() override;

    DarwinCountingSemaphore(const DarwinCountingSemaphore& other) = delete;

    CountingSemaphoreInterface& operator=(const CountingSemaphoreInterface& other) = delete;

    Status wait() override;

    Status waitTimeout(const Fw::TimeInterval& interval) override;

    Status tryWait() override;

    Status post() override;

    CountingSemaphoreHandle* getHandle() override;

  private:
    DarwinCountingSemaphoreHandle m_handle;
};

}  // namespace Semaphore
}  // namespace Darwin
}  // namespace Os
#endif  // OS_DARWIN_COUNTING_SEMAPHORE_HPP
