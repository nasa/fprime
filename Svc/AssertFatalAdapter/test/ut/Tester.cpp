// ======================================================================
// \title  AssertFatalAdapter.hpp
// \author tcanham
// \brief  cpp file for AssertFatalAdapter test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Fw/Types/String.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    Tester::Tester() : AssertFatalAdapterGTestBase("Tester", MAX_HISTORY_SIZE), component( "AssertFatalAdapter")
    {
        this->initComponents();
        this->connectPorts();
    }

    Tester::~Tester() {

    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void Tester::testAsserts() {

        U32 lineNo;
        char file[80 + 1]; // Limit to 80  characters in the port call
        Fw::String fileString;
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        fileString.format("0x%08" PRIX32, ASSERT_FILE_ID);
#else
        fileString = __FILE__;
#endif
        (void) Fw::StringUtils::string_copy(file, fileString.toChar(), sizeof(file));

        // FW_ASSERT_0

        FW_ASSERT(0);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_0_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_0(0,file,lineNo);

        // FW_ASSERT_1

        FW_ASSERT(0,1);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_1_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_1(0,file,lineNo,1);

        // FW_ASSERT_2

        FW_ASSERT(0,1,2);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_2_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_2(0,file,lineNo,1,2);

        // FW_ASSERT_3

        FW_ASSERT(0,1,2,3);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_3_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_3(0,file,lineNo,1,2,3);

        // FW_ASSERT_4

        FW_ASSERT(0,1,2,3,4);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_4_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_4(0,file,lineNo,1,2,3,4);

        // FW_ASSERT_5

        FW_ASSERT(0,1,2,3,4,5);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_5_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_5(0,file,lineNo,1,2,3,4,5);

        // FW_ASSERT_6

        FW_ASSERT(0,1,2,3,4,5,6);lineNo = __LINE__;
        ASSERT_EVENTS_AF_ASSERT_6_SIZE(1);
        ASSERT_EVENTS_AF_ASSERT_6(0,file,lineNo,1,2,3,4,5,6);

        // Test unexpected assert
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
        U32 unexpectedFile = 0xF00;
        const char *const unexpectedFileArg = "0x00000F00";
#else
        const char *const unexpectedFile = "foo";
        const char *const unexpectedFileArg = unexpectedFile;
#endif

        this->component.reportAssert(unexpectedFile,1000,10,1,2,3,4,5,6);
        ASSERT_EVENTS_AF_UNEXPECTED_ASSERT_SIZE(1);
        ASSERT_EVENTS_AF_UNEXPECTED_ASSERT(0,unexpectedFileArg,1000,10);

    }

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    void Tester::connectPorts() {

        // Time
        this->component.set_Time_OutputPort(0, this->get_from_Time(0));

        // Log
        this->component.set_Log_OutputPort(0, this->get_from_Log(0));

        // LogText
        this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    }

    void Tester::initComponents() {
        this->init();
        this->component.init(
        INSTANCE);
    }

    void Tester::textLogIn(const FwEventIdType id, //!< The event ID
            Fw::Time& timeTag, //!< The time
            const Fw::LogSeverity severity, //!< The severity
            const Fw::TextLogString& text //!< The event string
            ) {
        TextLogEntry e = { id, timeTag, severity, text };

        printTextLogHistoryEntry(e, stdout);
    }

} // end namespace Svc
