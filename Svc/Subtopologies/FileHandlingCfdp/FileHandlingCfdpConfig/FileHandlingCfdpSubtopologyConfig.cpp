#include "FileHandlingCfdpSubtopologyConfig.hpp"

namespace FileHandlingCfdp {
namespace Allocation {
// This instance can be changed to use a different allocator in the FileHandlingCfdp Subtopology
Fw::MallocAllocator mallocatorInstance;
Fw::MemAllocator& memAllocator = mallocatorInstance;
}  // namespace Allocation
}  // namespace FileHandlingCfdp
