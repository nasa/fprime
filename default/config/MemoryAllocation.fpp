module Fw {
module MemoryAllocation {
    @ Enumeration of the registered memory allocators provided by Fw::MemAllocatorRegistry. This allows system
    @ designers to enumerate the memory functions in their system and configure the Fw::MemAllocatorRegistry to delegate
    @ to the appropriate allocator. 
    enum MemoryAllocatorType : U8 {
        CUSTOM_ALLOCATOR_1 @< SAMPLE: sample allocator subsystem
        # Below this line are required allocators
        SYSTEM @< REQUIRED: required for allocation for memory using a standard system allocator (i.e. the default)
        OS_GENERIC_PRIORITY_QUEUE @< REQUIRED: required for Os::Queue memory allocation when using queues that allocate memory
    }
}
}
