// ====================================================================== 
// \title  BufferManager.hpp
// \author bocchino
// \brief  BufferManager component interface
//
// \copyright
// Copyright (C) 2015-2017, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include "Svc/BufferManager/BufferManager.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Warnings::State
  // ----------------------------------------------------------------------

  BufferManager::Warnings::State ::
    State(void) :
      storeSizeExceeded(false),
      tooManyBuffers(false)
  {

  }

  // ----------------------------------------------------------------------
  // Warnings 
  // ----------------------------------------------------------------------

  BufferManager::Warnings ::
    Warnings(BufferManager& bufferManager) :
      bufferManager(bufferManager)
  {

  }

  void BufferManager::Warnings ::
    update(const Status::t status)
  {
    switch (status) {
      case Status::SUCCESS:
        if (this->state.storeSizeExceeded || this->state.tooManyBuffers) {
          this->bufferManager.log_ACTIVITY_HI_ClearedErrorState();
        }
        this->state.storeSizeExceeded = false;
        this->state.tooManyBuffers = false;
        break;
      case Status::STORE_SIZE_EXCEEDED:
        if (!this->state.storeSizeExceeded) {
          this->bufferManager.log_WARNING_HI_StoreSizeExceeded();
          this->state.storeSizeExceeded = true;
        }
        break;
      case Status::TOO_MANY_BUFFERS:
        if (!this->state.tooManyBuffers) {
          this->bufferManager.log_WARNING_HI_TooManyBuffers();
          this->state.tooManyBuffers = true;
        }
        break;
      default:
        FW_ASSERT(0);
        break;
    }
  }

  // ----------------------------------------------------------------------
  // The store 
  // ----------------------------------------------------------------------

  BufferManager::Store :: 
    Store(const U32 size) : 
      totalSize(size),
      memoryBase(new U8[size]),
      freeIndex(0),
      padSize(0),
      allocatedSize(0)
  {
    
  }

  BufferManager::Store ::
    ~Store(void)
  {
    delete[] this->memoryBase;
  }

  U32 BufferManager::Store ::
    getAllocatedSize(void) const
  {
    return this->freeIndex;
  }

  BufferManager::Store::Status BufferManager::Store ::
    allocate(
        const U32 s,
        U8* &result
    )
  {
    Status status = SUCCESS;
    U32 newPadSize = 0;
    FW_ASSERT(this->allocatedSize <= this->totalSize);
    FW_ASSERT(this->freeIndex <= this->totalSize);
    if (this->freeIndex + s > this->totalSize) {
      // Pad size for wraparound allocation
      newPadSize = this->totalSize - this->freeIndex;
    }
    if (this->allocatedSize + s + newPadSize > this->totalSize) {
      // Allocation too large
      status = FAILURE;
      result = 0;
    }
    else {
      if (this->freeIndex + s > this->totalSize) {
        // Wraparound allocation
        FW_ASSERT(this->padSize == 0, this->padSize);
        this->padSize = newPadSize;
        this->allocatedSize += newPadSize;
        this->freeIndex = 0;
      }
      result = &this->memoryBase[this->freeIndex];
      this->allocatedSize += s;
      this->freeIndex += s;
      FW_ASSERT(this->allocatedSize <= this->totalSize);
      FW_ASSERT(this->freeIndex <= this->totalSize);
    }
    return status;
  }

  void BufferManager::Store ::
    free(
        const U32 size,
        U8 *const address
    )
  {
    FW_ASSERT(this->allocatedSize >= size, this->allocatedSize, size);
    this->allocatedSize -= size;
    if (address == this->memoryBase) {
      // Wraparound allocation
      FW_ASSERT(this->allocatedSize >= padSize, this->allocatedSize, padSize);
      this->allocatedSize -= this->padSize;
      this->padSize = 0;
    }
  }

  // ----------------------------------------------------------------------
  // The allocation queue 
  // ----------------------------------------------------------------------

  BufferManager::AllocationQueue :: 
    AllocationQueue(const U32 size) : 
      totalSize(size),
      data(new Entry[size]),
      nextId(0),
      allocateIndex(0),
      freeIndex(0),
      allocationSize(0)
  {
    
  }

  BufferManager::AllocationQueue ::
    ~AllocationQueue(void)
  {
    delete[] this->data;
  }

  U32 BufferManager::AllocationQueue ::
    getNextId(void)
  {
    const U32 id = this->nextId;
    ++this->nextId;
    FW_ASSERT(this->nextId != 0);
    return id;
  }

  U32 BufferManager::AllocationQueue ::
    getNextIndex(const U32 index)
  {
    FW_ASSERT(index < this->totalSize);
    const U32 result = index + 1;
    return result == this->totalSize ? 0 : result;
  }

  U32 BufferManager::AllocationQueue ::
    getAllocationSize(void) const
  {
    return this->allocationSize;
  }
  
  BufferManager::AllocationQueue::Allocate::Status 
    BufferManager::AllocationQueue ::
    allocate(
        const U32 size,
        U32& id
    )
  {
    FW_ASSERT(this->allocationSize <= this->totalSize);
    if (this->allocationSize == this->totalSize) {
      id = 0;
      return Allocate::FULL;
    }
    FW_ASSERT(this->allocateIndex < this->totalSize);
    Entry& e = this->data[allocateIndex];
    id = this->getNextId();
    e.id = id;
    e.size = size;
    this->allocateIndex = this->getNextIndex(this->allocateIndex);
    ++this->allocationSize;
    return Allocate::SUCCESS;
  }

  BufferManager::AllocationQueue::Free::Status 
    BufferManager::AllocationQueue ::
    free(
        const U32 expectedId,
        U32& sawId,
        U32& size
    )
  {
    sawId = 0;
    size = 0;
    if (this->allocationSize == 0) {
      FW_ASSERT(this->freeIndex == this->allocateIndex);
      return Free::EMPTY;
    }
    FW_ASSERT(this->freeIndex < this->totalSize);
    Entry& e = this->data[this->freeIndex];
    sawId = e.id;
    size = e.size;
    if (expectedId != sawId) {
      return Free::ID_MISMATCH;
    }
    this->freeIndex = this->getNextIndex(this->freeIndex);
    --this->allocationSize;
    return Free::SUCCESS;
  }

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  BufferManager ::
    BufferManager(
        const char *const compName,
        const U32 storeSize,
        const U32 maxNumBuffers
    ) :
      BufferManagerComponentBase(compName),
      warnings(*this),
      store(storeSize),
      allocationQueue(maxNumBuffers)
  {

  }

  void BufferManager ::
    init(const NATIVE_INT_TYPE instance) 
  {
    BufferManagerComponentBase::init(instance);
  }

  BufferManager ::
    ~BufferManager(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  Fw::Buffer BufferManager ::
    bufferGetCallee_handler(
        NATIVE_INT_TYPE portNum,
        U32 size 
    )
  {
    U8 *address;
    U32 id;
    Fw::Buffer buffer;
    buffer.set(this->getInstance(), 0, 0, size);

    Warnings::Status::t warningStatus = Warnings::Status::SUCCESS;

    {
      const Store::Status status = 
        this->store.allocate(size, address);
      if (status == BufferManager::Store::FAILURE) {
        warningStatus = Warnings::Status::STORE_SIZE_EXCEEDED;
      }
    }

    if (warningStatus == Warnings::Status::SUCCESS) {
      const AllocationQueue::Allocate::Status status =
        this->allocationQueue.allocate(size, id);
      if (status == AllocationQueue::Allocate::FULL) {
        this->store.free(size, address);
        warningStatus = Warnings::Status::TOO_MANY_BUFFERS;
      }
    }

    if (warningStatus == Warnings::Status::SUCCESS) {
      buffer.setbufferID(id);
      buffer.setdata(reinterpret_cast<U64>(address));
    }

    this->warnings.update(warningStatus);
    return buffer;
  }

  void BufferManager ::
    bufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &buffer
    )
  {

    const U32 instance = static_cast<U32>(this->getInstance());
    FW_ASSERT(buffer.getmanagerID() == instance);

    const U32 expectedId = buffer.getbufferID();
    U8 *const address = reinterpret_cast<U8*>(buffer.getdata());
    U32 sawId = 0;
    U32 size = 0;

    {
      const AllocationQueue::Free::Status status =
        this->allocationQueue.free(expectedId, sawId, size);
      FW_ASSERT(
          status == AllocationQueue::Free::SUCCESS,
          status, expectedId, sawId, size
      );
    }

    {
      this->store.free(size, address);
    }

  }

}
