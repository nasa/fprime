// ======================================================================
// \title  SetIterator
// \author bocchino
// \brief  An abstract class template representing an iterator for a set
// ======================================================================

#ifndef Fw_SetIterator_HPP
#define Fw_SetIterator_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename T>
class SetIterator {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    SetIterator(const SetIterator<T>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetIterator() = default;

    //! Destructor
    virtual ~SetIterator() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    SetIterator<T>& operator=(const SetIterator<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the element associated with this iterator
    //! \return The element
    virtual const T& getElement() const = 0;

    //! Get the next set iterator
    //! \return The set iterator, or nullptr if none
    virtual const SetIterator<T>* getNextSetIterator() const = 0;
};

}  // namespace Fw

#endif
