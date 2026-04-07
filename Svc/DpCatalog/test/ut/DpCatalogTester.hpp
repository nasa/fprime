// ======================================================================
// \title  DpCatalogTester.hpp
// \author tcanham
// \brief  hpp file for DpCatalog component test harness implementation class
// ======================================================================

#ifndef Svc_DpCatalogTester_HPP
#define Svc_DpCatalogTester_HPP

#include <STest/Pick/Pick.hpp>
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
    void testTree(DpCatalog::DpStateEntry* list, DpCatalog::DpStateEntry* output, FwIndexType numEntries);

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

    //! Handler implementation for productGet
    Fw::Success::T productGet_handler(FwDpIdType id,        //!< The container ID
                                      FwSizeType dataSize,  //!< The data size
                                      Fw::Buffer& buffer    //!< The buffer
                                      ) override;

    //! Handler implementation for productSend
    void productSend_handler(FwDpIdType id,            //!< The container ID
                             const Fw::Buffer& buffer  //!< The buffer
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

    //! Buffer for data product storage
    U8 m_dpBuff[10000];

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
    void test_DeleteDp_NotFound();
    void test_DeleteDp_Success();
    void test_DeleteDp_CurrentlyTransmitting();
    void test_DeleteDp_DuringTransmission();
    void test_DeleteDp_AlreadyTransmitted();
    void test_DeleteDp_ParentPointerIntegrity();
    void test_ChangeDpPriority_NotFound();
    void test_ChangeDpPriority_Success();
    void test_ChangeDpPriority_CurrentlyTransmitting();
    void test_ChangeDpPriority_SamePriority();
    void test_ChangeDpPriority_ReorderTree();
    void test_RetransmitDp_NotFound();
    void test_RetransmitDp_Success_FilePriority();
    void test_RetransmitDp_Success_OverridePriority();
    void test_RetransmitDp_AlreadyInCatalog();
    void test_RetransmitDp_AlreadyInCatalog_FilePriority();
    void test_RetransmitDp_CurrentlyTransmitting();
    void test_RetransmitDp_AfterTransmission();
    void test_ProcessDpFile_InvalidFile();
    void test_ProcessDpFile_InvalidSize();
    void test_ProcessDpFile_InvalidOp();
    void test_ProcessDpFile_DeleteOps();
    void test_ProcessDpFile_ReprioritizeOps();
    void test_ProcessDpFile_RetransmitOps();
    void test_ProcessDpFile_MixedOps();
    void test_SendCatalogDp_EmptyCatalog();
    void test_SendCatalogDp_WithEntries();
    void test_SendCatalogDp_DefaultPriority();
    void test_SendCatalogDp_CustomPriority();
};

}  // namespace Svc

#endif
