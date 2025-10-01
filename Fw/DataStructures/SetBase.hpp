// ======================================================================
// \title  SetBase
// \author bocchino
// \brief  An abstract base class template for a set
// ======================================================================

#ifndef Fw_SetBase_HPP
#define Fw_SetBase_HPP

#include "Fw/DataStructures/SetConstIterator.hpp"
#include "Fw/DataStructures/SizedContainer.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename T>
class SetBase : public SizedContainer {
  private:
    // ----------------------------------------------------------------------
    // Deleted elements
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    SetBase(const SetBase<T>&) = delete;

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    SetBase<T>& operator=(const SetBase<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public types
    // ----------------------------------------------------------------------

    //! The type of a set const iterator
    using ConstIterator = SetConstIterator<T>;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetBase() : SizedContainer() {}

    //! Destructor
    virtual ~SetBase() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the begin iterator
    //! \return The iterator
    virtual ConstIterator begin() const = 0;

    //! Get the end iterator
    //! \return The iterator
    virtual ConstIterator end() const = 0;

    //! Copy data from another set
    void copyDataFrom(const SetBase<T>& set) {
        if (&set != this) {
            this->clear();
            const FwSizeType size = FW_MIN(set.getSize(), this->getCapacity());
            auto it = set.begin();
            for (FwSizeType i = 0; i < size; i++) {
                const auto status = this->insert(*it);
                FW_ASSERT(status == Success::SUCCESS, static_cast<FwAssertArgType>(status));
                it++;
            }
        }
    }

    //! Find an element in a set
    //! SUCCESS if the item was found
    virtual Success find(const T& element  //!< The element
    ) const = 0;

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
