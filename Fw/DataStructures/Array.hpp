// ======================================================================
// \file   Array.hpp
// \author bocchino
// \brief  A statically-sized, bounds checked array
// ======================================================================

#ifndef Fw_Array_HPP
#define Fw_Array_HPP

#include <initializer_list>

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
    Array() = default;

    //! Initializer list constructor
    Array(const std::initializer_list<T>& il  //!< The initializer list
    ) {
        *this = il;
    }

    //! Primitive array constructor
    Array(const Elements& elements  //!< The array elements
    ) {
        *this = elements;
    }

    //! Single-element constructor
    explicit Array(const T& element  //!< The element
    ) {
        *this = element;
    }

    //! Copy constructor
    Array(const Array<T, S>& a  //!< The array to copy
    ) {
        *this = a;
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

    //! operator= (initializer list)
    //! \return *this
    Array<T, S>& operator=(const std::initializer_list<T>& il  //!< The initializer list
    ) {
        // Since we are required to use C++11, this has to be a runtime check
        // In C++14, it can be a static check
        FW_ASSERT(il.size() == S, static_cast<FwAssertArgType>(il.size()), static_cast<FwAssertArgType>(S));
        FwSizeType i = 0;
        for (const auto& e : il) {
            FW_ASSERT(i < S, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(S));
            this->m_elements[i] = e;
            i++;
        }
        return *this;
    }

    //! operator= (primitive array)
    //! \return *this
    Array<T, S>& operator=(const Elements& elements  //!< The array elements
    ) {
        for (FwSizeType i = 0; i < S; i++) {
            this->m_elements[i] = elements[i];
        }
        return *this;
    }

    //! operator= (single element)
    Array<T, S>& operator=(const T& element  //!< The element
    ) {
        for (FwSizeType i = 0; i < S; i++) {
            this->m_elements[i] = element;
        }
        return *this;
    }

    //! operator= (copy assignment)
    //! \return *this
    Array<T, S>& operator=(const Array<T, S>& a) {
        if (&a != this) {
            for (FwSizeType i = 0; i < S; i++) {
                this->m_elements[i] = a.m_elements[i];
            }
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
