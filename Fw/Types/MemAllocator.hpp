/**
 * \file
 * \author T. Canham
 * \brief Defines a base class for a memory allocator for classes.
 *
 * A memory allocator is a class that provides memory for a component.
 * This allows a user of the class to allocate memory as they choose.
 * The user writes derived classes for each of the allocator types.
 *
 * \copyright
 * Copyright 2009-2020, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */
#ifndef TYPES_MEMALLOCATOR_HPP_
#define TYPES_MEMALLOCATOR_HPP_

#include <Fw/FPrimeBasicTypes.hpp>
#include <config/MemoryAllocatorTypeEnumAc.hpp>
#include <cstddef>

namespace Fw {

//! \brief Memory Allocation base class
//!
//! This class is a pure virtual base class for memory allocators in F Prime. The intent is to provide derived classes
//! that allocate memory from different sources. The base class can be passed to classes so the allocator can be
//! selected at the system level, and different allocators can be used by different components as appropriate.
//!
//! The identifier can be used to look up a pre-allocated buffer by ID in an embedded system. There is no guarantee
//! that an identifier is unique across multiple calls to allocate(). It is intended to be unique to a given entity.
//!
//! \warning implementors providing derived classes that tie an identifier to a specific memory segment should use
//! bump pointer or other implementation to handle multiple calls to allocate() with the same identifier.
//!
//! \warning alignment must be respected in allocation calls, but may be larger than requested.
//!
//! The size is the requested size of the memory. If the allocator cannot return the requested amount, it should return
//! the actual amount and users should check.
//!
//! The recoverable flag is intended to be used in embedded environments where memory can survive a processor reset and
//! data can be recovered. The component using the allocator can then use the data. Any integrity checks are up to the
//! user of the memory.
//!
class MemAllocator {
  public:
    //! Allocate memory
    //!
    //! Allows allocation of memory of a given size and alignment. The actual returned memory size may be smaller than
    //! the requested size. The alignment of the memory is guaranteed to be at least as large as the requested
    //! alignment but may be larger. The recoverable flag indicates if the memory is recoverable (i.e. non-volatile)
    //! and thus may be valid without initialization.
    //!
    //! identifier is a unique identifier for the allocating entity. This entity (e.g. a component) may call allocate
    //! multiple times with the same id, but no other entity in the system shall call allocate with that id.
    //!
    //! \param identifier - a unique identifier for the allocating entity
    //! \param size the requested size - changed to actual if different
    //! \param recoverable - flag to indicate the memory could be recoverable
    //! \param alignment - alignment requirement for the allocation. Default: maximum alignment defined by C++.
    //! \return the pointer to memory. Zero if unable to allocate
    virtual void* allocate(const FwEnumStoreType identifier,
                           FwSizeType& size,
                           bool& recoverable,
                           FwSizeType alignment = alignof(std::max_align_t)) = 0;

    //! Deallocate memory
    //!
    //! Deallocate memory previously allocated by allocate(). The pointer must be one returned by allocate() and the
    //! identifier must match the one used in the original allocate() call.
    //!
    //! \param identifier - a unique identifier for the allocating entity, must match the call to allocate()
    //! \param ptr the pointer to memory returned by allocate()
    virtual void deallocate(const FwEnumStoreType identifier, void* ptr) = 0;

    //! Allocate memory without recoverable flag
    //!
    //! This is a convenience method that calls allocate() without the recoverable flag. The recoverable flag is filled
    //! by the underlying allocator but is not returned to the caller. This is for cases when the caller does not care
    //! about recoverability of memory.
    //!
    //! \param identifier - a unique identifier for the allocating entity
    //! \param size the requested size - changed to actual if different
    //! \param alignment - alignment requirement for the allocation. Default: maximum alignment defined by C++.
    //! \return the pointer to memory. Zero if unable to allocate
    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   FwSizeType alignment = alignof(std::max_align_t));

    //! Allocate memory checking that the allocation was successful
    //!
    //! This is a convenience method that calls allocate() and checks that the returned pointer is not null and that
    //! size is at least as large as the requested size.
    //!
    //! Allocations are checked using FW_ASSERT implying that an allocation failure results in a tripped assertion.
    //!
    //! \param identifier - a unique identifier for the allocating entity
    //! \param size the requested size, actual allocation will be at least this size
    //! \param recoverable - flag to indicate the memory could be recoverable
    //! \param alignment - alignment requirement for the allocation. Default: maximum alignment defined by C++.
    //! \return the pointer to memory. Zero if unable to allocate
    void* checkedAllocate(const FwEnumStoreType identifier,
                          FwSizeType& size,
                          bool& recoverable,
                          FwSizeType alignment = alignof(std::max_align_t));

    //! Allocate memory checking that the allocation was successful without recoverable flag
    //!
    //! This is a convenience method that calls allocate() and checks that the returned pointer is not null and that
    //! size is at least as large as the requested size. The recoverable flag is filled by the underlying allocator
    //! but is not returned to the caller. This is for cases when the caller does not care about recoverability of
    //! memory.
    //!
    //! Allocations are checked using FW_ASSERT implying that an allocation failure results in a tripped assertion.
    //!
    //! \param identifier - a unique identifier for the allocating entity
    //! \param size the requested size, actual allocation will be at least this size
    //! \param alignment - alignment requirement for the allocation. Default: maximum alignment defined by C++.
    //! \return the pointer to memory. Zero if unable to allocate
    void* checkedAllocate(const FwEnumStoreType identifier,
                          FwSizeType& size,
                          FwSizeType alignment = alignof(std::max_align_t));

  protected:
    MemAllocator();
    virtual ~MemAllocator();

  private:
    MemAllocator(MemAllocator&);  //!< disable
    MemAllocator(MemAllocator*);  //!< disable
};

class MemAllocatorRegistry {
  private:
    // Constructor which will register itself as the singleton
    MemAllocatorRegistry();
    ~MemAllocatorRegistry() = default;

  public:
    //! \brief get the singleton registry
    //!
    //! \return the singleton registry
    static MemAllocatorRegistry& getInstance();

    //! \brief register an allocator for the given type
    //!
    //! This will register an allocator for the given type. If the allocator is already registered it will overwrite.
    //!
    //! \warning allocator must remain valid for duration of the program.
    //!
    //! \param type the type of allocator
    //! \param allocator the allocator. The registry does not take ownership of the allocator.
    void registerAllocator(const MemoryAllocation::MemoryAllocatorType type, MemAllocator& allocator);

    //! Get an allocator for a type
    //!
    //! Return the memory allocator for the given type. It is an error to request an allocator for a type that has not
    //! been registered.
    //!
    //! \param type the type of allocator
    MemAllocator& getAllocator(const MemoryAllocation::MemoryAllocatorType type);

    //! Get an allocator for a type with default
    //!
    //! Return the memory allocator for the given type. If the type has not been registered, then return the allocator
    //! registered to MemoryAllocatorType::SYSTEM. It is an error if SYSTEM has not been registered.
    //!
    //! \param type the type of allocator
    MemAllocator& getAnAllocator(const MemoryAllocation::MemoryAllocatorType type);

  private:
    //! \brief get the default allocator
    //!
    //! Creates a single instance of the default allocator and returns a reference to it. This is done to ensure that
    //! the default allocator is only created once and is available when ill-ordered static initialization occurs.
    //!
    //! \return the default allocator
    static MemAllocator& getDefaultAllocator();

    //! Array of allocators for each type defaulted to nullptr
    MemAllocator* m_allocators[MemoryAllocation::MemoryAllocatorType::NUM_CONSTANTS] = {nullptr};
    MemAllocator& m_defaultAllocator;  //!< default allocator
};
} /* namespace Fw */

#endif /* TYPES_MEMALLOCATOR_HPP_ */
