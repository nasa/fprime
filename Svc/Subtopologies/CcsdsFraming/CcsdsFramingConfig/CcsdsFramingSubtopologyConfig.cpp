#include "CcsdsFramingSubtopologyConfig.hpp"

namespace CcsdsFraming {
namespace Allocation {
// This instance can be changed to use a different allocator in the CcsdsFraming Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace CcsdsFraming
