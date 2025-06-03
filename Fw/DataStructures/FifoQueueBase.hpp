// ======================================================================
// \title  FifoQueueBase
// \author bocchino
// \brief  An abstract base class for a FIFO queue
// ======================================================================

#ifndef Fw_FifoQueueBase_HPP
#define Fw_FifoQueueBase_HPP

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename T>
class FifoQueueBase {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Deleted copy constructor
    FifoQueueBase(const FifoQueueBase<T>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    FifoQueueBase() = default;

    //! Destructor
    virtual ~FifoQueueBase() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! Deleted operator=
    FifoQueueBase<T>& operator=(const FifoQueueBase<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the element at a specified index
    //! \return The element at index i
    virtual const T& at(FwSizeType i  //!< The index
    ) const = 0;

    //! Clear the queue
    virtual void clear() = 0;

    //! Copy data from another queue
    void copyDataFrom(const FifoQueueBase<T>& queue) {
        if (&queue != this) {
            this->clear();
            const FwSizeType size = FW_MIN(queue.getSize(), this->getCapacity());
            for (FwSizeType i = 0; i < size; i++) {
                const auto status = this->enqueue(queue.at(i));
                FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(status));
            }
        }
    }

    //! Enqueue an element
    //! \return SUCCESS if element enqueued
    virtual Success enqueue(const T& e  //!< The element
                            ) = 0;

    //! Peek an element
    //! \return SUCCESS if element exists
    Success peek(T& e  //!< The element
    ) const {
        auto status = Success::FAILURE;
        auto size = this->getSize();
        if (size > 0) {
            e = this->at(size - 1);
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Dequeue an element
    //! \return SUCCESS if element dequeued
    virtual Success dequeue(T& e  //!< The element
                            ) = 0;

    //! Get the size (number of items stored in the queue)
    //! \return The size
    virtual FwSizeType getSize() const = 0;

    //! Get the capacity (maximum number of items stored in the queue)
    //! \return The capacity
    virtual FwSizeType getCapacity() const = 0;
};

}  // namespace Fw

#endif
