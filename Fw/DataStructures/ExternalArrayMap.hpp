// ======================================================================
// \file   ExternalArrayMap.hpp
// \author bocchino
// \brief  An array-based map with external storage
// ======================================================================

#ifndef Fw_ExternalArrayMap_HPP
#define Fw_ExternalArrayMap_HPP

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/DataStructures/MapBase.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

template <typename K, typename V>
class ExternalArrayMap final : public MapBase<K, V> {
    // ----------------------------------------------------------------------
    // Friend class for testing
    // ----------------------------------------------------------------------

    template <typename KK, typename VV>
    friend class ExternalArrayMapTester;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a map implementation entry
    using ImplEntry = SetOrMapImplEntry<K, V>;

    //! The type of a map entry
    using MapEntry = MapEntry<K, V>;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalArrayMap() = default;

    //! Constructor providing typed backing storage.
    //! entries must point to at least capacity elements of type ImplEntry.
    ExternalArrayMap(ImplEntry* entries,  //!< The entries
                     FwSizeType capacity  //!< The capacity
                     )
        : MapBase<K, V>() {
        this->setStorage(entries, capacity);
    }

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    ExternalArrayMap(ByteArray data,      //!< The data,
                     FwSizeType capacity  //!< The capacity
                     )
        : MapBase<K, V>() {
        this->setStorage(data, capacity);
    }

    //! Copy constructor
    ExternalArrayMap(const ExternalArrayMap<K, V>& map) : MapBase<K, V>() { *this = map; }

    //! Destructor
    ~ExternalArrayMap() override = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! operator=
    ExternalArrayMap<K, V>& operator=(const ExternalArrayMap<K, V>& map) {
        if (&map != this) {
            this->m_impl = map.m_impl;
        }
        return *this;
    }

    //! Clear the map
    void clear() override { this->m_impl.clear(); }

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

    //! Get the head map entry for the map
    //! \return The map entry
    const MapEntry* getHeadMapEntry() const override { return this->m_impl.getHeadEntry(); }

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
    //! entries must point to at least capacity elements of type ImplEntry.
    void setStorage(ImplEntry* entries,  //!< The entries
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_impl.setStorage(entries, capacity);
    }

    //! Set the backing storage (untyped data)
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    void setStorage(ByteArray data,      //!< THe data
                    FwSizeType capacity  //!< The capacity
    ) {
        this->m_impl.setStorage(data, capacity);
    }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an ArraySetOrMapImpl
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return ArraySetOrMapImpl<K, V>::getByteArrayAlignment(); }

    //! Get the size of the storage for an ExternalArray of the specified capacity,
    //! as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType capacity  //!< The capacity
    ) {
        return ArraySetOrMapImpl<K, V>::getByteArraySize(capacity);
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The map implementation
    ArraySetOrMapImpl<K, V> m_impl = {};
};

}  // namespace Fw

#endif
