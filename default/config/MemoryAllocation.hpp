// ======================================================================
// \title  config/MemoryAllocation.hpp
// \author lestarch
// \brief  hpp file for memory allocation configuration
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#ifndef CONFIG_MEMORY_ALLOCATION_HPP
#define CONFIG_MEMORY_ALLOCATION_HPP
#include <Fw/Types/MallocAllocator.hpp>
namespace Fw {
namespace MemoryAllocation {
using DefaultMemoryAllocatorType = Fw::MallocAllocator;
}  // namespace MemoryAllocation
}  // namespace Fw

#endif  // CONFIG_MEMORY_ALLOCATION_HPP
