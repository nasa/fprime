// ======================================================================
// \title  SetConstEntry
// \author bocchino
// \brief  An abstract class template representing a const entry for a set
// ======================================================================

#ifndef Fw_SetConstEntry_HPP
#define Fw_SetConstEntry_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename T>
class SetConstEntry {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    SetConstEntry(const SetConstEntry<T>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetConstEntry() {}

    //! Destructor
    virtual ~SetConstEntry() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    SetConstEntry<T>& operator=(const SetConstEntry<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the element associated with this entry
    //! \return The element
    virtual const T& getElement() const = 0;

    //! Get the next set entry
    //! \return The set entry, or nullptr if none
    virtual const SetConstEntry<T>* getNextSetConstEntry() const = 0;
};

}  // namespace Fw

#endif
