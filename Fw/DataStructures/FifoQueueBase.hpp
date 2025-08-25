// ======================================================================
// \title  FifoQueueBase
// \author bocchino
// \brief  An abstract base class template for a FIFO queue
// ======================================================================

#ifndef Fw_FifoQueueBase_HPP
#define Fw_FifoQueueBase_HPP

#include "Fw/DataStructures/SizedContainer.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename T>
class FifoQueueBase : public SizedContainer {
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
    FifoQueueBase() : SizedContainer() {}

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

    //! Get an item at an index.
    //! Indices go from left to right in the queue.
    //! Fails an assertion if the index is out of range.
    //! \return The item
    virtual const T& at(FwSizeType index  //!< The index
    ) const = 0;

    //! Copy data from another queue
    void copyDataFrom(const FifoQueueBase<T>& queue  //!< The queue
    ) {
        if (&queue != this) {
            this->clear();
            const FwSizeType size = FW_MIN(queue.getSize(), this->getCapacity());
            for (FwSizeType i = 0; i < size; i++) {
                const auto& e = queue.at(i);
                const auto status = this->enqueue(e);
                FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(status));
            }
        }
    }

    //! Enqueue an item (add to the right)
    //! \return SUCCESS if item enqueued
    virtual Success enqueue(const T& e  //!< The item (output)
                            ) = 0;

    //! Peek an item at an index
    //! Indices go from left to right in the range [0, size)
    //! \return SUCCESS if item exists
    Success peek(T& e,                 //!< The item (output)
                 FwSizeType index = 0  //!< The index (input)
    ) const {
        auto status = Success::FAILURE;
        if (index < this->getSize()) {
            e = this->at(index);
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Dequeue an item (remove from the left)
    //! \return SUCCESS if item dequeued
    virtual Success dequeue(T& e  //!< The item (output)
                            ) = 0;
};

}  // namespace Fw

#endif
