// ======================================================================
// \file   ArrayMap.hpp
// \author bocchino
// \brief  An array-based map with internal storage
// ======================================================================

#ifndef Fw_ArrayMap_HPP
#define Fw_ArrayMap_HPP

#include "Fw/DataStructures/ExternalArrayMap.hpp"

namespace Fw {

template <typename K, typename V, FwSizeType C>
class ArrayMap final : public MapBase<K, V> {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(C > 0, "capacity must be greater than zero");

    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV, FwSizeType CC>
    friend class ArrayMapTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a const iterator
    using ConstIterator = MapConstIterator<K, V>;

    //! The type of an implementation entry
    using Entry = SetOrMapImplEntry<K, V>;

    //! The type of the implementation entries
    using Entries = Entry[C];

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ArrayMap() : MapBase<K, V>(), m_extMap(m_entries, C) {}

    //! Copy constructor
    ArrayMap(const ArrayMap<K, V, C>& map) : MapBase<K, V>(), m_extMap(m_entries, C) { *this = map; }

    //! Destructor
    ~ArrayMap() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ArrayMap<K, V, C>& operator=(const ArrayMap<K, V, C>& map) {
        this->m_extMap.copyDataFrom(map);
        return *this;
    }

    //! Get the begin iterator
    //! \return The iterator
    ConstIterator begin() const override { return this->m_extMap.begin(); }

    //! Clear the map
    void clear() override { this->m_extMap.clear(); }

    //! Get the end iterator
    //! \return The iterator
    ConstIterator end() const override { return this->m_extMap.end(); }

    //! Find a value associated with a key in the map
    //! \return SUCCESS if the item was found
    Success find(const K& key,  //!< The key
                 V& value       //!< The value
    ) const override {
        return this->m_extMap.find(key, value);
    }

    //! Get the capacity of the map (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_extMap.getCapacity(); }

    //! Get the size (number of entries)
    //! \return The size
    FwSizeType getSize() const override { return this->m_extMap.getSize(); }

    //! Insert a (key, value) pair in the map
    //! \return SUCCESS if there is room in the map
    Success insert(const K& key,   //!< The key
                   const V& value  //!< The value
                   ) override {
        return this->m_extMap.insert(key, value);
    }

    //! Remove a (key, value) pair from the map
    //! \return SUCCESS if the key was there
    Success remove(const K& key,  //!< The key
                   V& value       //!< The value
                   ) override {
        return this->m_extMap.remove(key, value);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The external map implementation
    ExternalArrayMap<K, V> m_extMap = {};

    //! The array providing the backing memory for m_extMap
    Entries m_entries = {};
};

}  // namespace Fw

#endif
