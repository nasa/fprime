// ======================================================================
// \title Os/Posix/CountingSemaphore.hpp
// \brief Posix definitions for Os::CountingSemaphore
// ======================================================================
#ifndef OS_POSIX_COUNTING_SEMAPHORE_HPP
#define OS_POSIX_COUNTING_SEMAPHORE_HPP
#include <semaphore.h>
#include <Os/CountingSemaphore.hpp>

namespace Os {
namespace Posix {
namespace Semaphore {

struct PosixCountingSemaphoreHandle : public CountingSemaphoreHandle {
    sem_t m_semaphore{};
};

class PosixCountingSemaphore : public CountingSemaphoreInterface {
  public:
    explicit PosixCountingSemaphore(U32 initial_count);

    ~PosixCountingSemaphore() override;

    PosixCountingSemaphore(const PosixCountingSemaphore& other) = delete;

    CountingSemaphoreInterface& operator=(const CountingSemaphoreInterface& other) = delete;

    PosixCountingSemaphore::Status wait() override;

    PosixCountingSemaphore::Status waitTimeout(const Fw::TimeInterval& interval) override;

    PosixCountingSemaphore::Status tryWait() override;

    PosixCountingSemaphore::Status post() override;

    CountingSemaphoreHandle* getHandle() override;

  private:
    PosixCountingSemaphoreHandle m_handle;
};

}  // namespace Semaphore
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_COUNTING_SEMAPHORE_HPP
