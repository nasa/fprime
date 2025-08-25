// ======================================================================
// \file   FifoQueue.hpp
// \author bocchino
// \brief  A FIFO queue with internal storage
// ======================================================================

#ifndef Fw_FifoQueue_HPP
#define Fw_FifoQueue_HPP

#include "Fw/DataStructures/Array.hpp"
#include "Fw/DataStructures/ExternalFifoQueue.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class FifoQueue final : public FifoQueueBase<T> {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(C > 0, "capacity must be greater than zero");

    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT, FwSizeType CC>
    friend class FifoQueueTester;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    FifoQueue() : FifoQueueBase<T>(), m_extQueue(m_items, C) {}

    //! Copy constructor
    FifoQueue(const FifoQueue<T, C>& queue) : FifoQueueBase<T>(), m_extQueue(m_items, C) { *this = queue; }

    //! Destructor
    ~FifoQueue() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    FifoQueue<T, C>& operator=(const FifoQueue<T, C>& queue) {
        this->m_extQueue.copyDataFrom(queue);
        return *this;
    }

    //! Clear the queue
    void clear() override { this->m_extQueue.clear(); }

    //! Enqueue an item (push on the right)
    //! \return SUCCESS if item enqueued
    Success enqueue(const T& e  //!< The item (output)
                    ) override {
        return this->m_extQueue.enqueue(e);
    }

    //! Dequeue an item (remove from the left)
    //! \return SUCCESS if item dequeued
    Success dequeue(T& e  //!< The item (output)
                    ) override {
        return this->m_extQueue.dequeue(e);
    }

    //! Get the size (number of items stored in the queue)
    //! \return The size
    FwSizeType getSize() const override { return this->m_extQueue.getSize(); }

    //! Get the capacity (maximum number of items stored in the queue)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_extQueue.getCapacity(); }

    //! Get an item at an index.
    //! Indices go from left to right in the queue.
    //! Fails an assertion if the index is out of range.
    //! \return The item
    const T& at(FwSizeType index  //!< The index
    ) const override {
        return this->m_extQueue.at(index);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The external queue implementation
    ExternalFifoQueue<T> m_extQueue = {};

    //! The array providing the backing memory for m_extQueue
    T m_items[C] = {};
};

}  // namespace Fw

#endif
