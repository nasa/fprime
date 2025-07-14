// ======================================================================
// \title  MapConstEntry
// \author bocchino
// \brief  An abstract class template representing a constant entry for a map
// ======================================================================

#ifndef Fw_MapConstEntry_HPP
#define Fw_MapConstEntry_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename K, typename V>
class MapConstEntry {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    MapConstEntry(const MapConstEntry<K, V>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    MapConstEntry() {}

    //! Destructor
    virtual ~MapConstEntry() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    MapConstEntry<K, V>& operator=(const MapConstEntry<K, V>&) = delete;

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
