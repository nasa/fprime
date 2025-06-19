// ======================================================================
// \title  MapIterator
// \author bocchino
// \brief  An abstract class representing an iterator for a map
// ======================================================================

#ifndef Fw_MapIterator_HPP
#define Fw_MapIterator_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename K, typename V>
class MapIterator {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    MapIterator(const MapIterator<K, V>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    MapIterator() = default;

    //! Destructor
    virtual ~MapIterator() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    MapIterator<K, V>& operator=(const MapIterator<K, V>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the key
    //! \return The key
    virtual const K& getKey() const = 0;

    //! Get the value
    //! \return The value
    virtual const V& getValue() const = 0;

    //! Get the next map iterator
    //! \return The map iterator, or nullptr if none
    virtual const MapIterator<K, V>* getNextMapIterator() const = 0;
};

}  // namespace Fw

#endif
