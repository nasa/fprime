#include "DpCompressionSubtopologyConfig.hpp"

namespace DpCompression {
namespace Allocation {
// This instance can be changed to use a different allocator in the DataProducts Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace DpCompression
