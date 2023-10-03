// ======================================================================
// \title  ComSplitter/test/ut/Tester.hpp
// \author gcgandhi
// \brief  hpp file for ComSplitter test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "ComSplitterGTestBase.hpp"
#include "Svc/ComSplitter/ComSplitter.hpp"

namespace Svc {

  class ComSplitterTester :
    public ComSplitterGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object ComSplitterTester
      //!
      ComSplitterTester();

      //! Destroy object ComSplitterTester
      //!
      ~ComSplitterTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------


      void test_nominal();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_comOut
      //!
      void from_comOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      void assert_comOut(
        const U32 index,
        const Fw::ComBuffer &data
      ) const;

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      ComSplitter component;

  };

} // end namespace Svc

#endif
