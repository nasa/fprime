// ======================================================================
// \title  LanguageHelpers.hpp
// \author lestarch
// \brief  hpp file for C++ language helper functions
//
// \copyright
// Copyright (C) 2025 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef FW_TYPES_LANGUAGE_HELPERS_HPP_
#define FW_TYPES_LANGUAGE_HELPERS_HPP_
#include <new>
#include <type_traits>
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ByteArray.hpp"
namespace Fw {
//! \brief placement new for arrays
//!
//! C++ as a language does not guaranteed that placement new for a C++ array of length N will fit within a memory
//! region of size N *sizeof(T). Moreover, there are some compilers whose implementation of placement new for arrays
//! do not guarantee this property.
//!
//! This function provides a helper for placement new for arrays that guarantees that the array will fit within the
//! provided memory region. It checks that the provided memory region is large enough to hold the array (N * sizeof(T)
//! and that the alignment of the provided memory region is sufficient for the type T. It also checks that the provided
//! memory region is non-null.
//!
//! \warning this function cannot be used for arrays of arrays (i.e. T cannot be an array type).
//!
//! \tparam T the type of the array elements
//! \param array the byte array to use for placement new (pair of bytes pointer and size)
//! \param arraySize the number of elements in the array
//! \return a pointer to the array of type T
template <typename T>
T* arrayPlacementNew(Fw::ByteArray array, FwSizeType arraySize) {
    static_assert(!std::is_array<T>::value, "Cannot use arrayPlacementNew new for arrays of arrays");
    static_assert(std::is_constructible<T>::value,
                  "Cannot use arrayPlacementNew on types without a default zero-argument constructor");
    void* base_pointer = reinterpret_cast<void*>(array.bytes);
    FW_ASSERT(base_pointer != nullptr);
    FW_ASSERT((reinterpret_cast<PlatformPointerCastType>(base_pointer) % alignof(T)) == 0);
    FW_ASSERT(array.size >= (sizeof(T) * arraySize));
    T* type_pointer = static_cast<T*>(base_pointer);
    for (FwSizeType index = 0; index < arraySize; index++) {
        new (&type_pointer[index]) T();
    }
    return type_pointer;
}

//! \brief placement delete for arrays
//!
//! This is the partner of tha above function that performs the destructor operation on every element of type T in the
//! array. This assumes that all elements have been constructed.
//!
//! \warning this function cannot be used for arrays of arrays (i.e. T cannot be an array type).
//!
//! \tparam T the type of the array elements
//! \param arrayPointer pointer to an array of type T
//! \param arraySize the number of elements in the array
template <typename T>
void arrayPlacementDestruct(T* arrayPointer, FwSizeType arraySize) {
    static_assert(!std::is_array<T>::value, "Cannot use arrayPlacementDestruct new for arrays of arrays");
    FW_ASSERT(arrayPointer != nullptr);
    for (FwSizeType index = 0; index < arraySize; index++) {
        arrayPointer[index].~T();
    }
}
}  // namespace Fw
#endif  // FW_TYPES_LANGUAGE_HELPERS_HPP_
