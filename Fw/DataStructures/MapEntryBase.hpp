// ======================================================================
// \title  MapEntryBase
// \author bocchino
// \brief  An abstract base class representing an entry in a map
// ======================================================================

#ifndef Fw_MapEntryBase_HPP
#define Fw_MapEntryBase_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename K, typename V>
class MapEntryBase {
  private:
    // ----------------------------------------------------------------------
    // Deleted elements
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    MapEntryBase(const MapEntryBase<K, V>&) = delete;

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    MapEntryBase<K, V>& operator=(const MapEntryBase<K, V>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    MapEntryBase() = default;

    //! Destructor
    virtual ~MapEntryBase() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the key associated with this entry
    //! \return The key
    virtual const K& getKey() const = 0;

    //! Get the value associated with this entry
    //! \return The value
    virtual const V& getValue() const = 0;
};

}  // namespace Fw

#endif
