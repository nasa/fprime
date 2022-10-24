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
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Buffer/Buffer.hpp>
#include <new>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  BufferManagerComponentImpl ::
    BufferManagerComponentImpl(
        const char *const compName
    ) : BufferManagerComponentBase(compName)
    ,m_setup(false)
    ,m_cleaned(false)
    ,m_mgrId(0)
    ,m_buffers(nullptr)
    ,m_allocator(nullptr)
    ,m_memId(0)
    ,m_numStructs(0)
    ,m_highWater(0)
    ,m_currBuffs(0)
    ,m_noBuffs(0)
    ,m_emptyBuffs(0)
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
    ~BufferManagerComponentImpl()
  {
      if (m_setup) {
          this->cleanup();
      }
  }

  void BufferManagerComponentImpl ::
    cleanup()
  {
      FW_ASSERT(this->m_buffers);
      FW_ASSERT(this->m_allocator);

      if (not this->m_cleaned) {
          // walk through Fw::Buffer instances and delete them
          for (NATIVE_UINT_TYPE entry = 0; entry < this->m_numStructs; entry++) {
              this->m_buffers[entry].buff.~Buffer();
          }
          this->m_cleaned = true;
          // release memory
          this->m_allocator->deallocate(this->m_memId,this->m_buffers);
          this->m_setup = false;
      }
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
      // make sure component has been set up
      FW_ASSERT(this->m_setup);
      FW_ASSERT(m_buffers);
      // check for empty buffers - this is just a warning since this component returns
      // empty buffers if it can't allocate one.
      if (fwBuffer.getSize() == 0) {
          this->log_WARNING_HI_ZeroSizeBuffer();
          this->m_emptyBuffs++;
          return;
      }
      // use the bufferID member field to find the original slot
      U32 context = fwBuffer.getContext();
      U32 id = context & 0xFFFF;
      U32 mgrId = context >> 16;
      // check some things
      FW_ASSERT(id < this->m_numStructs,id,this->m_numStructs);
      FW_ASSERT(mgrId == this->m_mgrId,mgrId,id,this->m_mgrId);
      FW_ASSERT(true == this->m_buffers[id].allocated,id,this->m_mgrId);
      FW_ASSERT(reinterpret_cast<U8*>(fwBuffer.getData()) >= this->m_buffers[id].memory,id,this->m_mgrId);
      FW_ASSERT(reinterpret_cast<U8*>(fwBuffer.getData()) < (this->m_buffers[id].memory + this->m_buffers[id].size),id,this->m_mgrId);
      // user can make smaller for their own purposes, but it shouldn't be bigger
      FW_ASSERT(fwBuffer.getSize() <= this->m_buffers[id].size,id,this->m_mgrId);
      // clear the allocated flag
      this->m_buffers[id].allocated = false;
      this->m_currBuffs--;
  }

  Fw::Buffer BufferManagerComponentImpl ::
    bufferGetCallee_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
      // make sure component has been set up
      FW_ASSERT(this->m_setup);
      FW_ASSERT(m_buffers);
      // find smallest buffer based on size.
      for (NATIVE_UINT_TYPE buff = 0; buff < this->m_numStructs; buff++) {
          if ((not this->m_buffers[buff].allocated) and (size <= this->m_buffers[buff].size)) {
              this->m_buffers[buff].allocated = true;
              this->m_currBuffs++;
              if (this->m_currBuffs > this->m_highWater) {
                  this->m_highWater = this->m_currBuffs;
              }
              Fw::Buffer copy = this->m_buffers[buff].buff;
              // change size to match request
              copy.setSize(size);
              return copy;
          }
      }

      // if no buffers found, return empty buffer
      this->log_WARNING_HI_NoBuffsAvailable(size);
      this->m_noBuffs++;
      return Fw::Buffer();

  }

  void BufferManagerComponentImpl::setup(
    NATIVE_UINT_TYPE mgrId, //!< manager ID
    NATIVE_UINT_TYPE memId, //!< Memory segment identifier
    Fw::MemAllocator& allocator, //!< memory allocator
    const BufferBins& bins //!< Set of user bins
  ) {

    this->m_mgrId = mgrId;
    this->m_memId = memId;
    this->m_allocator = &allocator;
    // clear bins
    memset(&this->m_bufferBins,0,sizeof(this->m_bufferBins));

    this->m_bufferBins = bins;

    // compute the amount of memory needed
    NATIVE_UINT_TYPE memorySize = 0; // track needed memory
    this->m_numStructs = 0; // size the number of tracking structs
    // walk through bins and add up the sizes
    for (NATIVE_UINT_TYPE bin = 0; bin < BUFFERMGR_MAX_NUM_BINS; bin++) {
        if (this->m_bufferBins.bins[bin].numBuffers) {
            memorySize +=
                (this->m_bufferBins.bins[bin].bufferSize * this->m_bufferBins.bins[bin].numBuffers) + // allocate each set of buffer memory
                (static_cast<NATIVE_UINT_TYPE>(sizeof(AllocatedBuffer)) * this->m_bufferBins.bins[bin].numBuffers); // allocate the structs to track the buffers
            this->m_numStructs += this->m_bufferBins.bins[bin].numBuffers;
        }
    }

    NATIVE_UINT_TYPE allocatedSize = memorySize;
    bool recoverable = false; //!< don't care if it is recoverable since they are a pool of user buffers

    // allocate memory
    void *memory = allocator.allocate(memId,allocatedSize,recoverable);
    // make sure the memory returns was non-zero and the size requested
    FW_ASSERT(memory);
    FW_ASSERT(memorySize == allocatedSize,memorySize,allocatedSize);
    // structs will be at beginning of memory
    this->m_buffers = static_cast<AllocatedBuffer*>(memory);
    // memory buffers will be at end of structs in memory, so compute that memory as the beginning of the
    // struct past the number of structs
    U8* bufferMem = reinterpret_cast<U8*>(&this->m_buffers[this->m_numStructs]);

    // walk through entries and initialize them
    NATIVE_UINT_TYPE currStruct = 0;
    for (NATIVE_UINT_TYPE bin = 0; bin < BUFFERMGR_MAX_NUM_BINS; bin++) {
        if (this->m_bufferBins.bins[bin].numBuffers) {
            for (NATIVE_UINT_TYPE binEntry = 0; binEntry < this->m_bufferBins.bins[bin].numBuffers; binEntry++) {
                // placement new for Fw::Buffer instance. We don't need the new() return value,
                // because we know where the Fw::Buffer instance is
                U32 context = (this->m_mgrId << 16) | currStruct;
                (void) new(&this->m_buffers[currStruct].buff) Fw::Buffer(bufferMem,this->m_bufferBins.bins[bin].bufferSize,context);
                this->m_buffers[currStruct].allocated = false;
                this->m_buffers[currStruct].memory = bufferMem;
                this->m_buffers[currStruct].size = this->m_bufferBins.bins[bin].bufferSize;
                bufferMem += this->m_bufferBins.bins[bin].bufferSize;
                currStruct++;
            }
        }
    }

    // check that the initiation pointer made it to the end of allocated space
    U8* const CURR_PTR = bufferMem;
    U8* const END_PTR = static_cast<U8*>(memory) + memorySize;
    FW_ASSERT(CURR_PTR == END_PTR,
        reinterpret_cast<POINTER_CAST>(CURR_PTR), reinterpret_cast<POINTER_CAST>(END_PTR));
    // secondary init verification
    FW_ASSERT(currStruct == this->m_numStructs,currStruct,this->m_numStructs);
    // indicate setup is done
    this->m_setup = true;
  }

  void BufferManagerComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // write telemetry values
    this->tlmWrite_HiBuffs(this->m_highWater);
    this->tlmWrite_CurrBuffs(this->m_currBuffs);
    this->tlmWrite_TotalBuffs(this->m_numStructs);
    this->tlmWrite_NoBuffs(this->m_noBuffs);
    this->tlmWrite_EmptyBuffs(this->m_emptyBuffs);
  }

} // end namespace Svc
