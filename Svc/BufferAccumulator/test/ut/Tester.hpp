// ====================================================================== 
// \title  Tester.hpp
// \author bocchino
// \brief  BufferAccumulator test harness interface
//
// \copyright
// Copyright 2009-2017, by the California Institute of Technology.
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

#include "Autocode/GTestBase.hpp"
#include "ASTERIA/Components/BufferAccumulator/BufferAccumulator.hpp"

#define MAX_NUM_BUFFERS 10

namespace ASTERIA {

  class Tester :
    public BufferAccumulatorGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester(
          const U32 maxNumBuffers = MAX_NUM_BUFFERS //!< The maximum number of buffers
      );

      //! Destroy object Tester
      //!
      ~Tester(void);

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_bufferSendOutDrain
      //!
      void from_bufferSendOutDrain_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer fwBuffer 
      );

      //! Handler for from_bufferSendOutReturn
      //!
      void from_bufferSendOutReturn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer fwBuffer 
      );

      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts(void);

      //! Initialize components
      //!
      void initComponents(void);

    protected:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      BufferAccumulator component;

  };

}

#endif
