// ======================================================================
// \title  Tester.hpp
// \author bocchino, mereweth
// \brief  BufferAccumulator test harness interface
//
// \copyright
// Copyright 2009-2017, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Svc/BufferAccumulator/BufferAccumulator.hpp"

#define MAX_NUM_BUFFERS 10

namespace Svc {

class Tester : public BufferAccumulatorGTestBase {
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

 public:
  //! Construct object Tester
  //!
  explicit Tester(bool doAllocateQueue = true);

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
      const NATIVE_INT_TYPE portNum,  //!< The port number
      Fw::Buffer& fwBuffer);

  //! Handler for from_bufferSendOutReturn
  //!
  void from_bufferSendOutReturn_handler(
      const NATIVE_INT_TYPE portNum,  //!< The port number
      Fw::Buffer& fwBuffer);

  //! Handler for from_pingOut
  //!
  void from_pingOut_handler(const NATIVE_INT_TYPE portNum,  //!< The port number
                            U32 key  //!< Value to return to pinger
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

  //! Whether to allocate/deallocate a queue for the user
  bool doAllocateQueue;
};

}  // end namespace Svc

#endif  //#ifndef TESTER_HPP
