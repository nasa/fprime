// ======================================================================
// \file   Array.hpp
// \author bocchino
// \brief  A statically-sized, bounds checked array
// ======================================================================

#ifndef Fw_Array_HPP
#define Fw_Array_HPP

#include "Fw/DataStructures/ExternalArray.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

namespace Fw {

template <typename T, FwSizeType S>
class Array final {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    static_assert(S > 0, "array size must be greater than zero");

  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type of the elements array
    using Elements = T[S];

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    Array() {}

    //! Array constructor
    //! Use a template to enforce exact array size
    //! Otherwise C++ may implicitly convert a smaller array to an array of size S
    template <FwSizeType S1>
    Array(const T (&elements)[S1]  //!< The array
    ) {
        static_assert(S1 == S, "array size must match");
        *this = elements;
    }

    //! Single-element constructor
    explicit Array(const T& element  //!< The element
    ) {
        for (FwSizeType i = 0; i < S; i++) {
            this->m_elements[i] = element;
        }
    }

    //! Copy constructor
    Array(const Array<T, S>& a  //!< The array to copy
    ) {
        for (FwSizeType i = 0; i < S; i++) {
            this->m_elements[i] = a.m_elements[i];
        }
    }

    //! Destructor
    ~Array() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Subscript operator
    //! \return The element at index i
    T& operator[](FwSizeType i  //!< The subscript index
    ) {
        FW_ASSERT(i < S, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Const subscript operator
    //! \return The element at index i
    const T& operator[](FwSizeType i  //!< The subscript index
    ) const {
        FW_ASSERT(i < S, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Copy assignment operator
    //! \return *this
    Array<T, S>& operator=(const Array<T, S>& a) {
        if (&a != this) {
            for (FwSizeType i = 0; i < S; i++) {
                this->m_elements[i] = a.m_elements[i];
            }
        }
        return *this;
    }

    //! Array assignment operator
    //! Use a template to enforce exact array size
    //! Otherwise C++ may implicitly convert a smaller array to an array of size S
    //! \return *this
    template <FwSizeType S1>
    Array<T, S>& operator=(const T (&elements)[S1]  //!< The elements
    ) {
        static_assert(S1 == S, "array size must match");
        for (FwSizeType i = 0; i < S; i++) {
            this->m_elements[i] = elements[i];
        }
        return *this;
    }

    //! Get a mutable reference to the elements
    //! \return A mutable reference to the elements
    Elements& getElements() { return this->m_elements; }

    //! Get a const reference to the elements
    //! \return A const reference to the elements
    const Elements& getElements() const { return this->m_elements; }

    //! Convert this array to an ExternalArray
    // \return The ExternalArray
    ExternalArray<T> asExternalArray() { return ExternalArray<T>(this->m_elements, S); }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array elements
    Elements m_elements = {};
};

}  // namespace Fw

#endif
