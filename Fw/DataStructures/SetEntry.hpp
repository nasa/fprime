// ======================================================================
// \title  SetEntry
// \author bocchino
// \brief  An abstract class template representing an iterator for a set
// ======================================================================

#ifndef Fw_SetEntry_HPP
#define Fw_SetEntry_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename T>
class SetEntry {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    SetEntry(const SetEntry<T>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetEntry() {}

    //! Destructor
    virtual ~SetEntry() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    SetEntry<T>& operator=(const SetEntry<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the element associated with this iterator
    //! \return The element
    virtual const T& getElement() const = 0;

    //! Get the next set iterator
    //! \return The set iterator, or nullptr if none
    virtual const SetEntry<T>* getNextSetEntry() const = 0;
};

}  // namespace Fw

#endif
