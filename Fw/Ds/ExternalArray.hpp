// ======================================================================
// \title  Array
// \author bocchino
// \brief  An array that stores its size and provides bounds checking
// ======================================================================

#ifndef Ds_Array_HPP
#define Ds_Array_HPP

#include <FpConfig.hpp>

#include "Fw/Types/Assert.hpp"
#include "Types/PointerAssert.hpp"

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

  public:
    // ----------------------------------------------------------------------
    // Public static functions
    // ----------------------------------------------------------------------

    //! Allocate storage for an array with element type T
    //! \return The storage
    static T* allocateStorage(Fw::MemAllocator& memAllocator,  //!< The mem allocator (input)
                              FwEnumStoreType memId,           //!< The memory segment identifier (input)
                              FwSizeType numElts,              //!< The number of array elements (input)
                              bool& recoverable  //!< Whether the memory should be recoverable (input and output)
    ) {
        const NATIVE_UINT_TYPE requestedSize = static_cast<NATIVE_UINT_TYPE>(numElts * sizeof(T));
        NATIVE_UINT_TYPE allocatedSize = requestedSize;
        void* memory = memAllocator.allocate(static_cast<NATIVE_UINT_TYPE>(memId), allocatedSize, recoverable);
        FW_ASSERT((memory != nullptr) && (allocatedSize == requestedSize), static_cast<FwAssertArgType>(memId),
                  Ds_PTR_ASSERT_ARGS(memory), static_cast<FwAssertArgType>(requestedSize),
                  static_cast<FwAssertArgType>(allocatedSize));
        return static_cast<T*>(memory);
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
