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

    //! The type of a map entry
    using Entry = SetOrMapIterator<K, V>;

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalArrayMap() = default;

    //! Constructor providing typed backing storage.
    //! entries must point to at least capacity elements of type Entry.
    ExternalArrayMap(Entry* entries,      //!< The entries
                     FwSizeType capacity  //!< The capacity
                     )
        : MapBase<K, V>() {
        this->m_impl.setStorage(entries, capacity);
    }

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(capacity) bytes.
    ExternalArrayMap(ByteArray data,      //!< The data,
                     FwSizeType capacity  //!< The capacity
                     )
        : MapBase<K, V>() {
        this->m_impl.setStorage(data, capacity);
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

    // TODO

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The map implementation
    ArraySetOrMapImpl<K, V> m_impl = {};
};

}  // namespace Fw

#endif
