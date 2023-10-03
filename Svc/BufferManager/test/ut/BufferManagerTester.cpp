// ======================================================================
// \title  BufferManager.hpp
// \author tcanham
// \brief  cpp file for BufferManager test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "BufferManagerTester.hpp"
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <cstdlib>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 100


// Bin buffer sizes/numbers
static const NATIVE_UINT_TYPE BIN0_BUFFER_SIZE = 10;
static const NATIVE_UINT_TYPE BIN0_NUM_BUFFERS = 2;
static const NATIVE_UINT_TYPE BIN1_BUFFER_SIZE = 12;
static const NATIVE_UINT_TYPE BIN1_NUM_BUFFERS = 4;
static const NATIVE_UINT_TYPE BIN2_BUFFER_SIZE = 100;
static const NATIVE_UINT_TYPE BIN2_NUM_BUFFERS = 3;

// Other constants
static const NATIVE_UINT_TYPE MEM_ID = 49;
static const NATIVE_UINT_TYPE MGR_ID = 32;

// Define our own instrumented allocator for testing
class TestAllocator: public Fw::MemAllocator {
    public:
        TestAllocator(){};
        virtual ~TestAllocator(){};
        //! Allocate memory
        /*!
          * \param identifier the memory segment identifier (not used)
          * \param size the requested size (not changed)
          * \param recoverable - flag to indicate the memory could be recoverable (always set to false)
          * \return the pointer to memory. Zero if unable to allocate.
          */
        void *allocate(
                const NATIVE_UINT_TYPE identifier,
                NATIVE_UINT_TYPE &size,
                bool& recoverable) {
            this->m_reqId = identifier;
            this->m_reqSize = size;
            this->m_mem = this->m_alloc.allocate(identifier,size,recoverable);
            return this->m_mem;
        }
        //! Deallocate memory
        /*!
          * \param identifier the memory segment identifier (not used)
          * \ptr the pointer to memory returned by allocate()
          */
        void deallocate(
                const NATIVE_UINT_TYPE identifier,
                void* ptr) {
            this->m_alloc.deallocate(identifier,ptr);
        }

        NATIVE_UINT_TYPE getId() {
            return this->m_reqId;
        }

        NATIVE_UINT_TYPE getSize() {
            return this->m_reqSize;
        }

        void* getMem() {
            return this->m_mem;
        }

        private:
          Fw::MallocAllocator m_alloc;
          NATIVE_UINT_TYPE m_reqId;
          NATIVE_UINT_TYPE m_reqSize;
          void* m_mem;

};


namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  BufferManagerTester ::
    BufferManagerTester() :
      BufferManagerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("BufferManager")
  {
    this->initComponents();
    this->connectPorts();
  }

  BufferManagerTester ::
    ~BufferManagerTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void BufferManagerTester ::
    testSetup()
  {

      REQUIREMENT("FPRIME-BM-001");

      BufferManagerComponentImpl::BufferBins bins;
      memset(&bins,0,sizeof(bins));
      bins.bins[0].bufferSize = BIN0_BUFFER_SIZE;
      bins.bins[0].numBuffers = BIN0_NUM_BUFFERS;
      bins.bins[1].bufferSize = BIN1_BUFFER_SIZE;
      bins.bins[1].numBuffers = BIN1_NUM_BUFFERS;
      bins.bins[2].bufferSize = BIN2_BUFFER_SIZE;
      bins.bins[2].numBuffers = BIN2_NUM_BUFFERS;

      TestAllocator alloc;

      this->component.setup(MGR_ID,MEM_ID,alloc,bins);
      ASSERT_EQ(MEM_ID,this->component.m_memId);
      ASSERT_EQ(MGR_ID,this->component.m_mgrId);
      // check allocation state

      // Check that enough buffers were created
      ASSERT_EQ(
        BIN0_NUM_BUFFERS +
        BIN1_NUM_BUFFERS +
        BIN2_NUM_BUFFERS,
        this->component.m_numStructs
        );

      REQUIREMENT("FPRIME-BM-005");

      // check that enough memory was requested
      NATIVE_UINT_TYPE memSize =
        (BIN0_NUM_BUFFERS + BIN1_NUM_BUFFERS + BIN2_NUM_BUFFERS)*sizeof(Svc::BufferManagerComponentImpl::AllocatedBuffer) +
        (BIN0_NUM_BUFFERS*BIN0_BUFFER_SIZE + BIN1_NUM_BUFFERS*BIN1_BUFFER_SIZE + BIN2_NUM_BUFFERS*BIN2_BUFFER_SIZE);
      ASSERT_EQ(memSize,alloc.getSize());

      // check that correct ID was requested
      ASSERT_EQ(MEM_ID,alloc.getId());

      // first buffer should point at location just past buffer structs
      U8 *mem = reinterpret_cast<U8*>(alloc.getMem()) + this->component.m_numStructs*sizeof(Svc::BufferManagerComponentImpl::AllocatedBuffer); ;

      // check the buffer properties
      for (NATIVE_UINT_TYPE entry = 0; entry < this->component.m_numStructs; entry++) {
          // check context ID
          ASSERT_EQ(this->component.m_buffers[entry].buff.getContext(),((MGR_ID << 16)| entry));
          // check allocation state
          ASSERT_FALSE(this->component.m_buffers[entry].allocated);
          // check buffer sizes
          if (entry < BIN0_NUM_BUFFERS) {
              ASSERT_EQ(BIN0_BUFFER_SIZE,this->component.m_buffers[entry].size);
              ASSERT_EQ(mem,this->component.m_buffers[entry].memory);
              mem += BIN0_BUFFER_SIZE;
          } else if (entry < BIN0_NUM_BUFFERS + BIN1_NUM_BUFFERS) {
              ASSERT_EQ(BIN1_BUFFER_SIZE,this->component.m_buffers[entry].size);
              ASSERT_EQ(mem,this->component.m_buffers[entry].memory);
              mem += BIN1_BUFFER_SIZE;
          } else if (entry < BIN0_NUM_BUFFERS + BIN1_NUM_BUFFERS + BIN2_NUM_BUFFERS) {
              ASSERT_EQ(BIN2_BUFFER_SIZE,this->component.m_buffers[entry].size);
              ASSERT_EQ(mem,this->component.m_buffers[entry].memory);
              mem += BIN2_BUFFER_SIZE;
          } else {
              // just in case the logic is wrong
              ASSERT_TRUE(false);
          }
      }

      // memory location should be at end of allocated memory
      ASSERT_EQ(mem,reinterpret_cast<U8*>(alloc.getMem()) + alloc.getSize());

      this->component.cleanup();
      ASSERT_TRUE(this->component.m_cleaned);
      ASSERT_FALSE(this->component.m_setup);
  }

  void BufferManagerTester::oneBufferSize() {

      BufferManagerComponentImpl::BufferBins bins;
      memset(&bins,0,sizeof(bins));
      bins.bins[0].bufferSize = BIN1_BUFFER_SIZE;
      bins.bins[0].numBuffers = BIN1_NUM_BUFFERS;

      TestAllocator alloc;

      this->component.setup(MGR_ID,MEM_ID,alloc,bins);

      Fw::Buffer buffs[BIN1_NUM_BUFFERS];

      for (NATIVE_UINT_TYPE b=0; b<BIN1_NUM_BUFFERS; b++) {
          // Get the buffers
          buffs[b] = this->invoke_to_bufferGetCallee(0,BIN1_BUFFER_SIZE);
          // check allocation state
          ASSERT_TRUE(this->component.m_buffers[b].allocated);
          // check stats
          ASSERT_EQ(b+1,this->component.m_currBuffs);
          // check stats
          ASSERT_EQ(b+1,this->component.m_highWater);
      }


      // should send back empty buffer
      Fw::Buffer noBuff = this->invoke_to_bufferGetCallee(0,BIN1_BUFFER_SIZE);
      ASSERT_EQ(1,this->component.m_noBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(5);
      ASSERT_TLM_TotalBuffs_SIZE(1);
      ASSERT_TLM_TotalBuffs(0,BIN1_NUM_BUFFERS);
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,BIN1_NUM_BUFFERS);
      ASSERT_TLM_HiBuffs_SIZE(1);
      ASSERT_TLM_HiBuffs(0,BIN1_NUM_BUFFERS);
      ASSERT_TLM_NoBuffs_SIZE(1);
      ASSERT_TLM_NoBuffs(0,1);
      ASSERT_TLM_EmptyBuffs_SIZE(1);
      ASSERT_TLM_EmptyBuffs(0,0);

      // clear histories
      this->clearHistory();

      REQUIREMENT("FPRIME-BM-006");

      // randomly return buffers
      time_t t;
      srand(static_cast<unsigned>(time(&t)));

      bool returned[BIN1_NUM_BUFFERS] = {false};

      for (NATIVE_UINT_TYPE b=0; b<BIN1_NUM_BUFFERS; b++) {
          NATIVE_UINT_TYPE entry;
          while (true) {
              entry = rand() % BIN1_NUM_BUFFERS;
              if (not returned[entry]) {
                  returned[entry] = true;
                  break;
              }
          }
          // return the buffer
          printf("Returning buffer %d\n",entry);
          this->invoke_to_bufferSendIn(0,buffs[entry]);
          // check allocation state
          ASSERT_FALSE(this->component.m_buffers[entry].allocated);
          ASSERT_EQ(BIN1_NUM_BUFFERS-b-1,this->component.m_currBuffs);
          ASSERT_EQ(BIN1_NUM_BUFFERS,this->component.m_highWater);
      }

      // should reject empty buffer
      this->invoke_to_bufferSendIn(0,noBuff);
      ASSERT_EQ(1,this->component.m_emptyBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(2);
      // No total buffs or no buffs since only on update
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,0);
      ASSERT_TLM_NoBuffs_SIZE(0);
      ASSERT_TLM_EmptyBuffs_SIZE(1);
      ASSERT_TLM_EmptyBuffs(0,1);

      // all buffers should be deallocated
      for (NATIVE_UINT_TYPE b=0; b<this->component.m_numStructs; b++) {
          ASSERT_FALSE(this->component.m_buffers[b].allocated);
      }

      // cleanup BufferManager memory
      this->component.cleanup();

  }

  void BufferManagerTester::multBuffSize() {

      BufferManagerComponentImpl::BufferBins bins;
      memset(&bins,0,sizeof(bins));
      bins.bins[0].bufferSize = BIN0_BUFFER_SIZE;
      bins.bins[0].numBuffers = BIN0_NUM_BUFFERS;
      bins.bins[1].bufferSize = BIN1_BUFFER_SIZE;
      bins.bins[1].numBuffers = BIN1_NUM_BUFFERS;
      bins.bins[2].bufferSize = BIN2_BUFFER_SIZE;
      bins.bins[2].numBuffers = BIN2_NUM_BUFFERS;

      TestAllocator alloc;

      this->component.setup(MGR_ID,MEM_ID,alloc,bins);

      Fw::Buffer buffs[BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS];

      REQUIREMENT("FPRIME-BM-002");

      // BufferManager should be able to provide the whole pool worth of buffers
      // for a requested size smaller than the smallest bin.

      for (NATIVE_UINT_TYPE b=0; b<BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS; b++) {
          // Get the buffers
          buffs[b] = this->invoke_to_bufferGetCallee(0,BIN0_BUFFER_SIZE);
          // check allocation state
          ASSERT_TRUE(this->component.m_buffers[b].allocated);
          // check stats
          ASSERT_EQ(b+1,this->component.m_currBuffs);
          // check stats
          ASSERT_EQ(b+1,this->component.m_highWater);
      }

      REQUIREMENT("FPRIME-BM-003");

      // should send back empty buffer
      Fw::Buffer noBuff = this->invoke_to_bufferGetCallee(0,BIN1_BUFFER_SIZE);
      ASSERT_EQ(1,this->component.m_noBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(5);
      ASSERT_TLM_TotalBuffs_SIZE(1);
      ASSERT_TLM_TotalBuffs(0,BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS);
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS);
      ASSERT_TLM_HiBuffs_SIZE(1);
      ASSERT_TLM_HiBuffs(0,BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS);
      ASSERT_TLM_NoBuffs_SIZE(1);
      ASSERT_TLM_NoBuffs(0,1);
      ASSERT_TLM_EmptyBuffs_SIZE(1);
      ASSERT_TLM_EmptyBuffs(0,0);

      // clear histories
      this->clearHistory();


      for (NATIVE_UINT_TYPE b=0; b<BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS; b++) {
          // return the buffer
          this->invoke_to_bufferSendIn(0,buffs[b]);
          // check allocation state
          ASSERT_FALSE(this->component.m_buffers[b].allocated);
          ASSERT_EQ(BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS-b-1,this->component.m_currBuffs);
          ASSERT_EQ(BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS,this->component.m_highWater);
      }

      REQUIREMENT("FPRIME-BM-004");

      // should reject empty buffer
      this->invoke_to_bufferSendIn(0,noBuff);
      ASSERT_EQ(1,this->component.m_emptyBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(2);
      // No total buffs or no buffs since only on update
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,0);
      ASSERT_TLM_NoBuffs_SIZE(0);
      ASSERT_TLM_EmptyBuffs_SIZE(1);
      ASSERT_TLM_EmptyBuffs(0,1);

      // clear histories
      this->clearHistory();

      // all buffers should be deallocated
      for (NATIVE_UINT_TYPE b=0; b<this->component.m_numStructs; b++) {
          ASSERT_FALSE(this->component.m_buffers[b].allocated);
      }

      // clear histories
      this->clearEvents();

      // BufferManager should be able to provide the BIN1 and BIN2 worth of buffers
      // for a requested size just smaller than the BIN1 size

      for (NATIVE_UINT_TYPE b=0; b<BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS; b++) {
          // Get the buffers
          buffs[b] = this->invoke_to_bufferGetCallee(0,BIN1_BUFFER_SIZE);
          // check allocation state - should be allocating from bin 1
          ASSERT_TRUE(this->component.m_buffers[b+BIN0_NUM_BUFFERS].allocated);
          // check stats
          ASSERT_EQ(b+1,this->component.m_currBuffs);
      }

      // should send back empty buffer
      noBuff = this->invoke_to_bufferGetCallee(0,BIN1_BUFFER_SIZE);
      ASSERT_EQ(2,this->component.m_noBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_TotalBuffs_SIZE(0);
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS);
      ASSERT_TLM_NoBuffs_SIZE(1);
      ASSERT_TLM_NoBuffs(0,2);
      ASSERT_TLM_EmptyBuffs_SIZE(0);

      // clear histories
      this->clearHistory();

      for (NATIVE_UINT_TYPE b=0; b<BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS; b++) {
          // return the buffer
          this->invoke_to_bufferSendIn(0,buffs[b]);
          // check allocation state - should be freeing from bin 1
          ASSERT_FALSE(this->component.m_buffers[b+BIN0_NUM_BUFFERS].allocated);
          ASSERT_EQ(BIN1_NUM_BUFFERS+BIN2_NUM_BUFFERS-b-1,this->component.m_currBuffs);
      }

      // should reject empty buffer
      this->invoke_to_bufferSendIn(0,noBuff);
      ASSERT_EQ(2,this->component.m_emptyBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(2);
      // No total buffs or no buffs since only on update
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,0);
      ASSERT_TLM_NoBuffs_SIZE(0);
      ASSERT_TLM_EmptyBuffs_SIZE(1);
      ASSERT_TLM_EmptyBuffs(0,2);

      // clear histories
      this->clearHistory();

      // all buffers should be deallocated
      for (NATIVE_UINT_TYPE b=0; b<this->component.m_numStructs; b++) {
          ASSERT_FALSE(this->component.m_buffers[b].allocated);
      }

      // BufferManager should be able to provide the BIN2 worth of buffers
      // for a requested size just smaller than the BIN2 size

      for (NATIVE_UINT_TYPE b=0; b<BIN2_NUM_BUFFERS; b++) {
          // Get the buffers
          buffs[b] = this->invoke_to_bufferGetCallee(0,BIN2_BUFFER_SIZE);
          // check allocation state - should be allocating from bin 1
          ASSERT_TRUE(this->component.m_buffers[b+BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS].allocated);
          // check stats
          ASSERT_EQ(b+1,this->component.m_currBuffs);
      }

      // should send back empty buffer
      noBuff = this->invoke_to_bufferGetCallee(0,BIN2_BUFFER_SIZE);
      ASSERT_EQ(3,this->component.m_noBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_TotalBuffs_SIZE(0);
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,BIN2_NUM_BUFFERS);
      ASSERT_TLM_NoBuffs_SIZE(1);
      ASSERT_TLM_NoBuffs(0,3);
      ASSERT_TLM_EmptyBuffs_SIZE(0);

      // clear histories
      this->clearHistory();


      for (NATIVE_UINT_TYPE b=0; b<BIN2_NUM_BUFFERS; b++) {
          // return the buffer
          this->invoke_to_bufferSendIn(0,buffs[b]);
          // check allocation state - should be freeing from bin 1
          ASSERT_FALSE(this->component.m_buffers[b+BIN0_NUM_BUFFERS+BIN1_NUM_BUFFERS].allocated);
          ASSERT_EQ(BIN2_NUM_BUFFERS-b-1,this->component.m_currBuffs);
      }

      // should reject empty buffer
      this->invoke_to_bufferSendIn(0,noBuff);
      ASSERT_EQ(3,this->component.m_emptyBuffs);

      // check telemetry
      this->invoke_to_schedIn(0,0);
      ASSERT_TLM_SIZE(2);
      // No total buffs or no buffs since only on update
      ASSERT_TLM_CurrBuffs_SIZE(1);
      ASSERT_TLM_CurrBuffs(0,0);
      ASSERT_TLM_NoBuffs_SIZE(0);
      ASSERT_TLM_EmptyBuffs_SIZE(1);
      ASSERT_TLM_EmptyBuffs(0,3);

      // all buffers should be deallocated
      for (NATIVE_UINT_TYPE b=0; b<this->component.m_numStructs; b++) {
          ASSERT_FALSE(this->component.m_buffers[b].allocated);
      }

      // cleanup BufferManager memory
      this->component.cleanup();

  }




  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void BufferManagerTester ::
    connectPorts()
  {

    // bufferSendIn
    this->connect_to_bufferSendIn(
        0,
        this->component.get_bufferSendIn_InputPort(0)
    );

    // bufferGetCallee
    this->connect_to_bufferGetCallee(
        0,
        this->component.get_bufferGetCallee_InputPort(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(
        0,
        this->get_from_timeCaller(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0,
        this->get_from_eventOut(0)
    );

    // textEventOut
    this->component.set_textEventOut_OutputPort(
        0,
        this->get_from_textEventOut(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0,
        this->get_from_tlmOut(0)
    );

    // schedIn
    this->connect_to_schedIn(
        0,
        this->component.get_schedIn_InputPort(0)
    );

  }

  void BufferManagerTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

  void BufferManagerTester::textLogIn(const FwEventIdType id, //!< The event ID
          const Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
          ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);
  }


} // end namespace Svc
