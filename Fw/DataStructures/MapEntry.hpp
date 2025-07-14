// ======================================================================
// \title  MapEntry
// \author bocchino
// \brief  An abstract class template representing an entry in a map
// ======================================================================

#ifndef Fw_MapEntry_HPP
#define Fw_MapEntry_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename K, typename V>
class MapEntry {
  private:
    // ----------------------------------------------------------------------
    // Deleted elements
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    MapEntry(const MapEntry<K, V>&) = delete;

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    MapEntry<K, V>& operator=(const MapEntry<K, V>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    MapEntry() {}

    //! Destructor
    virtual ~MapEntry() = default;

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
