#include "ComFprimeSubtopologyConfig.hpp"

namespace ComFprime {
namespace Allocation {
// This instance can be changed to use a different allocator in the ComFprime Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace ComFprime
