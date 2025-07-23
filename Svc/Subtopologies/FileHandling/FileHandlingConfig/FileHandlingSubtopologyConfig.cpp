#include "FileHandlingSubtopologyConfig.hpp"

namespace FileHandling {
    namespace Allocation{ 
        //This instance can be changed to use a different allocator in the FileHandling Subtopology 
        Fw::MallocAllocator mallocatorInstance;
        Fw::MemAllocator& memAllocator = mallocatorInstance;
    }
} 