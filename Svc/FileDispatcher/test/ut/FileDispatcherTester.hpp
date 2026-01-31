// ======================================================================
// \title  FileDispatcherTester.hpp
// \author tcanham
// \brief  hpp file for FileDispatcher component test harness implementation class
// ======================================================================

#ifndef Svc_FileDispatcherTester_HPP
#define Svc_FileDispatcherTester_HPP

#include "Svc/FileDispatcher/FileDispatcher.hpp"
#include "Svc/FileDispatcher/FileDispatcherGTestBase.hpp"

namespace Svc {

class FileDispatcherTester final : public FileDispatcherGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object FileDispatcherTester
    FileDispatcherTester();

    //! Destroy object FileDispatcherTester
    ~FileDispatcherTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Dispatch test
    void dispatchTest();

    //! Dispatch disabled test
    void dispatchAllDisabledTest();

    //! Dispatch command disabled test
    void dispatchAllCmdDisabledTest();

    //! Dispatch command enabled test
    void dispatchAllCmdEnabledTest();

    //! Dispatch ping test
    void dispatchPingTest();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! helper
    void populateTable(Svc::FileDispatcherTable& table, bool enabled);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FileDispatcher component;

    void textLogIn(FwEventIdType id,                //!< The event ID
                   const Fw::Time& timeTag,         //!< The time
                   const Fw::LogSeverity severity,  //!< The severity
                   const Fw::TextLogString& text    //!< The event string
                   ) override;
};

}  // namespace Svc

#endif
