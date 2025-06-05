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

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
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

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    FifoQueueBase<T>& operator=(const FifoQueueBase<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Clear the queue
    virtual void clear() = 0;

    //! Copy data from another queue
    void copyDataFrom(const FifoQueueBase<T>& queue  //!< The queue
    ) {
        if (&queue != this) {
            this->clear();
            const FwSizeType size = FW_MIN(queue.getSize(), this->getCapacity());
            for (FwSizeType i = 0; i < size; i++) {
                const auto status = this->enqueue(queue.getElementAtIndex(i));
                FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(status));
            }
        }
    }

    //! Enqueue an element (add to the right)
    //! \return SUCCESS if element enqueued
    virtual Success enqueue(const T& e  //!< The element (output)
                            ) = 0;

    //! Peek an element at an index
    //! Indices go from left to right in the range [0, size)
    //! \return SUCCESS if element exists
    virtual Success peek(T& e,                 //!< The element (output)
                         FwSizeType index = 0  //!< The index (input)
    ) const = 0;

    //! Dequeue an element (pop from the left)
    //! \return SUCCESS if element dequeued
    virtual Success dequeue(T& e  //!< The element (output)
                            ) = 0;

    //! Get the size (number of items stored in the queue)
    //! \return The size
    virtual FwSizeType getSize() const = 0;

    //! Get the capacity (maximum number of items stored in the queue)
    //! \return The capacity
    virtual FwSizeType getCapacity() const = 0;

  protected:
    // ----------------------------------------------------------------------
    // Protected member functions
    // ----------------------------------------------------------------------

    //! Get an element at an index
    //! Indices go from left to right in the queue
    virtual const T& getElementAtIndex(FwSizeType index  //!< The index
    ) const = 0;
};

}  // namespace Fw

#endif
