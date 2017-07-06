// ====================================================================== 
// \title  BufferManager/test/ut/Tester.hpp
// \author bocchino
// \brief  hpp file for BufferManager test harness implementation class
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

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Svc/BufferManager/BufferManager.hpp"

namespace Svc {

  class Tester :
    public BufferManagerGTestBase
  {

    public:

      Tester(void);

      ~Tester(void);

    public:

      // ---------------------------------------------------------------------- 
      // Tests
      // ---------------------------------------------------------------------- 

      // Allocate and free one buffer
      void allocateAndFreeOne(void);

      // Error: Free request with empty allocation queue
      void allocationQueueEmpty(void);

      // Error: Allocation request with full allocation queue
      void allocationQueueFull(void);

      // Error: Mismatched buffer ID
      void idMismatch(void);

      // Error: Store size exceeded
      void storeSizeExceeded(void);

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      void connectPorts(void);

      void initComponents(void);

    private:

      // ----------------------------------------------------------------------
      // The component under test 
      // ----------------------------------------------------------------------

      BufferManager bufferManager;

  };

};

#endif
