// ====================================================================== 
// \title  BufferManager.hpp
// \author bocchino
// \brief  hpp file for BufferManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 


#include "Svc/BufferManager/BufferManager.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  BufferManager ::
    BufferManager(
        const char *const compName,
        const U32 storeSize,
        const U32 allocationQueueSize
    ) :
      BufferManagerComponentBase(compName),
      store(storeSize),
      allocationQueue(allocationQueueSize)
  {

  }

  void BufferManager ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    BufferManagerComponentBase::init(instance);
  }

  BufferManager ::
    ~BufferManager(void)
  {

  }

  // ----------------------------------------------------------------------
  // The store 
  // ----------------------------------------------------------------------

  BufferManager::Store :: 
    Store(
        const U32 size
    ) : 
      totalSize(size),
      memoryBase(new U8[size]),
      freeIndex(0)
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
        const U32 n,
        U8* &result
    )
  {
    FW_ASSERT(this->freeIndex + n >= this->freeIndex);
    if (this->freeIndex + n > this->totalSize) {
      result = 0;
      return STAT_FAIL;
    }
    result = &this->memoryBase[this->freeIndex];
    this->freeIndex += n;
    return STAT_OK;
  }

  BufferManager::Store::Status BufferManager::Store ::
    free(const U32 n)
  {
    if (n > freeIndex) {
      this->freeIndex = 0;
      return STAT_FAIL;
    }
    this->freeIndex -= n;
    return STAT_OK;
  }

  // ----------------------------------------------------------------------
  // The allocation queue 
  // ----------------------------------------------------------------------

  BufferManager::AllocationQueue :: 
    AllocationQueue(
        const U32 size
    ) : 
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
    return Allocate::STAT_OK;
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
    return Free::STAT_OK;
  }

  void BufferManager ::
    sendTelemetry(void)
  {
    const U32 allocatedSize = this->store.getAllocatedSize();
    this->tlmWrite_BufferManager_AllocatedSize(
        const_cast<U32&>(allocatedSize)
    );
    const U32 numAllocatedBuffers = this->allocationQueue.getAllocationSize();
    this->tlmWrite_BufferManager_NumAllocatedBuffers(
        const_cast<U32&>(numAllocatedBuffers)
    );
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

    {
      const Store::Status status = 
        this->store.allocate(size, address);
      if (status == BufferManager::Store::STAT_FAIL) {
        this->log_WARNING_HI_BufferManager_StoreSizeExceeded();
        return buffer;
      }
    }

    {
      const AllocationQueue::Allocate::Status status =
        this->allocationQueue.allocate(size, id);
      if (status == AllocationQueue::Allocate::FULL) {
        this->store.free(size);
        this->log_WARNING_HI_BufferManager_AllocationQueueFull();
        return buffer;
      }
    }

    this->sendTelemetry();

    buffer.setbufferID(id);
    buffer.setdata(reinterpret_cast<U64>(address));

    return buffer;
  }

  void BufferManager ::
    bufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer buffer
    )
  {
    const U32 instance = static_cast<U32>(this->getInstance());
    FW_ASSERT(buffer.getmanagerID() == instance);

    const U32 expectedId = buffer.getbufferID();
    U32 sawId = 0;
    U32 size = 0;

    const AllocationQueue::Free::Status status =
      this->allocationQueue.free(expectedId, sawId, size);

    if (status == AllocationQueue::Free::EMPTY) {
      this->log_WARNING_HI_BufferManager_AllocationQueueEmpty();
    }
    else if (status == AllocationQueue::Free::ID_MISMATCH) {
      this->log_WARNING_HI_BufferManager_IDMismatch(expectedId, sawId);
    }
    else {
      const Store::Status status = this->store.free(size);
      FW_ASSERT(status == Store::STAT_OK);
      this->sendTelemetry();
    }
  }

}
