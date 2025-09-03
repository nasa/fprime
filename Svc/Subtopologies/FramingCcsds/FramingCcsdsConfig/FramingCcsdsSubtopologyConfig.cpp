#include "FramingCcsdsSubtopologyConfig.hpp"

namespace FramingCcsds {
namespace Allocation {
// This instance can be changed to use a different allocator in the FramingCcsds Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace FramingCcsds
