// ======================================================================
// \file   ExternalFifoQueue.hpp
// \author bocchino
// \brief  A FIFO queue with external storage
// ======================================================================

#ifndef Fw_ExternalFifoQueue_HPP
#define Fw_ExternalFifoQueue_HPP

#include "Fw/DataStructures/CircularIndex.hpp"
#include "Fw/DataStructures/ExternalArray.hpp"
#include "Fw/DataStructures/FifoQueueBase.hpp"
#include "Fw/Types/ByteArray.hpp"

namespace Fw {

template <typename T>
class ExternalFifoQueue final : public FifoQueueBase<T> {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT>
    friend class ExternalFifoQueueTester;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalFifoQueue() = default;

    //! Constructor providing typed backing storage
    ExternalFifoQueue(T* items,            //!< The items
                      FwSizeType capacity  //!< The capacity
                      )
        : FifoQueueBase<T>() {
        this->setStorage(items, capacity);
    }

    //! Constructor providing untyped backing storage
    ExternalFifoQueue(ByteArray data,      //!< The data
                      FwSizeType capacity  //!< The capacity
                      )
        : FifoQueueBase<T>() {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ExternalFifoQueue(const ExternalFifoQueue<T>& queue) : FifoQueueBase<T>() { *this = queue; }

    //! Destructor
    ~ExternalFifoQueue() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ExternalFifoQueue<T>& operator=(const ExternalFifoQueue<T>& queue) {
        if (&queue != this) {
            this->m_items = queue.m_items;
            this->m_enqueueIndex = queue.m_enqueueIndex;
            this->m_dequeueIndex = queue.m_dequeueIndex;
            this->m_size = queue.m_size;
        }
        return *this;
    }

    //! Clear the queue
    void clear() override {
        this->m_enqueueIndex.setValue(0);
        this->m_dequeueIndex.setValue(0);
        this->m_size = 0;
    }

    //! Set the storage (typed data)
    void setStorage(T* items,            //!< The items
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_items.setStorage(items, capacity);
        if (capacity > 0) {
            this->m_enqueueIndex.setModulus(capacity);
            this->m_dequeueIndex.setModulus(capacity);
        }
        this->clear();
    }

    //! Set the storage (untyped data)
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_items.setStorage(data, capacity);
        if (capacity > 0) {
            this->m_enqueueIndex.setModulus(capacity);
            this->m_dequeueIndex.setModulus(capacity);
        }
        this->clear();
    }

    //! Enqueue an element (push on the right)
    //! \return SUCCESS if element enqueued
    Success enqueue(const T& e  //!< The element (output)
                    ) override {
        auto status = Success::FAILURE;
        if (this->m_size < this->getCapacity()) {
            const auto i = this->m_enqueueIndex.getValue();
            this->m_items[i] = e;
            (void)this->m_enqueueIndex.increment();
            this->m_size++;
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Get an item at an index.
    //! Indices go from left to right in the queue.
    //! Fails an assertion if the index is out of range.
    //! \return The item
    const T& at(FwSizeType index  //!< The index
    ) const override {
        FW_ASSERT(index < this->m_size, static_cast<FwAssertArgType>(index),
                  static_cast<FwAssertArgType>(this->m_size));
        auto ci = this->m_dequeueIndex;
        const auto i = ci.increment(index);
        return this->m_items[i];
    }

    //! Dequeue an element (remove from the left)
    //! \return SUCCESS if element dequeued
    Success dequeue(T& e  //!< The element (output)
                    ) override {
        auto status = Success::FAILURE;
        if (this->m_size > 0) {
            e = this->at(0);
            (void)this->m_dequeueIndex.increment();
            this->m_size--;
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Get the size (number of items stored in the queue)
    //! \return The size
    FwSizeType getSize() const override { return this->m_size; }

    //! Get the capacity (maximum number of items stored in the queue)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_items.getSize(); }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an ExternalFifoQueue
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return ExternalArray<T>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalFifoQueue of the specified
    //! capacity, as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType capacity  //!< The capacity
    ) {
        return ExternalArray<T>::getByteArraySize(capacity);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array for storing the queue items
    ExternalArray<T> m_items = {};

    //! The enqueue index
    CircularIndex m_enqueueIndex = {};

    //! The dequeue index
    CircularIndex m_dequeueIndex = {};

    //! The number of items on the queue
    FwSizeType m_size = 0;
};

}  // namespace Fw

#endif
