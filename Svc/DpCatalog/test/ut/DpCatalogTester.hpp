// ======================================================================
// \title  DpCatalogTester.hpp
// \author tcanham
// \brief  hpp file for DpCatalog component test harness implementation class
// ======================================================================

#ifndef Svc_DpCatalogTester_HPP
#define Svc_DpCatalogTester_HPP

#include "Svc/DpCatalog/DpCatalogGTestBase.hpp"
#include "Svc/DpCatalog/DpCatalog.hpp"

namespace Svc {

    class DpCatalogTester :
        public DpCatalogGTestBase
    {

    public:

        // ----------------------------------------------------------------------
        // Constants
        // ----------------------------------------------------------------------

        // Maximum size of histories storing events, telemetry, and port outputs
        static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 100;

        // Instance ID supplied to the component instance under test
        static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;

        // Queue depth supplied to the component instance under test
        static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

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

        struct DpSet {
            FwDpIdType id;
            FwDpPriorityType prio;
            Fw::Time time;
            FwSizeType dataSize;
            Fw::DpState state;
            const char* dir;
        };

        //! Read a set of DPs
        void readDps(
            Fw::FileNameString *dpDirs,
            FwSizeType numDirs,
            const DpSet* dpSet,
            FwSizeType numDps
        );

        //! Generate some data product files
        void genDP(
            FwDpIdType id,
            FwDpPriorityType prio,
            const Fw::Time& time,
            FwSizeType dataSize,
            Fw::DpState dpState,
            bool hdrHashError,
            const char *dir
        );

        void delDp(
            FwDpIdType id,
            const Fw::Time& time,
            const char* dir
        );

        void makeDpDir(
            const char* dir
        );

    private:

        // ----------------------------------------------------------------------
        // Handlers for typed from ports
        // ----------------------------------------------------------------------

        //! Handler implementation for fileOut
        Svc::SendFileResponse from_fileOut_handler(
            NATIVE_INT_TYPE portNum, //!< The port number
            const Fw::StringBase& sourceFileName, //!< Path of file to downlink
            const Fw::StringBase& destFileName, //!< Path to store downlinked file at
            U32 offset, //!< Amount of data in bytes to downlink from file. 0 to read until end of file
            U32 length //!< Amount of data in bytes to downlink from file. 0 to read until end of file
        ) override;

        //! Handler implementation for pingOut
        void from_pingOut_handler(
            NATIVE_INT_TYPE portNum, //!< The port number
            U32 key //!< Value to return to pinger
        ) override;

        void textLogIn(
            FwEventIdType id, //!< The event ID
            const Fw::Time& timeTag, //!< The time
            const Fw::LogSeverity severity, //!< The severity
            const Fw::TextLogString& text //!< The event string
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

    };

}

#endif
