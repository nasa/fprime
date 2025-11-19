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
#ifndef FW_TYPES_LANGUAGEHELPERS_HPP_
#define FW_TYPES_LANGUAGEHELPERS_HPP_
#include <Fw/ByteArray.hpp>
#include <type_traits>

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
    static_assert(std::is_trivially_constructible<T>::value, "Cannot use arrayPlacementNew for non-trivially constructible types");
    void* base_pointer = reinterpret_cast<void*>(array.bytes);
    FW_ASSERT(base_pointer != nullptr); // Confirm non-null
    FW_ASSERT((array.bytes % alignof(T)) == 0); // Confirm alignment
    FW_ASSERT(array.size >= (sizeof(T) * arraySize)); // Confirm size
    T* type_pointer = static_cast<T*>(base_pointer);
    for (FwSizeType index = 0; index < arraySize; index++) {
        new (&type_pointer[index]) T();
    }
    return type_pointer;
}
#endif