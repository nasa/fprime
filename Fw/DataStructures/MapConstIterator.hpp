// ======================================================================
// \title  MapConstIterator
// \author bocchino
// \brief  An abstract class template representing a const iterator for a map
// ======================================================================

#ifndef Fw_MapConstIterator_HPP
#define Fw_MapConstIterator_HPP

#include <new>

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/DataStructures/MapEntryBase.hpp"
#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename K, typename V>
class MapConstIterator {
  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of an array iterator
    using ArrayIterator = typename ArraySetOrMapImpl<K, V>::ConstIterator;
    //! The type of a map entry base
    using EntryBase = MapEntryBase<K, V>;
    //! The type of a red-black tree iterator
    using RedBlackTreeIterator = typename RedBlackTreeSetOrMapImpl<K, V>::ConstIterator;

  private:
    // ----------------------------------------------------------------------
    // Private types
    // ----------------------------------------------------------------------

    //! The type of an implementation kind
    using ImplKind = typename SetOrMapImplConstIterator<K, V>::ImplKind;

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
    MapConstIterator(const ArrayIterator& it) : m_impl(it), m_implIterator(&m_impl.array) {}

    //! Constructor providing a red-black tree implementation
    MapConstIterator(const RedBlackTreeIterator& it) : m_impl(it), m_implIterator(&m_impl.redBlackTree) {}

    //! Copy constructor
    MapConstIterator(const MapConstIterator& it) : m_impl(), m_implIterator() {
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
    ~MapConstIterator() {}

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Copy assignment operator
    MapConstIterator& operator=(const MapConstIterator&) = default;

    //! Equality comparison operator
    bool operator==(const MapConstIterator& it) {
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
    bool operator!=(const MapConstIterator& it) { return !(*this == it); };

    //! Prefix increment
    MapConstIterator& operator++() {
        this->getImplIterator().increment();
        return *this;
    }

    //! Postfix increment
    MapConstIterator operator++(int) {
        MapConstIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    //! Check whether the iterator is in range
    bool isInRange() const { return this->getImplIterator().isInRange(); }

    //! Dereference
    const EntryBase& operator*() const { return this->getImplIterator().getEntry(); }

    //! Pointer
    const EntryBase* operator->() const { return &this->getImplIterator().getEntry(); }

  private:
    // ----------------------------------------------------------------------
    // Private helper functions
    // ----------------------------------------------------------------------

    //! Assert and get the impl iterator
    SetOrMapImplConstIterator<K, V>& getImplIterator() {
        FW_ASSERT(this->m_implIterator != nullptr);
        return *this->m_implIterator;
    }

    //! Assert and get the impl iterator (const)
    const SetOrMapImplConstIterator<K, V>& getImplIterator() const {
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
    SetOrMapImplConstIterator<K, V>* m_implIterator = nullptr;
};

}  // namespace Fw

#endif
