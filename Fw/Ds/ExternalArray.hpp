// ======================================================================
// \title  ExternalArray
// \author bocchino
// \brief  A bounds-checked array with external memory
// ======================================================================

#ifndef Ds_ExternalArray_HPP
#define Ds_ExternalArray_HPP

#include <Fw/FPrimeBasicTypes.hpp>

#include "Fw/Types/Assert.hpp"

namespace Ds {

template <typename T>
class ExternalArray final {
  public:
    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalArray() {}

    //! Constructor providing backing storage
    ExternalArray(T* elements,     //!< The elements
                  FwSizeType size  //!< The array size
                  )
        : m_elements(elements), m_size(size) {}

    //! Copy constructor
    ExternalArray(const ExternalArray<T>& a) : m_elements(a.m_elements), m_size(a.m_size) {}

    //! Destructor
    ~ExternalArray() {}

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Subscript operator
    T& operator[](const FwSizeType i  //!< The subscript index
    ) {
        FW_ASSERT(i < this->m_elements != nullptr);
        FW_ASSERT(i < this->m_size, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Const subscript operator
    const T& operator[](const FwSizeType i  //!< The subscript index
    ) const {
        FW_ASSERT(i < this->m_elements != nullptr);
        FW_ASSERT(i < this->m_size, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Copy assignment operator
    ExternalArray<T>& operator=(const ExternalArray<T>& a) {
        if (&a != this) {
            this->m_elements = a.m_elements;
            this->m_size = a.m_size;
        }
        return *this;
    }

    //! Get a mutable pointer to the elements
    T* getElements() { return this->m_elements; }

    //! Get a const pointer to the elements
    const T* getElements() const { return this->m_elements; }

    //! Get the size
    //! \return The size
    FwSizeType getSize() const { return this->m_size; }

    //! Set the backing storage
    void setStorage(T* elements,     //!< The array elements
                    FwSizeType size  //!< The size
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
