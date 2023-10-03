// ======================================================================
// \title  StaticMemory.hpp
// \author mstarch
// \brief  cpp file for StaticMemory test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "StaticMemoryTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  StaticMemoryTester ::
    StaticMemoryTester() :
      StaticMemoryGTestBase("Tester", MAX_HISTORY_SIZE),
      component("StaticMemory")
  {
    this->initComponents();
    this->connectPorts();
  }

  StaticMemoryTester ::
    ~StaticMemoryTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void StaticMemoryTester ::
      test_allocate()
  {
     Fw::Buffer allocations[StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS];
      for (U32 i = 0; i < StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS; i++) {
          U32 random_size = STest::Pick::lowerUpper(1, StaticMemoryConfig::STATIC_MEMORY_ALLOCATION_SIZE);
          allocations[i] = invoke_to_bufferAllocate(i, random_size);
          // Test allocated, correct pointer
          ASSERT_TRUE(component.m_allocated[i]) << "Did not mark buffer as allocated";
          ASSERT_EQ(allocations[i].getData(), component.m_static_memory[i]) << "Sent incorrect buffer";
          ASSERT_GE(allocations[i].getSize(), random_size) << "Did not allocate enough";
      }
      for (U32 i = 0; i < StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS; i++) {
          // Set size to zero, and adjust the data pointer to ensure user can modify these values
          allocations[i].setSize(0);
          allocations[i].setData(allocations[i].getData() + 1);
          invoke_to_bufferDeallocate(i, allocations[i]);
          // Test allocated, correct pointer
          ASSERT_FALSE(component.m_allocated[i]) << "Did not mark buffer as allocated";
      }
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void StaticMemoryTester ::
    connectPorts()
  {

    // bufferDeallocate
    for (NATIVE_INT_TYPE i = 0; i < StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS; ++i) {
      this->connect_to_bufferDeallocate(
          i,
          this->component.get_bufferDeallocate_InputPort(i)
      );
    }

    // bufferAllocate
    for (NATIVE_INT_TYPE i = 0; i < StaticMemoryComponentBase::NUM_BUFFERALLOCATE_INPUT_PORTS; ++i) {
      this->connect_to_bufferAllocate(
          i,
          this->component.get_bufferAllocate_InputPort(i)
      );
    }




  }

  void StaticMemoryTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
