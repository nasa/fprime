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
    dispatch_semaphore_t m_semaphore;
};

class DarwinCountingSemaphore : public CountingSemaphoreInterface {
  public:
    DarwinCountingSemaphore(U32 initial_count, int pshared);

    ~DarwinCountingSemaphore() override;

    CountingSemaphoreInterface& operator=(const CountingSemaphoreInterface& other) override = delete;

    Status wait() override;

    Status waitTimeout(U32 timeout_ms) override;

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
