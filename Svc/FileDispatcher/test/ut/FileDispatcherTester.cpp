// ======================================================================
// \title  FileDispatcherTester.cpp
// \author tcanham
// \brief  cpp file for FileDispatcher component test harness implementation class
// ======================================================================

#include "FileDispatcherTester.hpp"

#include <cstring>

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FileDispatcherTester ::FileDispatcherTester()
    : FileDispatcherGTestBase("FileDispatcherTester", FileDispatcherTester::MAX_HISTORY_SIZE),
      component("FileDispatcher") {
    this->initComponents();
    this->connectPorts();
}

FileDispatcherTester ::~FileDispatcherTester() {
    this->component.deinit();
}

void FileDispatcherTester::populateTable(Svc::FileDispatcherTable& table, bool enabled) {
    table.numEntries = Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE;

    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE; i++) {
        table.entries[i].enabled = enabled;
        table.entries[i].fileExt.format(".file%d", i);
        table.entries[i].port = static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
            i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS);
    }

    this->component.configure(table);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FileDispatcherTester ::dispatchTest() {
    Svc::FileDispatcherTable table;
    this->populateTable(table, true);

    // dispatch files

    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE; i++) {
        this->clearHistory();
        Fw::String testFileName;
        testFileName.format("afile.file%d", i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS);
        this->invoke_to_fileAnnounceRecv(0, testFileName);
        this->component.doDispatch();
        ASSERT_EVENTS_FileDispatched_SIZE(1);
        ASSERT_EVENTS_FileDispatched(0, testFileName.toChar(),
                                     static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
                                         i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS));
        ASSERT_from_fileDispatch_SIZE(1);
        ASSERT_from_fileDispatch(0, testFileName);
    }
}

void FileDispatcherTester ::dispatchMaxLengthNameTest() {
    Svc::FileDispatcherTable table;
    this->populateTable(table, true);

    // Build a file name of exactly FileNameStringSize characters that ends in a mapped
    // extension (".file0"). This verifies that a maximum-length file name round-trips
    // through the FileAnnounce -> FileDispatch path without truncation, guarding the
    // `string size FileNameStringSize` sizing of those ports (a smaller port would silently
    // truncate the name here and the assertions below would fail).
    const char ext[] = ".file0";
    const FwSizeType extLen = sizeof(ext) - 1;
    char nameBuf[FileNameStringSize + 1];
    const FwSizeType fillLen = static_cast<FwSizeType>(FileNameStringSize) - extLen;
    (void)::memset(nameBuf, 'a', fillLen);
    (void)::memcpy(nameBuf + fillLen, ext, extLen);
    nameBuf[FileNameStringSize] = '\0';

    Fw::String testFileName(nameBuf);
    ASSERT_EQ(testFileName.length(), static_cast<FwSizeType>(FileNameStringSize));

    this->clearHistory();
    this->invoke_to_fileAnnounceRecv(0, testFileName);
    this->component.doDispatch();

    // The full-length name must dispatch to port 0 with the name intact (no truncation).
    // This is the invariant the FileAnnounce/FileDispatch `string size FileNameStringSize`
    // sizing guarantees.
    ASSERT_from_fileDispatch_SIZE(1);
    ASSERT_from_fileDispatch(0, testFileName);

    // Note: the FileDispatched *event* is intentionally not checked for the full name here.
    // Although the event declares `file_name: string size FileNameStringSize` (240), F Prime
    // event string arguments are carried as Fw::LogStringArg (= FW_LOG_STRING_MAX_SIZE, 200),
    // so any event file name longer than 200 chars is truncated. That is a framework-wide
    // event-string limit, independent of the FileAnnounce/FileDispatch port sizing this test
    // exercises.
    ASSERT_EVENTS_FileDispatched_SIZE(1);
}

void FileDispatcherTester ::dispatchAllDisabledTest() {
    Svc::FileDispatcherTable table;
    this->populateTable(table, false);

    // dispatch files

    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE; i++) {
        this->clearHistory();
        Fw::String testFileName;
        testFileName.format("afile.file%d", i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS);
        this->invoke_to_fileAnnounceRecv(0, testFileName);
        this->component.doDispatch();
        ASSERT_EVENTS_FileDispatched_SIZE(0);
        ASSERT_from_fileDispatch_SIZE(0);
    }
}

void FileDispatcherTester ::dispatchAllCmdDisabledTest() {
    Svc::FileDispatcherTable table;
    this->populateTable(table, true);

    // disable all via command
    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS; i++) {
        this->clearHistory();
        this->sendCmd_ENABLE_DISPATCH(0, 0x10,
                                      static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
                                          i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS),
                                      Fw::Enabled::DISABLED);
        this->component.doDispatch();
        ASSERT_EVENTS_FileDispatchState_SIZE(1);
        ASSERT_EVENTS_FileDispatchState(0,
                                        static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
                                            i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS),
                                        Fw::Enabled::DISABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, 0, 0x10, Fw::CmdResponse::OK);
    }

    // dispatch files

    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE; i++) {
        this->clearHistory();
        Fw::String testFileName;
        testFileName.format("afile.file%d", i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS);
        this->invoke_to_fileAnnounceRecv(0, testFileName);
        this->component.doDispatch();
        ASSERT_EVENTS_FileDispatched_SIZE(0);
        ASSERT_from_fileDispatch_SIZE(0);
    }
}

void FileDispatcherTester ::dispatchAllCmdEnabledTest() {
    Svc::FileDispatcherTable table;
    this->populateTable(table, false);

    // disable all via command
    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS; i++) {
        this->clearHistory();
        this->sendCmd_ENABLE_DISPATCH(0, 0x10,
                                      static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
                                          i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS),
                                      Fw::Enabled::ENABLED);
        this->component.doDispatch();
        ASSERT_EVENTS_FileDispatchState_SIZE(1);
        ASSERT_EVENTS_FileDispatchState(0,
                                        static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
                                            i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS),
                                        Fw::Enabled::ENABLED);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, 0, 0x10, Fw::CmdResponse::OK);
    }

    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE; i++) {
        this->clearHistory();
        Fw::String testFileName;
        testFileName.format("afile.file%d", i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS);
        this->invoke_to_fileAnnounceRecv(0, testFileName);
        this->component.doDispatch();
        ASSERT_EVENTS_FileDispatched_SIZE(1);
        ASSERT_EVENTS_FileDispatched(0, testFileName.toChar(),
                                     static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(
                                         i % Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS));
        ASSERT_from_fileDispatch_SIZE(1);
        ASSERT_from_fileDispatch(0, testFileName);
    }
}

void FileDispatcherTester::dispatchPingTest() {
    // dispatch ping
    this->invoke_to_pingIn(0, 0x1234);
    // dispatch ping message
    this->component.doDispatch();
    // verify return port call
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, 0x1234);
}

//! Handle a text event
void FileDispatcherTester::textLogIn(FwEventIdType id,                //!< The event ID
                                     const Fw::Time& timeTag,         //!< The time
                                     const Fw::LogSeverity severity,  //!< The severity
                                     const Fw::TextLogString& text    //!< The event string
) {
    TextLogEntry e = {id, timeTag, severity, text};

    printTextLogHistoryEntry(e, stdout);
}

}  // namespace Svc
