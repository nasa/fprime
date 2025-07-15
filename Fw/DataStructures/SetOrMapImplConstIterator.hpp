// ======================================================================
// \title  SetOrMapImplConstIterator
// \author bocchino
// \brief  A class template representing a const iterator for a set or map implementation
// ======================================================================

#ifndef Fw_SetOrMapImplConstIterator_HPP
#define Fw_SetOrMapImplConstIterator_HPP

#include "Fw/DataStructures/SetOrMapImplEntry.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Fw {

template <typename KE, typename VN>
class SetOrMapImplConstIterator {
  private:
    // ----------------------------------------------------------------------
    // Deleted elements
    // ----------------------------------------------------------------------

    //! Copy constructor
    SetOrMapImplConstIterator(const SetOrMapImplConstIterator<KE, VN>& it) = delete;

    //! Copy assignment operator
    SetOrMapImplConstIterator& operator=(const SetOrMapImplConstIterator<KE, VN>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The kind of a const iterator implementation
    enum class ImplKind { ARRAY, RED_BLACK_TREE };

  public:
    // ----------------------------------------------------------------------
    // Constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    SetOrMapImplConstIterator() = default;

    //! Destructor
    virtual ~SetOrMapImplConstIterator() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Return the impl kind
    //! \return The impl kind
    virtual ImplKind implKind() const = 0;

    //! Increment the iterator
    virtual void increment() = 0;

    //! Check whether the iterator is in range
    //! \return True if the iterator is in range
    virtual bool isInRange() const = 0;

    //! Get the set or map impl entry pointed to by this iterator
    //! \return The set or map impl entry
    virtual const SetOrMapImplEntry<KE, VN>& getEntry() const = 0;
};

}  // namespace Fw

#endif
