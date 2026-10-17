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
    void testWriteZeroLength();
    void testAppending();
    void testWriteReadRoundTrip();
    void testWriteTruncatesExisting();
    void testTimeout();
    //! A clock that moves backward during a read or write must not abort the transfer
    void testTimeMovesBackward();
    //! Paths that leave no room for the hash extension are rejected as invalid input, not asserted
    void testPathTooLong();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    Fw::Time getTestTime(void) const;

    //! Time source that steps the clock back one second on every call
    static void backwardTime_handler(Fw::PassiveComponentBase* callComp, FwIndexType portNum, Fw::Time& time);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    FileWorker component;

    //! Time port that replaces the tester time source in testTimeMovesBackward
    Fw::InputTimePort m_backwardTimePort;

    //! Seconds returned by the next call to backwardTime_handler
    U32 m_backwardTimeSeconds;
};

}  // namespace Svc

#endif
