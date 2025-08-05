// ======================================================================
// \file   ExternalRedBlackTreeMap.hpp
// \author bocchino
// \brief  A map based on a red-black tree with external storage
// ======================================================================

#ifndef Fw_ExternalRedBlackTreeMap_HPP
#define Fw_ExternalRedBlackTreeMap_HPP

#include "Fw/DataStructures/MapBase.hpp"
#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

template <typename K, typename V>
class ExternalRedBlackTreeMap final : public MapBase<K, V> {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV>
    friend class ExternalRedBlackTreeMapTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a const iterator
    using ConstIterator = MapConstIterator<K, V>;

    //! The type of a tree node
    using Node = typename RedBlackTreeSetOrMapImpl<K, V>::Node;

    //! The type of a tree node index
    using Index = typename RedBlackTreeSetOrMapImpl<K, V>::Index;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalRedBlackTreeMap() = default;

    //! Constructor providing typed backing storage.
    //! nodes must point to at least capacity elements of type Node.
    //! freeNodes must point to at least capacity elements of type FwSizeType.
    ExternalRedBlackTreeMap(Node* nodes,         //!< The nodes
                            Index* freeNodes,    //!< The free nodes
                            FwSizeType capacity  //!< The capacity
                            )
        : MapBase<K, V>() {
        this->setStorage(nodes, freeNodes, capacity);
    }

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    ExternalRedBlackTreeMap(ByteArray data,      //!< The data,
                            FwSizeType capacity  //!< The capacity
                            )
        : MapBase<K, V>() {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ExternalRedBlackTreeMap(const ExternalRedBlackTreeMap<K, V>& map) : MapBase<K, V>() { *this = map; }

    //! Destructor
    ~ExternalRedBlackTreeMap() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ExternalRedBlackTreeMap<K, V>& operator=(const ExternalRedBlackTreeMap<K, V>& map) {
        if (&map != this) {
            this->m_impl = map.m_impl;
        }
        return *this;
    }

    //! Get the begin iterator
    //! \return The iterator
    ConstIterator begin() const override { return ConstIterator(this->m_impl.begin()); }

    //! Clear the map
    void clear() override { this->m_impl.clear(); }

    //! Get the end iterator
    //! \return The iterator
    ConstIterator end() const override { return ConstIterator(this->m_impl.end()); }

    //! Find a value associated with a key in the map
    //! \return SUCCESS if the item was found
    Success find(const K& key,  //!< The key
                 V& value       //!< The value
    ) const override {
        return this->m_impl.find(key, value);
    }

    //! Get the capacity of the map (max number of entries)
    //! \return The capacity
    FwSizeType getCapacity() const override { return this->m_impl.getCapacity(); }

    //! Get the size (number of entries)
    //! \return The size
    FwSizeType getSize() const override { return this->m_impl.getSize(); }

    //! Insert a (key, value) pair in the map
    //! \return SUCCESS if there is room in the map
    Success insert(const K& key,   //!< The key
                   const V& value  //!< The value
                   ) override {
        return this->m_impl.insert(key, value);
    }

    //! Remove a (key, value) pair from the map
    //! \return SUCCESS if the key was there
    Success remove(const K& key,  //!< The key
                   V& value       //!< The value
                   ) override {
        return this->m_impl.remove(key, value);
    }

    //! Set the backing storage (typed data)
    //! nodes must point to at least capacity elements of type Node.
    //! freeNodes must point to at least capacity elements of type FwSizeType.
    void setStorage(Node* nodes,         //!< The nodes
                    Index* freeNodes,    //!< The free nodes
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_impl.setStorage(nodes, freeNodes, capacity);
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< The data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_impl.setStorage(data, capacity);
    }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an RedBlackTreeSetOrMapImpl
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return RedBlackTreeSetOrMapImpl<K, V>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalArray of the specified capacity,
    //! as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType capacity  //!< The capacity
    ) {
        return RedBlackTreeSetOrMapImpl<K, V>::getByteArraySize(capacity);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The map implementation
    RedBlackTreeSetOrMapImpl<K, V> m_impl = {};
};

}  // namespace Fw

#endif
