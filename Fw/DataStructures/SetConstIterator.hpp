// ======================================================================
// \title  SetConstIterator
// \author bocchino
// \brief  An abstract class template representing a const iterator for a set
// ======================================================================

#ifndef Fw_SetConstIterator_HPP
#define Fw_SetConstIterator_HPP

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/DataStructures/Nil.hpp"
#include "Fw/DataStructures/SetConstEntry.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename T>
class SetConstIterator {
  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of an array iterator
    using ArrayIterator = typename ArraySetOrMapImpl<T, Nil>::ConstIterator;

  private:
    // ----------------------------------------------------------------------
    // Private types
    // ----------------------------------------------------------------------

    //! The type of an implementation kind
    enum class ImplKind { ARRAY, RED_BLACK_TREE };

    //! The type of an implementation
    union Impl {
        Impl(const ArrayIterator& it) : array(it) {}
        //! An array iterator
        ArrayIterator array;
        // TODO: Add red-black tree implementation
        ~Impl() {}
    };

  public:
    // ----------------------------------------------------------------------
    // Constructors and destructors
    // ----------------------------------------------------------------------

    //! Constructor providing an array implementation
    SetConstIterator(const ArrayIterator& it) : m_implKind(ImplKind::ARRAY), m_impl(it), m_implIterator(m_impl.array) {}

    //! Copy constructor
    SetConstIterator(const SetConstIterator& it)
        : m_implKind(it.m_implKind), m_impl(it.m_impl.array), m_implIterator(it.m_implIterator) {
        // TODO: Handle tree case
    }

    //! Destructor
    ~SetConstIterator() {}

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Copy assignment operator
    SetConstIterator& operator=(const SetConstIterator&) = default;

    //! Equality comparison operator
    bool operator==(const SetConstIterator& it) {
        bool result = false;
        switch (this->m_implKind) {
            case ImplKind::ARRAY:
                result = this->m_impl.array.compareEqual(it.m_impl.array);
                break;
            case ImplKind::RED_BLACK_TREE:
                // TODO
                break;
            default:
                FW_ASSERT(0, static_cast<FwAssertArgType>(this->m_implKind));
                break;
        }
        return result;
    }

    //! Inequality comparison operator
    bool operator!=(const SetConstIterator& it) { return !(*this == it); };

    //! Prefix increment
    SetConstIterator& operator++() {
        this->m_implIterator.increment();
        return *this;
    }

    //! Prefix increment
    SetConstIterator operator++(int) {
        SetConstIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    //! Postfix increment
    void increment() { this->m_implIterator.increment(); }

    //! Check whether the iterator is in range
    bool isInRange() const { return this->m_implIterator.isInRange(); }

    //! Reset the iterator
    void reset() { return this->m_implIterator.reset(); }

    //! Dereference
    const SetConstEntry<T>& operator*() const { return this->m_implIterator.getEntry(); }

    //! Pointer
    const SetConstEntry<T>* operator->() const { return &this->m_implIterator.getEntry(); }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The implementation kind
    ImplKind m_implKind;

    //! The implementation
    Impl m_impl;

    //! The impl iterator
    SetOrMapImplConstIterator<T, Nil>& m_implIterator;
};

}  // namespace Fw

#endif
