// ======================================================================
// \file   ExternalArray.hpp
// \author bocchino
// \brief  A bounds-checked array with external memory
// ======================================================================

#ifndef Fw_ExternalArray_HPP
#define Fw_ExternalArray_HPP

#include <cstdint>
#include <new>
#include <type_traits>

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ByteArray.hpp"

namespace Fw {

template <typename T>
class ExternalArray final {
    // ----------------------------------------------------------------------
    // Static assertions
    // ----------------------------------------------------------------------

    static_assert(std::is_assignable<T&, T>::value, "T must be assignable to T&");

  public:
    // ----------------------------------------------------------------------
    // Public constructors and destructors
    // ----------------------------------------------------------------------

    //! Zero-argument constructor
    ExternalArray() {}

    //! Constructor providing typed backing storage.
    //! elements must point to at least size elements of type T.
    ExternalArray(T* elements,     //!< The elements
                  FwSizeType size  //!< The array size
                  )
        : m_elements(elements), m_size(size) {}

    //! Constructor providing untyped backing storage.
    //! data must be aligned according to getByteArrayAlignment().
    //! data must contain at least getByteArraySize(size) bytes.
    ExternalArray(ByteArray data,  //!< The data
                  FwSizeType size  //!< The array size
    ) {
        this->setStorage(data, size);
    }

    //! Copy constructor
    ExternalArray(const ExternalArray<T>& a) : m_elements(a.m_elements), m_size(a.m_size) {}

    //! Destructor
    ~ExternalArray() = default;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Subscript operator
    //! \return The element at index i
    T& operator[](const FwSizeType i  //!< The subscript index
    ) {
        FW_ASSERT(this->m_elements != nullptr);
        FW_ASSERT(i < this->m_size, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Const subscript operator
    //! \return The element at index i
    const T& operator[](const FwSizeType i  //!< The subscript index
    ) const {
        FW_ASSERT(this->m_elements != nullptr);
        FW_ASSERT(i < this->m_size, static_cast<FwAssertArgType>(i));
        return this->m_elements[i];
    }

    //! Copy assignment operator
    //! \return *this
    ExternalArray<T>& operator=(const ExternalArray<T>& a) {
        if (&a != this) {
            this->m_elements = a.m_elements;
            this->m_size = a.m_size;
        }
        return *this;
    }

    //! Copy the data from a
    void copyDataFrom(const ExternalArray<T>& a) {
        const FwSizeType size = FW_MIN(this->m_size, a.m_size);
        for (FwSizeType i = 0; i < size; i++) {
            (*this)[i] = a[i];
        }
    }

    //! Get a mutable pointer to the elements
    //! \return A mutable pointer to the elements
    T* getElements() { return this->m_elements; }

    //! Get a const pointer to the elements
    //! \return A const pointer to the elements
    const T* getElements() const { return this->m_elements; }

    //! Get the size
    //! \return The size
    FwSizeType getSize() const { return this->m_size; }

    //! Set the backing storage (typed data)
    void setStorage(T* elements,     //!< The array elements
                    FwSizeType size  //!< The size
    ) {
        this->m_elements = elements;
        this->m_size = size;
    }

    //! Set the backing storage (untyped data)
    //! Data must be aligned for T and must contain at least getByteArraySize(size) bytes.
    void setStorage(ByteArray data,  //!< The data
                    FwSizeType size  //!< The array size
    ) {
        // Check that data.bytes is not null
        FW_ASSERT(data.bytes != nullptr);
        // Check that data.bytes is properly aligned
        FW_ASSERT(reinterpret_cast<uintptr_t>(data.bytes) % alignof(T) == 0);
        // Check that data.size is large enough to hold the array
        FW_ASSERT(size * sizeof(T) <= data.size);
        // Initialize the array members
        this->m_elements = reinterpret_cast<T*>(data.bytes);
        // Construct the array members in place
        // This step ensures that each array element holds a valid object
        // into which we can assign data
        for (FwSizeType i = 0; i < size; i++) {
            // This code trips an alignment check in clang-tidy
            // However the alignment has been checked by FW_ASSERT above
            (void)new (&this->m_elements[i]) T();  // NOLINT
        }
        // Set the size
        this->m_size = size;
    }

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Get the alignment of the storage for an ExternalArray
    //! \return The alignment
    static constexpr U8 getByteArrayAlignment() { return alignof(T); }

    //! Get the size of the storage for an ExternalArray of the specified size,
    //! as a byte array
    //! \return The byte array size
    static constexpr FwSizeType getByteArraySize(FwSizeType size  //!< The size
    ) {
        return size * sizeof(T);
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

}  // namespace Fw

#endif
