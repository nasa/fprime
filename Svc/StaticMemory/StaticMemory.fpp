module Svc {

  @ A component that provides statically allocated memory
  passive component StaticMemory {

    @ Mutexed buffer deallocate input port
    guarded input port bufferDeallocate: [StaticMemoryAllocations] Fw.BufferSend

    @ Mutexed buffer allocate input port
    guarded input port bufferAllocate: [StaticMemoryAllocations] Fw.BufferGet

  }

}
