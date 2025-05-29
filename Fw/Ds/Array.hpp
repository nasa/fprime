// ======================================================================
// \title  Array
// \author bocchino
// \brief  A statically-sized, bounds checked array
// ======================================================================

#ifndef Ds_Array_HPP
#define Ds_Array_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <initializer_list>

#include "Fw/Ds/ExternalArray.hpp"
#include "Fw/Types/Assert.hpp"

namespace Ds {

template <typename T, FwSizeType S>
class Array final {
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

    //! Initializer list constructor
    Array(const std::initializer_list<T>& il  //!< The initializer list
    ) {
        FW_ASSERT(il.size() == S, static_cast<FwAssertArgType>(il.size()), static_cast<FwAssertArgType>(S));
        FwSizeType i = 0;
        for (const auto& e : il) {
            FW_ASSERT(i < S);
            this->m_elements[i] = e;
            i++;
        }
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

    //! Get a mutable reference to the elements
    //! \return A mutable reference to the elements
    Elements& getElements() { return this->m_elements; }

    //! Get a const reference to the elements
    //! \return A const reference to the elements
    const Elements& getElements() const { return this->m_elements; }

    //! Convert this array to an ExternalArray
    // \return The ExternalArray
    ExternalArray<T> asExternalArray() { return ExternalArray<T>(this->m_elements, S); }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the array size
    //! \return The size
    static constexpr FwSizeType getSize() { return S; }

  private:
    // ----------------------------------------------------------------------
    // Private member variables
    // ----------------------------------------------------------------------

    //! The array elements
    Elements m_elements = {};
};

}  // namespace Ds

#endif
