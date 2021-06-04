module Svc {

  passive component StaticMemory {

    guarded input port bufferDeallocate: [$StaticMemoryAllocations] Fw.BufferSend

    guarded input port bufferAllocate: [$StaticMemoryAllocations] Fw.BufferGet

  }

}
