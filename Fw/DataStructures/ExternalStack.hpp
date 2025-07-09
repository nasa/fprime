// ======================================================================
// \file   ExternalStack.hpp
// \author bocchino
// \brief  A stack with external storage
// ======================================================================

#ifndef Fw_ExternalStack_HPP
#define Fw_ExternalStack_HPP

#include "Fw/DataStructures/ExternalArray.hpp"
#include "Fw/DataStructures/StackBase.hpp"
#include "Fw/Types/ByteArray.hpp"

namespace Fw {

template <typename T>
class ExternalStack final : public StackBase<T> {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT>
    friend class ExternalStackTester;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalStack() = default;

    //! Constructor providing typed backing storage
    ExternalStack(T* items,            //!< The items
                  FwSizeType capacity  //!< The capacity
                  )
        : StackBase<T>() {
        this->setStorage(items, capacity);
    }

    //! Constructor providing untyped backing storage
    ExternalStack(ByteArray data,      //!< The data
                  FwSizeType capacity  //!< The capacity
                  )
        : StackBase<T>() {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ExternalStack(const ExternalStack<T>& stack) : StackBase<T>() { *this = stack; }

    //! Destructor
    ~ExternalStack() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ExternalStack<T>& operator=(const ExternalStack<T>& stack) {
        if (&stack != this) {
            this->m_items = stack.m_items;
            this->m_size = stack.m_size;
        }
        return *this;
    }

    //! Clear the stack
    void clear() override { this->m_size = 0; }

    //! Set the storage (typed data)
    void setStorage(T* items,            //!< The items
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_items.setStorage(items, capacity);
        this->clear();
    }

    //! Set the storage (untyped data)
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_items.setStorage(data, capacity);
        this->clear();
    }

    //! Push an element (push on the right)
    //! \return SUCCESS if element pushed
    Success push(const T& e  //!< The element (output)
                 ) override {
        auto status = Success::FAILURE;
        if (this->m_size < this->getCapacity()) {
            this->m_items[this->m_size] = e;
            this->m_size++;
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Get an item at an index.
    //! Index 0 is the rightmost (latest) element in the stack.
    //! Increasing indices go from right to left.
    //! Fails an assertion if the index is out of range.
    //! \return The item
    const T& at(FwSizeType index  //!< The index
    ) const override {
        FW_ASSERT(index < this->m_size, static_cast<FwAssertArgType>(index),
                  static_cast<FwAssertArgType>(this->m_size));
        return this->m_items[this->m_size - 1 - index];
    }

    //! Pop an element (remove from the right)
    //! \return SUCCESS if element popped
    Success pop(T& e  //!< The element (output)
                ) override {
        auto status = Success::FAILURE;
        if (this->m_size > 0) {
            e = this->at(0);
            this->m_size--;
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Get the size (number of items stored in the stack)
    //! \return The size
    FwSizeType getSize() const override { return this->m_size; }

    //! Get the capacity (maximum number of items stored in the stack)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_items.getSize(); }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an ExternalStack
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return ExternalArray<T>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalStack of the specified
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

    //! The array for storing the stack items
    ExternalArray<T> m_items = {};

    //! The number of items on the stack
    FwSizeType m_size = 0;
};

}  // namespace Fw

#endif
