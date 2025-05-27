// ======================================================================
// \title  Array
// \author bocchino
// \brief  An array that stores its size and provides bounds checking
// ======================================================================

#ifndef Ds_Array_HPP
#define Ds_Array_HPP

#include <FpConfig.hpp>

#include "Fw/Types/Assert.hpp"

namespace Ds {

template <typename T>
class Array {
  public:
    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Construct an Array object
    Array() {}

    //! Construct an Array object with backing storage
    Array(T* const elements,     //!< The array elements
          const FwSizeType size  //!< The size
          )
        : m_elements(elements), m_size(size) {}

  public:
    // ----------------------------------------------------------------------
    // Public operators
    // ----------------------------------------------------------------------

    //! Subscript operator
    T& operator[](const FwSizeType i  //!< The subscript index
    ) {
        FW_ASSERT(i < this->m_size, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Const subscript operator
    const T& operator[](const FwSizeType i  //!< The subscript index
    ) const {
        FW_ASSERT(i < this->m_size, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Get the size
    //! \return The size
    FwSizeType getSize() const { return this->m_size; }

    //! Set the backing storage
    void setStorage(T* const elements,     //!< The array elements
                    const FwSizeType size  //!< The size
    ) {
        this->m_elements = elements;
        this->m_size = size;
    }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array elements
    T* m_elements = nullptr;

    //! The size
    FwSizeType m_size = 0;
};

}  // namespace Ds

#endif
