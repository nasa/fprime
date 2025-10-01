#include "DataProductsSubtopologyConfig.hpp"

namespace DataProducts {
namespace Allocation {
// This instance can be changed to use a different allocator in the DataProducts Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace DataProducts
