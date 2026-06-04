// ======================================================================
// \title Os/CountingSemaphore.hpp
// \brief common function definitions for Os::CountingSemaphore
// ======================================================================
#include "Os/Os.hpp"

#ifndef OS_COUNTING_SEMAPHORE_HPP_
#define OS_COUNTING_SEMAPHORE_HPP_

namespace Os {

class CountingSemaphoreHandle {};

class CountingSemaphoreInterface {
  public:
    enum Status {
        OP_OK,                  //!< Operation was successful
        ERROR_TIMEOUT,          //!< Timeout occurred during wait
        ERROR_INVALID,          //!< Invalid semaphore or argument
        ERROR_NOT_IMPLEMENTED,  //!< Feature not implemented
        NOT_SUPPORTED,          //!< CountingSemaphore does not support operation
        ERROR_OTHER             //!< All other errors
    };

    CountingSemaphoreInterface() = default;
    virtual ~CountingSemaphoreInterface() = default;

    CountingSemaphoreInterface(const CountingSemaphoreInterface& other) = delete;

    virtual CountingSemaphoreInterface& operator=(const CountingSemaphoreInterface& other) = delete;

    virtual Status wait() = 0;

    virtual Status waitTimeout(U32 timeout_ms) = 0;

    virtual Status tryWait() = 0;

    virtual Status post() = 0;

    virtual CountingSemaphoreHandle* getHandle() = 0;

    static CountingSemaphoreInterface* getDelegate(CountingSemaphoreHandleStorage& aligned_new_memory,
                                                   U32 initial_count);
};

class CountingSemaphore final : public CountingSemaphoreInterface {
  public:
    CountingSemaphore(U32 initial_count);

    ~CountingSemaphore() final;

    CountingSemaphore(const CountingSemaphoreInterface& other) = delete;

    CountingSemaphore(const CountingSemaphore& other) = delete;
    CountingSemaphore& operator=(const CountingSemaphore& other) = delete;
    CountingSemaphore(CountingSemaphore&& other) = delete;
    CountingSemaphore& operator=(CountingSemaphore&& other) = delete;

    Status waitTimeout(U32 timeout_ms) override;

    Status tryWait() override;

    Status post() override;

    CountingSemaphoreHandle* getHandle() override;

  private:
    alignas(FW_HANDLE_ALIGNMENT) CountingSemaphoreHandleStorage m_handle_storage;
    CountingSemaphoreInterface& m_delegate;
};
}  // namespace Os
#endif  // OS_COUNTING_SEMAPHORE_HPP_
