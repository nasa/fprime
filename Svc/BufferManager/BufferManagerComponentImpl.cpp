// ======================================================================
// \title  BufferManagerComponentImpl.cpp
// \author tcanham
// \brief  cpp file for BufferManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Svc/BufferManager/BufferManagerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Fw/Types/Assert.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  BufferManagerComponentImpl ::
    BufferManagerComponentImpl(
        const char *const compName
    ) : BufferManagerComponentBase(compName)
    ,m_buffers(0)
    ,m_allocator(0)
    ,m_identifier(0)
  {

  }

  void BufferManagerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    BufferManagerComponentBase::init(instance);
  }

  BufferManagerComponentImpl ::
    ~BufferManagerComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void BufferManagerComponentImpl ::
    bufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  Fw::Buffer BufferManagerComponentImpl ::
    bufferGetCallee_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    return Fw::Buffer();
  }

  void BufferManagerComponentImpl::setup(
    NATIVE_UINT_TYPE memID, //!< Memory segment identifier
    Fw::MemAllocator& allocator, //!< memory allocator
    const BufferBins& bins //!< Set of user bins
  ) {

    this->m_identifier = memID;
    this->m_allocator = &allocator;
    this->m_bufferBins = bins;
    // compute the amount of memory needed
    NATIVE_UINT_TYPE memorySize = 0; // size needed memory
    NATIVE_UINT_TYPE numStructs = 0; // size the number of tracking structs
    // walk through bins and add up the sizes
    for (NATIVE_UINT_TYPE bin = 0; bin < MAX_NUM_BINS; bin++) {
        if (bins.bins[bin].numBuffers) {
            memorySize += 
                bins.bins[bin].bufferSize * bins.bins[bin].numBuffers // allocate each set of buffer memory
                + sizeof(AllocatedBuffer) * bins.bins[bin].numBuffers; // allocate the structs to track the buffers
            numStructs += bins.bins[bin].numBuffers;
        }
    }

    NATIVE_UINT_TYPE allocatedSize = memorySize;
    bool recoverable; //!< don't care if it is recoverable since they are a pool of user buffers

    // allocate memory
    void *memory = allocator.allocate(memID,allocatedSize,recoverable);
    // structs will be at beginning of memory
    this->m_buffers = static_cast<AllocatedBuffer*>(memory);
    // memory buffers will be at end of structs in memory, so compute that memory as the struct past
    // the number of structs
    U8* bufferMem = reinterpret_cast<U8*>(&this->m_buffers[numStructs]);
    // make sure the memory returns was non-zero and the size requested
    FW_ASSERT(memorySize == allocatedSize,memorySize,allocatedSize);
    FW_ASSERT(this->m_buffers);

    // walk through entries and initialize them
    NATIVE_UINT_TYPE currStruct = 0;
    for (NATIVE_UINT_TYPE bin = 0; bin < MAX_NUM_BINS; bin++) {
        if (bins.bins[bin].numBuffers) {
            for (NATIVE_UINT_TYPE binEntry = 0; binEntry < bins.bins[bin].numBuffers; binEntry++) {
                // placement new for Fw::Buffer instance
                Fw::Buffer* ptr = new(&this->m_buffers[currStruct].buff) Fw::Buffer;
                this->m_buffers[currStruct].allocated = false;
                this->m_buffers[currStruct].memory = bufferMem;
                bufferMem += bins.bins[bin].bufferSize;
                currStruct++;
            }
        }
    }
      
    
  }

} // end namespace Svc
