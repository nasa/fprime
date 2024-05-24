// ======================================================================
// \title  VersionTester.hpp
// \author sreddy
// \brief  hpp file for Version component test harness implementation class
// ======================================================================

#ifndef Svc_VersionTester_HPP
#define Svc_VersionTester_HPP

#include "Svc/Version/VersionGTestBase.hpp"
#include "Svc/Version/Version.hpp"

namespace Svc {

  class VersionTester :
    public VersionGTestBase
  {

    public:

      // ----------------------------------------------------------------------
      // Constants
      // ----------------------------------------------------------------------

      // Maximum size of histories storing events, telemetry, and port outputs
      static const FwSizeType MAX_HISTORY_SIZE = 100;

      // Instance ID supplied to the component instance under test
      static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object VersionTester
      VersionTester();

      //! Destroy object VersionTester
      ~VersionTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! test startup EVR
      void test_startup();
      
      //! test enable command
      void test_enable();

      //! test version command
      void test_versions();
      //! test all commands
      void test_commands();

      //! test get version
      void test_getVer();
      //! test set version
      void test_setVer(bool is_enabled);
      //!test all ports
      void test_ports();
      //clear history
      void clear_all();
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
      Version component;

  };

}

#endif
