// ======================================================================
// \title  SetOrMapImplConstIterator
// \author bocchino
// \brief  A class template representing a const iterator for a set or map implementation
// ======================================================================

#ifndef Fw_SetOrMapImplConstIterator_HPP
#define Fw_SetOrMapImplConstIterator_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename KE, typename VN>
class SetOrMapImplConstIterator {

    // ----------------------------------------------------------------------
    // Deleted elements
    // ----------------------------------------------------------------------

  private:
    //! Copy constructor
    SetOrMapImplConstIterator(const SetOrMapImplConstIterator<KE, VN>& it) = delete;

  private:
    //! Copy assignment operator
    SetOrMapImplConstIterator& operator=(const SetOrMapImplConstIterator<KE, VN>&) = delete;

    // ----------------------------------------------------------------------
    // Constructors and destructors
    // ----------------------------------------------------------------------

  public:
    //! Zero-argument constructor
    SetOrMapImplConstIterator() = default;

  public:
    //! Destructor
    virtual ~SetOrMapImplConstIterator() = default;

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

  public:
    //! Increment the iterator
    virtual void increment() = 0;

    //! Get the key or element in the entry pointed to by this iterator
    //! \return The key or element
    virtual const KE& getKeyOrElement() const = 0;

    //! Get the value in the entry pointed to by this iterator, or nil for a set
    //! \return The value or nil
    virtual const VN& getValueOrNil() const = 0;

    //! Check whether the iterator is in range
    //! \return True if the iterator is in range
    virtual bool isInRange() const = 0;

    //! Reset the iterator
    virtual void reset() = 0;

};

}  // namespace Fw

#endif
