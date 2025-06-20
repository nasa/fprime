// ======================================================================
// \title  MapBase
// \author bocchino
// \brief  An abstract base class template for a map
// ======================================================================

#ifndef Fw_MapBase_HPP
#define Fw_MapBase_HPP

#include "Fw/DataStructures/MapIterator.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename K, typename V>
class MapBase {
  private:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of an abstract map iterator
    using Iterator = MapIterator<K, V>;

  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    MapBase(const MapBase<K, V>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    MapBase() = default;

    //! Destructor
    virtual ~MapBase() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    MapBase<K, V>& operator=(const MapBase<K, V>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Clear the map
    virtual void clear() = 0;

    //! Copy data from another map
    void copyDataFrom(const MapBase<K, V>& map) {
        if (&map != this) {
            this->clear();
            const auto* e = map.getHeadIterator();
            const FwSizeType size = FW_MIN(map.getSize(), this->getCapacity());
            for (FwSizeType i = 0; i < size; i++) {
                FW_ASSERT(e != nullptr);
                const auto status = this->insert(e->getKey(), e->getValue());
                FW_ASSERT(status == Success::SUCCESS, static_cast<FwAssertArgType>(status));
                e = e->getNextMapIterator();
            }
        }
    }

    //! Find the value associated with a key in the map
    Success find(const K& key,  //!< The key (input)
                 V& value       //!< The value (output)
    ) const = 0;

    //! Get the capacity (maximum number of items stored in the map)
    //! \return The capacity
    virtual FwSizeType getCapacity() const = 0;

    //! Get the head iterator for the map
    virtual const Iterator* getHeadIterator() const = 0;

    //! Get the size (number of items stored in the map)
    //! \return The size
    virtual FwSizeType getSize() const = 0;

    //! Insert a (key, value) pair in the map
    //! \return SUCCESS if there is room in the map
    virtual Success insert(const K& key,   //!< The key
                           const V& value  //!< The value
                           ) = 0;
    //! Remove a (key, value) pair from the map
    //! Store the value into the value parameter if the key was there
    //! \return SUCCESS if the key was there
    virtual Success remove(const K& key,  //!< The key (input)
                           V& value       //!< The value (output)
                           ) = 0;
};

}  // namespace Fw

#endif
