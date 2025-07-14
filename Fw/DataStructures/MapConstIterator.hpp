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
        //! An array iterator
        ArrayIterator arrayIterator;
        // TODO: Add red-black tree implementation
    };

  public:
    //! Constructor providing an array implementation
    MapConstIterator(const ArrayIterator& it) : m_implKind(ImplKind::ARRAY), m_implIterator(m_impl.array) {
        this->m_impl.array = it;
    }

    //! Copy constructor
    MapConstIterator(const MapConstIterator& it) = delete;

    //! Destructor
    ~MapConstIterator() = default;

  public:
    //! Copy assignment operator
    MapConstIterator& operator=(const MapConstIterator&) = delete;

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

    //! Increment operator
    MapConstIterator& operator++() {
        this->m_implIterator.increment();
        return *this;
    }

    //! Get the key or element in the entry pointed to by this iterator
    const K& getKey() const { return this->m_implIterator.getKeyOrElement(); }

    //! Get the value in the entry pointed to by this iterator, or nil for a set
    const V& getValue() const { return this->m_implIterator.getValueOrNil(); }

    //! Check whether the iterator is in range
    bool isInRange() const { return this->m_implIterator.isInRange(); }

    //! Reset the iterator
    void reset() { return this->m_implIterator.reset(); }

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
