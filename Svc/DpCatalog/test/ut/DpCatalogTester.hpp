// ======================================================================
// \title  DpCatalogTester.hpp
// \author tcanham
// \brief  hpp file for DpCatalog component test harness implementation class
// ======================================================================

#ifndef Svc_DpCatalogTester_HPP
#define Svc_DpCatalogTester_HPP

#include <STest/Pick/Pick.hpp>
#include "Fw/Types/MallocAllocator.hpp"
#include "Svc/DpCatalog/DpCatalog.hpp"
#include "Svc/DpCatalog/DpCatalogGTestBase.hpp"

namespace Svc {

class DpCatalogTester : public DpCatalogGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const U32 MAX_HISTORY_SIZE = 1000;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    // Highest priority value; with a start of 0 this requests the whole catalog
    static const FwDpPriorityType PRIORITY_MAX = std::numeric_limits<FwDpPriorityType>::max();

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpCatalogTester
    DpCatalogTester();

    //! Destroy object DpCatalogTester
    ~DpCatalogTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Initialization/teardown smoke test
    void doInit();

    //! Test tree construction
    void testTree(DpCatalog::DpStateEntry* input, FwIndexType numEntries);

    struct DpSet {
        FwDpIdType id;
        FwDpPriorityType prio;
        Fw::Time time;
        FwSizeType dataSize;
        Fw::DpState state;
        const char* dir;
    };

    //! Read a set of DPs
    //! Runtime addition and stopAfter should not be used together
    void readDps(Fw::FileNameString* dpDirs,
                 FwSizeType numDirs,
                 Fw::FileNameString& stateFile,
                 const DpSet* dpSet,
                 FwSizeType numDps,
                 FwSizeType numRuntime = 0,
                 FwSizeType stopAfter = 0,
                 Fw::Wait wait = Fw::Wait::NO_WAIT);

    //! Verify transmitted state is preserved across catalog rebuilds
    void stateFileSkipsTransmitted();

    //! configure the component on one directory and build the catalog as the first command
    void configureAndBuild(Fw::FileNameString& dir, Fw::FileNameString& stateFile, U32 cmdSeq, Fw::MemAllocator& alloc);

    //! Generate some data product files
    Fw::String genDP(FwDpIdType id,
                     FwDpPriorityType prio,
                     const Fw::Time& time,
                     FwSizeType dataSize,
                     Fw::DpState dpState,
                     bool hdrHashError,
                     const char* dir);

    void delDp(FwDpIdType id, const Fw::Time& time, const char* dir);

    void makeDpDir(const char* dir);

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler implementation for fileOut
    Svc::SendFileResponse from_fileOut_handler(
        FwIndexType portNum,                   //!< The port number
        const Fw::StringBase& sourceFileName,  //!< Path of file to downlink
        const Fw::StringBase& destFileName,    //!< Path to store downlinked file at
        U32 offset,  //!< Amount of data in bytes to downlink from file. 0 to read until end of file
        U32 length   //!< Amount of data in bytes to downlink from file. 0 to read until end of file
        ) override;

    //! Handler implementation for pingOut
    void from_pingOut_handler(FwIndexType portNum,  //!< The port number
                              U32 key               //!< Value to return to pinger
                              ) override;

    void textLogIn(FwEventIdType id,                //!< The event ID
                   const Fw::Time& timeTag,         //!< The time
                   const Fw::LogSeverity severity,  //!< The severity
                   const Fw::TextLogString& text    //!< The event string
                   ) override;

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
    DpCatalog component;

    //! When true, from_fileOut_handler invokes a successful fileDone automatically
    bool m_autoFileDone = true;

    //! Context handed back by from_fileOut_handler for each send, as FileDownlink does
    U32 m_nextContext = 1;

    //! Context of the most recent send, so tests can build matching or stale fileDone responses
    U32 m_lastContext = 0;

  public:
    // ----------------------------------------------------------------------
    // Moved Tests due to private/protected access
    // ----------------------------------------------------------------------
    void test_TreeTestRandomTransmitted();
    void test_TreeTestManual1();
    void test_TreeTestManual2();
    void test_TreeTestManual3();
    void test_TreeTestManual5();
    void test_TreeTestManual1_Transmitted();
    void test_TreeTestManual_All_Transmitted();
    void test_TreeTestRandomPriority();
    void test_TreeTestRandomTime();
    void test_TreeTestRandomId();
    void test_TreeTestRandomPrioIdTime();
    void test_RandomDp();
    void test_XmitBeforeInit();
    void test_StopWarn();
    void test_CompareEntries();
    void test_PingIn();
    void test_BadFileDone();
    void test_StaleFileDoneAfterStopBuild();
    void test_StaleFileDoneAfterClear();
    void test_LateFileDoneNotAppliedToNewSend();
    void test_StopRecordsInFlightCompletion();
    void test_StartAfterStopResumesInFlight();
    void test_StopThenErrorCompletion();
    void test_ProcessFileInvalidDir();
    void test_MalformedFile();

    void test_TruncatedDpRejected();
    void test_NonCanonicalDpRejected();
    void test_BadHeaderHashRejected();
    void test_NonDpFilesDoNotConsumeSlots();
    void test_InsertIntoEmptyBuiltCatalog();
    void test_InsertDuringTransmitInPriorityOrder();
    void test_InsertWhenFull();
    void test_InsertBeforeBuild();
    void test_InsertDuplicate();
    void test_InsertPreservesTransmittedState();
    void test_InsertAlreadyTransmitted();
    void test_XmitPriorityRangeAll();
    void test_XmitPriorityRangeSubset();
    void test_XmitPriorityRangeBoundary();
    void test_XmitPriorityRangeInverted();
    void test_XmitPriorityRangeRuntimeAdd();
    void test_XmitPriorityRangeStopThenRuntimeAdd();
    void test_XmitPriorityRangeStartAfterStopAppliesRangeToNextFile();
    void test_XmitLegacyCommandSendsFullRange();

    //! SET_DP_PRIORITY to a lower number moves the product earlier in the transmit order
    void test_SetPriorityRaise();

    //! SET_DP_PRIORITY to a higher number moves the product later in the transmit order
    void test_SetPriorityLower();

    //! SET_DP_PRIORITY for a product not in the catalog is rejected with DpNotFound
    void test_SetPriorityNotFound();

    //! SET_DP_PRIORITY to the current priority is a reported no-op
    void test_SetPrioritySameNoOp();

    //! SET_DP_PRIORITY during a transmit reorders the pending products and leaves the send in flight intact
    void test_SetPriorityDuringXmit();

    //! SET_DP_PRIORITY change is kept across restart, rebuild and CLEAR_CATALOG
    void test_SetPriorityPersisted();

    //! Runtime addToCat after SET_DP_PRIORITY and after transmit keeps the state file consistent
    void test_SetPriorityRuntimeAdd();

    //! SET_DP_PRIORITY with a state file that cannot be opened for writing
    void test_SetPriorityStateFileOpenError();

    //! SET_DP_PRIORITY with full state file data
    void test_SetPriorityStateFileFull();

    //! SET_DP_PRIORITY with no state file and full state file data: a runtime addToCat is still a duplicate
    void test_SetPriorityNoStateFileTableFull();

    //! Two directories holding a file of the same name at different header priorities: both are transmitted
    //! after SET_DP_PRIORITY moves the one in flight onto the other's priority
    void test_SetPriorityNamesakeDirs();

    //! Two directories holding a file of the same name at the same header priority: both are cataloged and sent
    void test_NamesakeDirsSamePriority();

    //! loadStateFile with more products in the state file than slots: later records for slotted products still
    //! apply, the slotless transmitted product is re-added, and the read offset stays exact past the skipped record
    void test_LoadStateFileSlotExhausted();

    //! loadStateFile with records whose directory index is out of range (high or negative): each is skipped
    //! with StateFileInvalidDirIndex and the in-range records still load
    void test_LoadStateFileInvalidDirIndex();

    //! SET_DP_PRIORITY before configure
    void test_SetPriorityNotInit();

    //! SET_DP_PRIORITY tells apart products of one container (same ID) by their time tags
    void test_SetPrioritySameIdByTime();

    //! SET_DP_PRIORITY moves products into and out of a START_XMIT_CATALOG_WITH_PRIORITY range
    void test_SetPriorityWithXmitRange();
    //! DELETE_DP removes the file and the catalog entry; the product is no longer transmitted
    void test_DeleteDp();
    //! DELETE_DP is refused before configure and reports DpNotFound before the build and for an unknown product
    void test_DeleteDpNotFound();
    //! The product being sent cannot be deleted, before and after STOP; a pending one can
    void test_DeleteDpInFlight();
    //! Deleting a transmitted product drops its state file record, so a rebuild no longer sees it
    void test_DeleteDpStateFileReload();
    void test_DeleteDpStaleDirectory();
    void test_DeleteDpPendingWithStateFile();
    void test_DeleteDpNamesakeDirs();
    //! A file that cannot be removed leaves the catalog untouched
    void test_DeleteDpRemoveError();
    //! A state file that cannot be rewritten fails the command after the product is deleted
    void test_DeleteDpStateFileWriteError();

  private:
    //! Generate one DP per priority (ids 1..numPrios) in dir, configure the component, and build the catalog
    void buildPriorityCatalog(const char* dir, const FwDpPriorityType* prios, FwSizeType numPrios);

    //! Send a waited START_XMIT_CATALOG over [startPriority, endPriority], drain the queue, and check that
    //! exactly the products with expectedPrios were sent, in order
    void xmitPriorityRange(U32 cmdSeq,
                           FwDpPriorityType startPriority,
                           FwDpPriorityType endPriority,
                           const FwDpPriorityType* expectedPrios,
                           FwSizeType numExpected);

    //! Shut the component down and remove the DPs and state file created by buildPriorityCatalog
    void cleanPriorityCatalog(const char* dir, FwSizeType numPrios);

    static const Fw::Time PRIORITY_RANGE_TIME;
    static constexpr FwSizeType PRIORITY_RANGE_DATA_SIZE = 100;  //!< data size of each priority-range test DP

    //! Allocator handed to configure() by buildPriorityCatalog; the component keeps a pointer to it until shutdown()
    Fw::MallocAllocator m_priorityRangeAlloc;

    //! Generate three DPs with priorities 10, 20, 30 in dir, configure the component, and build the catalog
    void setPriorityBuild(Fw::FileNameString& dir, Fw::FileNameString& stateFile, Fw::String (&dpFiles)[3]);

    //! Restart the component and rebuild the catalog
    void setPriorityRebuild(Fw::FileNameString& dir, Fw::FileNameString& stateFile, U32 cmdSeq);

    //! Transmit the whole catalog
    void setPriorityXmitAll(U32 cmdSeq);

    //! generate a file of the same name in two directories (header priorities prio0, prio1), configure on both,
    //! and build the catalog: both files are added
    void namesakeBuild(Fw::FileNameString (&dirs)[2],
                       FwDpPriorityType prio0,
                       FwDpPriorityType prio1,
                       Fw::String (&dpFiles)[2]);

    //! remove the namesake files
    void namesakeClean(Fw::FileNameString (&dirs)[2]);

    //! Size of a generated DP file, as reported by SendingProduct
    U32 setPriorityFileSize(const Fw::String& dpFile);
};

}  // namespace Svc

#endif
