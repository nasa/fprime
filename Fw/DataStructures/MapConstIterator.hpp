// ======================================================================
// \title  MapConstIterator
// \author bocchino
// \brief  An abstract class template representing an iterator for a map
// ======================================================================

#ifndef Fw_MapConstIterator_HPP
#define Fw_MapConstIterator_HPP

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename K, typename V>
class MapConstIterator {
  public:
    //! The type of an array iterator
    using ArrayIterator = typename ArraySetOrMapImpl<K, V>::ConstIterator;

  private:
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
    //! Constructor providing an array implementation
    MapConstIterator(const ArrayIterator& it) : m_implKind(ImplKind::ARRAY), m_impl(it), m_implIterator(m_impl.array) {}

    //! Copy constructor
    MapConstIterator(const MapConstIterator& it)
        : m_implKind(it.m_implKind), m_impl(it.m_impl.array), m_implIterator(it.m_implIterator) {
      // TODO: Handle tree case
    }

    //! Destructor
    ~MapConstIterator() {}

  public:
    //! Copy assignment operator
    MapConstIterator& operator=(const MapConstIterator&) = default;

    //! Equality comparison operator
    bool operator==(const MapConstIterator& it) {
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
    bool operator!=(const MapConstIterator& it) { return !(*this == it); };

    //! Prefix increment
    MapConstIterator& operator++() {
        this->m_implIterator.increment();
        return *this;
    }

    //! Prefix increment
    MapConstIterator operator++(int) { MapConstIterator tmp = *this; ++(*this); return tmp; }

    //! Postfix increment
    void increment() {
        this->m_implIterator.increment();
    }

    //! Get the key or element in the entry pointed to by this iterator
    const K& getKey() const { return this->m_implIterator.getKeyOrElement(); }

    //! Get the value in the entry pointed to by this iterator, or nil for a set
    const V& getValue() const { return this->m_implIterator.getValueOrNil(); }

    //! Check whether the iterator is in range
    bool isInRange() const { return this->m_implIterator.isInRange(); }

    //! Reset the iterator
    void reset() { return this->m_implIterator.reset(); }

    //! Dereference
    const MapConstIterator& operator*() const { return *this; }

    //! Pointer
    const MapConstIterator* operator->() const { return this; }

  private:
    //! The implementation kind
    ImplKind m_implKind;

    //! The implementation
    Impl m_impl;

    //! The impl iterator
    SetOrMapImplConstIterator<K, V>& m_implIterator;
};

}  // namespace Fw

#endif
