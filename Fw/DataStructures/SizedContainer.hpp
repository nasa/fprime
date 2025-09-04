// ======================================================================
// \title  SizedContainer
// \author bocchino
// \brief  An abstract base class representing a sized container
// ======================================================================

#ifndef Fw_SizedContainer_HPP
#define Fw_SizedContainer_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

class SizedContainer {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    SizedContainer(const SizedContainer&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SizedContainer() {}

    //! Destructor
    virtual ~SizedContainer() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    SizedContainer& operator=(const SizedContainer&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Clear the container
    virtual void clear() = 0;

    //! Get the size (number of items stored in the container)
    //! \return The size
    virtual FwSizeType getSize() const = 0;

    //! Get the capacity (maximum number of items storable in the container)
    //! \return The capacity
    virtual FwSizeType getCapacity() const = 0;

    //! Check whether the container is empty
    //! \return True if the container is empty
    bool isEmpty() const { return this->getSize() == 0; }

    //! Check whether the container is full
    //! \return True if the container is full
    bool isFull() const { return this->getSize() >= this->getCapacity(); }
};

}  // namespace Fw

#endif
