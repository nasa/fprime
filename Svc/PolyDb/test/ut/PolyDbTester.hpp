// ======================================================================
// \title  PolyDbTester.hpp
// \author tcanham
// \brief  hpp file for PolyDb component test harness implementation class
// ======================================================================

#ifndef Svc_PolyDbTester_HPP
#define Svc_PolyDbTester_HPP

#include "Svc/PolyDb/PolyDbGTestBase.hpp"
#include "Svc/PolyDb/PolyDb.hpp"

namespace Svc {

  class PolyDbTester :
    public PolyDbGTestBase
  {

    public:

      // ----------------------------------------------------------------------
      // Constants
      // ----------------------------------------------------------------------

      // Maximum size of histories storing events, telemetry, and port outputs
      static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;

      // Instance ID supplied to the component instance under test
      static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object PolyDbTester
      PolyDbTester();

      //! Destroy object PolyDbTester
      ~PolyDbTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Do nominal testing
      void runNominalReadWrite();

    private:

      // ----------------------------------------------------------------------
      // Helper functions
      // ----------------------------------------------------------------------

      //! Connect ports
      void connectPorts();

      //! Initialize components
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Member variables
      // ----------------------------------------------------------------------

      //! The component under test
      PolyDb component;

  };

}

#endif
