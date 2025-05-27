// ======================================================================
// \title  Array
// \author bocchino
// \brief  An array that stores its size and provides bounds checking
// ======================================================================

#ifndef Ds_Array_HPP
#define Ds_Array_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <initializer_list>

#include "Fw/Types/Assert.hpp"

namespace Ds {

template <typename T, FwSizeType S>
class Array final {
    static_assert(S > 0, "array size must be greater than zero");

  public:
    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    Array() {}

    //! Initializer list constructor
    Array(const std::initializer_list<T>& il) {
        FW_ASSERT(il.size() == S, static_cast<FwAssertArgType>(il.size()), static_cast<FwAssertArgType>(S));
        // TODO
    }

#if 0
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

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------
#endif

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array elements
    T m_elements[S] = {};
};

}  // namespace Ds

#endif
