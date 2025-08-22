// ======================================================================
// \file   RedBlackTreeMap.hpp
// \author bocchino
// \brief  An map based on a red-black tree with internal storage
// ======================================================================

#ifndef Fw_RedBlackTreeMap_HPP
#define Fw_RedBlackTreeMap_HPP

#include "Fw/DataStructures/ExternalRedBlackTreeMap.hpp"

namespace Fw {

template <typename K, typename V, FwSizeType C>
class RedBlackTreeMap final : public MapBase<K, V> {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(C > 0, "capacity must be greater than zero");

    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV, FwSizeType CC>
    friend class RedBlackTreeMapTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a const iterator
    using ConstIterator = MapConstIterator<K, V>;

    //! The type of a tree node
    using Node = typename RedBlackTreeSetOrMapImpl<K, V>::Node;

    //! The type of the tree node array
    using Nodes = Node[C];

    //! The type of a tree node index
    using Index = typename RedBlackTreeSetOrMapImpl<K, V>::Index;

    //! The type of the free node array
    using FreeNodes = Index[C];

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    RedBlackTreeMap() : MapBase<K, V>(), m_extMap(m_nodes, m_freeNodes, C) {}

    //! Copy constructor
    RedBlackTreeMap(const RedBlackTreeMap<K, V, C>& map) : MapBase<K, V>(), m_extMap(m_nodes, m_freeNodes, C) {
        *this = map;
    }

    //! Destructor
    ~RedBlackTreeMap() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    RedBlackTreeMap<K, V, C>& operator=(const RedBlackTreeMap<K, V, C>& map) {
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

    //! The array for storing the tree nodes
    Nodes m_nodes = {};

    //! The array for storing the free node indices
    FreeNodes m_freeNodes = {};

    //! The external map implementation
    ExternalRedBlackTreeMap<K, V> m_extMap = {};
};

}  // namespace Fw

#endif
