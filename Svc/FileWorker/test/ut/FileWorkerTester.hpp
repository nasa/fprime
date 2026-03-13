// ======================================================================
// \title  FileWorkerTester.hpp
// \author racheljt
// \brief  hpp file for FileWorker component test harness implementation class
// ======================================================================

#ifndef Svc_FileWorkerTester_HPP
#define Svc_FileWorkerTester_HPP

#include "Os/ValidateFile.hpp"
#include "Svc/FileWorker/FileWorker.hpp"
#include "Svc/FileWorker/FileWorkerGTestBase.hpp"
#include "Utils/CRCChecker.hpp"
#include "Utils/RateLimiter.hpp"

namespace Svc {

class FileWorkerTester final : public FileWorkerGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 20;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 20;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object FileWorkerTester
    FileWorkerTester();

    //! Destroy object FileWorkerTester
    ~FileWorkerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void testReadFile();
    void testClientDoneInvalid();
    void testCancel();
    void testFileError();
    void testVerify();
    void testTransfer();
    void testWriting();
    void testWritingOffset();
    void testAppending();
    void testTimeout();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    Fw::Time getTestTime(void) const;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FileWorker component;
};

}  // namespace Svc

#endif
