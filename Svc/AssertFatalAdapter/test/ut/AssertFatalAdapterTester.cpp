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

#include "AssertFatalAdapterTester.hpp"
#include <algorithm>
#include "Fw/Types/String.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "config/FppConstantsAc.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

// Apply all truncations to file buffer size
const FwSizeType FILE_ARG_MAX_LEN =
    FW_MIN(static_cast<FwSizeType>(AssertFatalAdapterEventFileSize),
           FW_MIN(static_cast<FwSizeType>(FW_LOG_STRING_MAX_SIZE), static_cast<FwSizeType>(FW_ASSERT_TEXT_SIZE)));

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AssertFatalAdapterTester::AssertFatalAdapterTester()
    : AssertFatalAdapterGTestBase("Tester", MAX_HISTORY_SIZE), component("AssertFatalAdapter") {
    this->initComponents();
    this->connectPorts();
}

AssertFatalAdapterTester::~AssertFatalAdapterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void doLeadingTruncation(char* dst, const char* src, FwSizeType truncatedSize) {
    FW_ASSERT(src != nullptr);
    FW_ASSERT(dst != nullptr);
    FwSizeType len = Fw::StringUtils::string_length(src, FileNameStringSize);
    // Calculate start index. If string is shorter than N, keep whole string.
    const char* start = (len > truncatedSize) ? src + (len - truncatedSize) : src;
    // Copy safely into output buffer (also ensures null-termination)
    (void)Fw::StringUtils::string_copy(dst, start, std::min(truncatedSize, len) + 1);
}

void AssertFatalAdapterTester::testAsserts() {
    U32 lineNo;

    char file[Fw::StringBase::BUFFER_SIZE(FILE_ARG_MAX_LEN)];
    Fw::String fileString;

// Asserts may be turned off resulting in this component doing a no-op
#if FW_ASSERT_LEVEL == FW_NO_ASSERT
    const int expectedSize = 0;
#else
    const int expectedSize = 1;
#endif

#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    fileString.format("0x%08" PRIX32, ASSERT_FILE_ID);
    (void)Fw::StringUtils::string_copy(file, fileString.toChar(), static_cast<FwSizeType>(sizeof(file)));
#else
    fileString = __FILE__;
    doLeadingTruncation(file, fileString.toChar(), AssertFatalAdapterEventFileSize);
#endif

    // FW_ASSERT_0

    FW_ASSERT(0);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_0_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_0(0, file, lineNo);
    }

    // FW_ASSERT_1

    FW_ASSERT(0, 1);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_1_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_1(0, file, lineNo, 1);
    }

    // FW_ASSERT_2

    FW_ASSERT(0, 1, 2);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_2_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_2(0, file, lineNo, 1, 2);
    }

    // FW_ASSERT_3

    FW_ASSERT(0, 1, 2, 3);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_3_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_3(0, file, lineNo, 1, 2, 3);
    }

    // FW_ASSERT_4

    FW_ASSERT(0, 1, 2, 3, 4);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_4_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_4(0, file, lineNo, 1, 2, 3, 4);
    }

    // FW_ASSERT_5

    FW_ASSERT(0, 1, 2, 3, 4, 5);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_5_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_5(0, file, lineNo, 1, 2, 3, 4, 5);
    }

    // FW_ASSERT_6

    FW_ASSERT(0, 1, 2, 3, 4, 5, 6);
    lineNo = __LINE__ - 1;  // __LINE__ - 1 retrieves the line number of assert above
    ASSERT_EVENTS_AF_ASSERT_6_SIZE(expectedSize);
    if (expectedSize > 0) {
        ASSERT_EVENTS_AF_ASSERT_6(0, file, lineNo, 1, 2, 3, 4, 5, 6);
    }
    // Test unexpected assert
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    U32 unexpectedFile = 0xF00;
    const char* const unexpectedFileArg = "0x00000F00";
#else
    const char* const unexpectedFile = "foo";
    const char* const unexpectedFileArg = unexpectedFile;
#endif

    this->component.reportAssert(unexpectedFile, 1000, 10, 1, 2, 3, 4, 5, 6);
    ASSERT_EVENTS_AF_UNEXPECTED_ASSERT_SIZE(1);
    ASSERT_EVENTS_AF_UNEXPECTED_ASSERT(0, unexpectedFileArg, 1000, 10);
}

void AssertFatalAdapterTester::testTruncation() {
#if FW_ASSERT_LEVEL == FW_FILEID_ASSERT
    // File ID mode doesn't use string paths, skip truncation test
    return;
#else
    constexpr FwSizeType maxSize = FILE_ARG_MAX_LEN;
    ASSERT_LE(maxSize, static_cast<FwSizeType>(FW_ASSERT_TEXT_SIZE));
    // Test 1: Short path should remain unchanged
    const char* shortPath = "test/short.cpp";
    this->clearTextLogs();
    this->component.reportAssert(shortPath, 100, 0, 0, 0, 0, 0, 0, 0);
    ASSERT_EVENTS_AF_ASSERT_0_SIZE(1);
    ASSERT_EVENTS_AF_ASSERT_0(0, shortPath, 100);
    // Verify text log contains the short path
    const std::string& textLog1 = this->getLastTextLog();
    ASSERT_NE(textLog1.find(shortPath), std::string::npos) << "Text log should contain full short path";
    this->clearHistory();

    // Test 2: Path exactly at limit should remain unchanged
    char exactPath[Fw::StringBase::BUFFER_SIZE(maxSize)];
    fillWithBytePattern(exactPath, sizeof(exactPath), true);
    this->clearTextLogs();
    this->component.reportAssert(exactPath, 200, 0, 0, 0, 0, 0, 0, 0);
    ASSERT_EVENTS_AF_ASSERT_0_SIZE(1);
    ASSERT_EVENTS_AF_ASSERT_0(0, exactPath, 200);
    // Verify text log contains the exact path
    const std::string& textLog2 = this->getLastTextLog();
    ASSERT_NE(textLog2.find(exactPath), std::string::npos) << "Text log should contain exact-length path";
    this->clearHistory();

    // Test 3: Long path should be truncated (keeps trailing portion)
    // Create a path longer than maxSize
    constexpr FwSizeType longPathSize = maxSize + 2;
    char longPath[longPathSize + 1];
    fillWithBytePattern(longPath, longPathSize, true);

    // Fill with pattern: "prefix..." + "suffix/truncation_test.cpp"
    const char* suffix = "/suffix/truncation_test.cpp";
    FwSizeType suffixLen = Fw::StringUtils::string_length(suffix, FileNameStringSize);
    FwSizeType prefixLen = longPathSize - maxSize;
    // Fill prefix with repeating digits
    for (FwSizeType i = 0; i < prefixLen; i++) {
        longPath[i] = (i % 10) + 'a';
    }
    Fw::StringUtils::string_copy(longPath + prefixLen + (maxSize - suffixLen), suffix, suffixLen + 1);

    // Expected truncated result: keeps last (maxSize) characters
    char expectedTruncated[Fw::StringBase::BUFFER_SIZE(maxSize)];
    doLeadingTruncation(expectedTruncated, longPath, maxSize);

    this->clearTextLogs();
    this->component.reportAssert(longPath, 300, 1, 42, 0, 0, 0, 0, 0);
    ASSERT_EVENTS_AF_ASSERT_1_SIZE(1);
    ASSERT_EVENTS_AF_ASSERT_1(0, expectedTruncated, 300, 42);
    // Verify text log contains truncated path, not full path
    const std::string& textLog3 = this->getLastTextLog();
    ASSERT_NE(textLog3.find(expectedTruncated), std::string::npos) << "Text log should contain truncated path";
    // Verify the prefix (which should be truncated away) is NOT in the text log
    char prefixOnly[11] = {0};
    Fw::StringUtils::string_copy(prefixOnly, longPath, 11);
    ASSERT_EQ(textLog3.find(prefixOnly), std::string::npos) << "Text log should NOT contain truncated prefix portion";
    this->clearHistory();

    // Test 4: Verify truncation with all assert variants (test AF_ASSERT_3 as example)
    char veryLongPath[longPathSize + 1];
    fillWithBytePattern(veryLongPath, sizeof(veryLongPath), true);

    char expectedTruncated2[Fw::StringBase::BUFFER_SIZE(maxSize)];
    doLeadingTruncation(expectedTruncated2, veryLongPath, maxSize);

    this->clearTextLogs();
    this->component.reportAssert(veryLongPath, 400, 3, 1, 2, 3, 0, 0, 0);
    ASSERT_EVENTS_AF_ASSERT_3_SIZE(1);
    ASSERT_EVENTS_AF_ASSERT_3(0, expectedTruncated2, 400, 1, 2, 3);
    // Verify text log contains truncated path
    const std::string& textLog4 = this->getLastTextLog();
    ASSERT_NE(textLog4.find(expectedTruncated2), std::string::npos)
        << "Text log should contain truncated path for ASSERT_3";
    // Verify the beginning (which should be truncated away) is NOT in the text log
    char beginOnly[11] = {0};
    Fw::StringUtils::string_copy(beginOnly, veryLongPath, 11);
    ASSERT_EQ(textLog4.find(beginOnly), std::string::npos)
        << "Text log should NOT contain truncated beginning for ASSERT_3";
#endif
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void AssertFatalAdapterTester::connectPorts() {
    // Time
    this->component.set_Time_OutputPort(0, this->get_from_Time(0));

    // Log
    this->component.set_Log_OutputPort(0, this->get_from_Log(0));

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));
}

void AssertFatalAdapterTester::initComponents() {
    this->init();
    this->component.init(INSTANCE);
}

void AssertFatalAdapterTester::textLogIn(const FwEventIdType id,          //!< The event ID
                                         const Fw::Time& timeTag,         //!< The time
                                         const Fw::LogSeverity severity,  //!< The severity
                                         const Fw::TextLogString& text    //!< The event string
) {
    TextLogEntry e = {id, timeTag, severity, text};

    // Store the text log message for verification
    this->textLogMessages.push_back(text.toChar());

    printTextLogHistoryEntry(e, stdout);
}

void AssertFatalAdapterTester::clearTextLogs() {
    this->textLogMessages.clear();
}

const std::string& AssertFatalAdapterTester::getLastTextLog() const {
    FW_ASSERT(!this->textLogMessages.empty(), static_cast<FwAssertArgType>(this->textLogMessages.size()));
    return this->textLogMessages.back();
}

void AssertFatalAdapterTester::fillWithBytePattern(char* buffer, FwSizeType size, bool nullTerminate) {
    FW_ASSERT(buffer != nullptr);
    FW_ASSERT(size > 0);

    const FwSizeType fillSize = nullTerminate ? (size - 1) : size;

    // Fill with repeating two-digit pattern: "00112233445566778899001122..."
    // Each position shows (index % 100) in decimal, making byte positions identifiable
    for (FwSizeType i = 0; i < fillSize; i++) {
        const FwSizeType digitValue = (i / 2) % 100;
        if (i % 2 == 0) {
            buffer[i] = '0' + (digitValue / 10);  // Tens digit
        } else {
            buffer[i] = '0' + (digitValue % 10);  // Ones digit
        }
    }

    if (nullTerminate) {
        buffer[fillSize] = '\0';
    }
}

}  // end namespace Svc
