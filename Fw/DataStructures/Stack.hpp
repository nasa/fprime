// ======================================================================
// \file   Stack.hpp
// \author bocchino
// \brief  A stack with internal storage
// ======================================================================

#ifndef Fw_Stack_HPP
#define Fw_Stack_HPP

#include "Fw/DataStructures/Array.hpp"
#include "Fw/DataStructures/ExternalStack.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class Stack final : public StackBase<T> {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(C > 0, "capacity must be greater than zero");

    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename TT, FwSizeType CC>
    friend class StackTester;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    Stack() : StackBase<T>(), m_extStack(m_items, C) {}

    //! Copy constructor
    Stack(const Stack<T, C>& stack) : StackBase<T>(), m_extStack(m_items, C) { *this = stack; }

    //! Destructor
    ~Stack() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    Stack<T, C>& operator=(const Stack<T, C>& stack) {
        this->m_extStack.copyDataFrom(stack);
        return *this;
    }

    //! Clear the stack
    void clear() override { this->m_extStack.clear(); }

    //! Push an item (push on the right)
    //! \return SUCCESS if item pushed
    Success push(const T& e  //!< The item (output)
                 ) override {
        return this->m_extStack.push(e);
    }

    //! Pop an item (remove from the right)
    //! \return SUCCESS if item popped
    Success pop(T& e  //!< The item (output)
                ) override {
        return this->m_extStack.pop(e);
    }

    //! Get the size (number of items stored in the stack)
    //! \return The size
    FwSizeType getSize() const override { return this->m_extStack.getSize(); }

    //! Get the capacity (maximum number of items stored in the stack)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_extStack.getCapacity(); }

    //! Get an item at an index.
    //! Index 0 is the rightmost (latest) element in the stack.
    //! Increasing indices go from right to left.
    //! Fails an assertion if the index is out of range.
    //! \return The item
    const T& at(FwSizeType index  //!< The index
    ) const override {
        return this->m_extStack.at(index);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The external stack implementation
    ExternalStack<T> m_extStack = {};

    //! The array providing the backing memory for m_extStack
    T m_items[C] = {};
};

}  // namespace Fw

#endif
