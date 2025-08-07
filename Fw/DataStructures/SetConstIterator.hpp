// ======================================================================
// \title  SetConstIterator
// \author bocchino
// \brief  An abstract class template representing a const iterator for a set
// ======================================================================

#ifndef Fw_SetConstIterator_HPP
#define Fw_SetConstIterator_HPP

#include <new>

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/DataStructures/Nil.hpp"
#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
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

    //! The type of a red-black tree iterator
    using RedBlackTreeIterator = typename RedBlackTreeSetOrMapImpl<T, Nil>::ConstIterator;

  private:
    // ----------------------------------------------------------------------
    // Private types
    // ----------------------------------------------------------------------

    //! The type of an implementation kind
    using ImplKind = typename SetOrMapImplConstIterator<T, Nil>::ImplKind;

    //! The type of an implementation
    union Impl {
        //! Default constructor
        Impl() {}
        //! Array constructor
        Impl(const ArrayIterator& it) : array(it) {}
        //! Red-black tree constructor
        Impl(const RedBlackTreeIterator& it) : redBlackTree(it) {}
        //! An array iterator
        ArrayIterator array;
        //! A red-black tree iterator
        RedBlackTreeIterator redBlackTree;
        // ! Destructor
        ~Impl() {}
    };

  public:
    // ----------------------------------------------------------------------
    // Constructors and destructors
    // ----------------------------------------------------------------------

    //! Constructor providing an array implementation
    SetConstIterator(const ArrayIterator& it) : m_impl(it), m_implIterator(&m_impl.array) {}

    //! Constructor providing a red-black tree implementation
    SetConstIterator(const RedBlackTreeIterator& it) : m_impl(it), m_implIterator(&m_impl.redBlackTree) {}

    //! Copy constructor
    SetConstIterator(const SetConstIterator& it) : m_impl(), m_implIterator() {
        const auto implKind = it.getImplIterator().implKind();
        switch (implKind) {
            case ImplKind::ARRAY:
                this->m_implIterator = new (&this->m_impl.array) ArrayIterator(it.m_impl.array);
                break;
            case ImplKind::RED_BLACK_TREE:
                this->m_implIterator = new (&this->m_impl.redBlackTree) RedBlackTreeIterator(it.m_impl.redBlackTree);
                break;
            default:
                FW_ASSERT(0, static_cast<FwAssertArgType>(implKind));
                break;
        }
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
        const auto implKind1 = this->getImplIterator().implKind();
        const auto implKind2 = it.getImplIterator().implKind();
        if (implKind1 == implKind2) {
            switch (implKind1) {
                case ImplKind::ARRAY:
                    result = this->m_impl.array.compareEqual(it.m_impl.array);
                    break;
                case ImplKind::RED_BLACK_TREE:
                    result = this->m_impl.redBlackTree.compareEqual(it.m_impl.redBlackTree);
                    break;
                default:
                    FW_ASSERT(0, static_cast<FwAssertArgType>(implKind1));
                    break;
            }
        }
        return result;
    }

    //! Inequality comparison operator
    bool operator!=(const SetConstIterator& it) { return !(*this == it); };

    //! Prefix increment
    SetConstIterator& operator++() {
        this->getImplIterator().increment();
        return *this;
    }

    //! Postfix increment
    SetConstIterator operator++(int) {
        SetConstIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    //! Check whether the iterator is in range
    bool isInRange() const { return this->getImplIterator().isInRange(); }

    //! Dereference
    const T& operator*() const { return this->getImplIterator().getEntry().getKeyOrElement(); }

    //! Pointer
    const T* operator->() const { return &this->getImplIterator().getEntry().getKeyOrElement(); }

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Assert and get the impl iterator
    SetOrMapImplConstIterator<T, Nil>& getImplIterator() {
        FW_ASSERT(this->m_implIterator != nullptr);
        return *this->m_implIterator;
    }

    //! Assert and get the impl iterator (const)
    const SetOrMapImplConstIterator<T, Nil>& getImplIterator() const {
        FW_ASSERT(this->m_implIterator != nullptr);
        return *this->m_implIterator;
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The implementation
    Impl m_impl;

    //! The impl iterator
    SetOrMapImplConstIterator<T, Nil>* m_implIterator = nullptr;
};

}  // namespace Fw

#endif
