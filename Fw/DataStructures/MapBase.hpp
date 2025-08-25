// ======================================================================
// \title  MapBase
// \author bocchino
// \brief  An abstract base class template for a map
// ======================================================================

#ifndef Fw_MapBase_HPP
#define Fw_MapBase_HPP

#include "Fw/DataStructures/MapConstIterator.hpp"
#include "Fw/DataStructures/SizedContainer.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename K, typename V>
class MapBase : public SizedContainer {
  private:
    // ----------------------------------------------------------------------
    // Deleted elements
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    MapBase(const MapBase<K, V>&) = delete;

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    MapBase<K, V>& operator=(const MapBase<K, V>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a map const iterator
    using ConstIterator = MapConstIterator<K, V>;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    MapBase() : SizedContainer() {}

    //! Destructor
    virtual ~MapBase() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the begin value of the iterator
    //! \return The iterator
    virtual ConstIterator begin() const = 0;

    //! Copy data from another map
    void copyDataFrom(const MapBase<K, V>& map) {
        if (&map != this) {
            this->clear();
            const FwSizeType size = FW_MIN(map.getSize(), this->getCapacity());
            auto it = map.begin();
            for (FwSizeType i = 0; i < size; i++) {
                const auto status = this->insert(it->getKey(), it->getValue());
                FW_ASSERT(status == Success::SUCCESS, static_cast<FwAssertArgType>(status));
                it++;
            }
        }
    }

    //! Get the end value of the iterator
    //! \return The iterator
    virtual ConstIterator end() const = 0;

    //! Find the value associated with a key in the map
    //! SUCCESS if the item was found
    virtual Success find(const K& key,  //!< The key (input)
                         V& value       //!< The value (output)
    ) const = 0;

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
