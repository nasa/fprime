// ======================================================================
// \title  StackBase
// \author bocchino
// \brief  An abstract base class template for a stack
// ======================================================================

#ifndef Fw_StackBase_HPP
#define Fw_StackBase_HPP

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"

namespace Fw {

template <typename T>
class StackBase {
  private:
    // ----------------------------------------------------------------------
    // Private constructors
    // ----------------------------------------------------------------------

    //! Copy constructor deleted in the base class
    //! Behavior depends on the implementation
    StackBase(const StackBase<T>&) = delete;

  protected:
    // ----------------------------------------------------------------------
    // Protected constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    StackBase() = default;

    //! Destructor
    virtual ~StackBase() = default;

  private:
    // ----------------------------------------------------------------------
    // Private member functions
    // ----------------------------------------------------------------------

    //! operator= deleted in the base class
    //! Behavior depends on the implementation
    //! We avoid virtual user-defined operators
    StackBase<T>& operator=(const StackBase<T>&) = delete;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Clear the stack
    virtual void clear() = 0;

    //! Get an item at an index.
    //! Indices go from left to right in the stack.
    //! Fails an assertion if the index is out of range.
    //! \return The item
    virtual const T& at(FwSizeType index  //!< The index
    ) const = 0;

    //! Copy data from another stack
    void copyDataFrom(const StackBase<T>& stack  //!< The stack
    ) {
        if (&stack != this) {
            this->clear();
            const FwSizeType size = FW_MIN(stack.getSize(), this->getCapacity());
            for (FwSizeType i = 0; i < size; i++) {
                const auto& e = stack.at(size - 1 - i);
                const auto status = this->push(e);
                FW_ASSERT(status == Fw::Success::SUCCESS, static_cast<FwAssertArgType>(status));
            }
        }
    }

    //! Push an item (add to the right)
    //! \return SUCCESS if item pushd
    virtual Success push(const T& e  //!< The item (output)
                         ) = 0;

    //! Peek an item at an index
    //! Indices go from left to right in the range [0, size)
    //! \return SUCCESS if item exists
    Success peek(T& e,                 //!< The item (output)
                 FwSizeType index = 0  //!< The index (input)
    ) const {
        auto status = Success::FAILURE;
        if (index < this->getSize()) {
            e = this->at(index);
            status = Success::SUCCESS;
        }
        return status;
    }

    //! Pop an item (remove from the right)
    //! \return SUCCESS if item popped
    virtual Success pop(T& e  //!< The item (output)
                        ) = 0;

    //! Get the size (number of items stored in the stack)
    //! \return The size
    virtual FwSizeType getSize() const = 0;

    //! Get the capacity (maximum number of items stored in the stack)
    //! \return The capacity
    virtual FwSizeType getCapacity() const = 0;
};

}  // namespace Fw

#endif
