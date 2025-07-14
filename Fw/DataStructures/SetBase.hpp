// ======================================================================
// \title  SetBase
// \author bocchino
// \brief  An abstract base class template for a set
// ======================================================================

#ifndef Fw_SetBase_HPP
#define Fw_SetBase_HPP

#include "Fw/DataStructures/SetConstEntry.hpp"
#include "Fw/DataStructures/SetConstIterator.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename T>
class SetBase {
  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a set entry
    using SetConstEntry = SetConstEntry<T>;

  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    SetBase(const SetBase<T>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetBase() {}

    //! Destructor
    virtual ~SetBase() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    SetBase<T>& operator=(const SetBase<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Clear the set
    virtual void clear() = 0;

    //! Copy data from another set
    void copyDataFrom(const SetBase<T>& set) {
        if (&set != this) {
            this->clear();
            const auto* e = set.getHeadSetConstEntry();
            const FwSizeType size = FW_MIN(set.getSize(), this->getCapacity());
            for (FwSizeType i = 0; i < size; i++) {
                FW_ASSERT(e != nullptr);
                const auto status = this->insert(e->getElement());
                FW_ASSERT(status == Success::SUCCESS, static_cast<FwAssertArgType>(status));
                e = e->getNextSetConstEntry();
            }
        }
    }

    //! Find the an element in a set
    //! SUCCESS if the item was found
    virtual Success find(const T& element  //!< The element
    ) const = 0;

    //! Get the capacity (maximum number of items stored in the set)
    //! \return The capacity
    virtual FwSizeType getCapacity() const = 0;

    //! Get the head set entry for the set
    //! \return The set entry
    virtual const SetConstEntry* getHeadSetConstEntry() const = 0;

    //! Get the size (number of items stored in the set)
    //! \return The size
    virtual FwSizeType getSize() const = 0;

    //! Insert an element in the set
    //! \return SUCCESS if there is room in the set
    virtual Success insert(const T& element  //!< The element
                           ) = 0;

    //! Remove an element from the set
    //! \return SUCCESS if the element was there
    virtual Success remove(const T& element  //!< The element
                           ) = 0;
};

}  // namespace Fw

#endif
