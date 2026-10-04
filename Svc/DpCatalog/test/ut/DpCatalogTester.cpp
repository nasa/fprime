// ======================================================================
// \title  DpCatalogTester.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test harness implementation class
// ======================================================================

#include "DpCatalogTester.hpp"
#include <cstdlib>
#include <set>
#include <vector>
#include "Fw/Dp/DpContainer.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "Fw/Types/FileNameString.hpp"
#include "Fw/Types/MallocAllocator.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "config/DpCfg.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

DpCatalogTester ::DpCatalogTester()
    : DpCatalogGTestBase("DpCatalogTester", DpCatalogTester::MAX_HISTORY_SIZE), component("DpCatalog") {
    this->initComponents();
    this->connectPorts();

    // Clear out any garbage left behind
    std::system("rm -rf ./DpTest*");
}

DpCatalogTester ::~DpCatalogTester() {
    this->component.deinit();
    std::system("rm -rf ./DpTest*");
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void DpCatalogTester ::doInit() {
    Fw::MallocAllocator alloc;

    Fw::FileNameString dirs[2];
    dirs[0] = "dir0";
    dirs[1] = "dir1";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, FW_NUM_ARRAY_ELEMENTS(dirs)), stateFile, 100,
                              alloc);
    this->component.shutdown();
}

void DpCatalogTester::testTree(DpCatalog::DpStateEntry* input, FwIndexType numEntries) {
    ASSERT_TRUE(input != nullptr);
    ASSERT_TRUE(numEntries > 0);

    Fw::MallocAllocator alloc;

    Fw::FileNameString dirs[1];
    dirs[0] = "dir0";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, FW_NUM_ARRAY_ELEMENTS(dirs)), stateFile, 100,
                              alloc);

    // reset catalog
    this->component.resetCatalog();

    // add entries
    for (FwIndexType entry = 0; entry < numEntries; entry++) {
        ASSERT_TRUE(this->component.insertEntry(input[entry]));
    }

    // hot wire in progress
    this->component.m_xmitInProgress = true;

    // Collect expected entries (non-transmitted) using input order so that
    // duplicate handling in std::set matches RedBlackTreeSet (first insert wins).
    // std::set sorts via operator< on insertion, so priority ordering is maintained
    // without a separate sort step.
    std::set<DpCatalog::DpStateEntry> expectedEntries;
    for (FwIndexType entry = 0; entry < numEntries; entry++) {
        if (input[entry].record.get_state() != Fw::DpState::TRANSMITTED) {
            expectedEntries.insert(input[entry]);
        }
    }

    // Collect actual entries from catalog
    std::vector<DpCatalog::DpStateEntry> actualEntries;
    DpCatalog::DpStateEntry foundEntry;
    while (this->component.findNextEntry(foundEntry)) {
        actualEntries.push_back(foundEntry);
        // Remove the "sent" entry from catalog
        Fw::Success status = this->component.m_dpCatalog.remove(foundEntry);
        ASSERT_EQ(status, Fw::Success::SUCCESS);
    }

    // Verify we got the right number of entries
    ASSERT_EQ(actualEntries.size(), expectedEntries.size())
        << "Expected " << expectedEntries.size() << " entries, got " << actualEntries.size();

    // Verify all entries match
    size_t i = 0;
    for (const auto& expectedEntry : expectedEntries) {
        ASSERT_EQ(actualEntries[i].record, expectedEntry.record) << "Entry mismatch at sorted index " << i;
        i++;  // Increment the index of the actual entries to walk one at a time in order
    }

    // Verify catalog is now empty
    DpCatalog::DpStateEntry testEntry;
    ASSERT_FALSE(this->component.findNextEntry(testEntry));

    this->component.shutdown();
}

//! Read one DP test
void DpCatalogTester::readDps(Fw::FileNameString* dpDirs,
                              FwSizeType numDirs,
                              Fw::FileNameString& stateFile,
                              const DpSet* dpSet,
                              FwSizeType numDps,
                              FwSizeType numRuntime,
                              FwSizeType stopAfter,
                              Fw::Wait wait) {
    ASSERT_GE(numDps, numRuntime);
    // make a directory for the files
    for (FwSizeType dir = 0; dir < numDirs; dir++) {
        this->makeDpDir(dpDirs[dir].toChar());
    }

    // clean old Dps
    for (FwSizeType dp = 0; dp < numDps; dp++) {
        this->delDp(dpSet[dp].id, dpSet[dp].time, dpSet[dp].dir);

        // Only make non runtime added Dps at this point
        if (dp + numRuntime < numDps) {
            this->genDP(dpSet[dp].id, dpSet[dp].prio, dpSet[dp].time, dpSet[dp].dataSize, dpSet[dp].state, false,
                        dpSet[dp].dir);
        }
    }

    Fw::MallocAllocator alloc;
    this->clearHistory();

    ASSERT_EVENTS_DpFileAdded_SIZE(0);

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dpDirs, numDirs), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    ASSERT_EVENTS_DpFileAdded_SIZE(numDps - numRuntime);

    this->sendCmd_START_XMIT_CATALOG(0, 11, wait, true);

    ASSERT_from_fileOut_SIZE(0);

    // dispatch messages
    for (FwSizeType dp = 0; dp < numDps; dp++) {
        if (stopAfter > 0 && dp > stopAfter) {
            ASSERT_from_fileOut_SIZE(stopAfter);
        } else if (numRuntime == 0) {
            ASSERT_from_fileOut_SIZE(dp);
        }

        // Create a runtime added Dp if we've exhausted all startup Dps
        if (dp + numRuntime >= numDps) {
            Fw::String dpPath = this->genDP(dpSet[dp].id, dpSet[dp].prio, dpSet[dp].time, dpSet[dp].dataSize,
                                            dpSet[dp].state, false, dpSet[dp].dir);
            ASSERT_STRNE(dpPath.toChar(), "");

            // Add the runtime Dp to the catalog
            this->invoke_to_addToCat(0, dpPath, 0, 0);
            this->component.doDispatch();
        }

        // If we've transmitted stopAfter files, then issue the stop seq
        if (dp + 1 == stopAfter) {
            // Stop Transmission
            this->sendCmd_STOP_XMIT_CATALOG(0, 123);
            // Clear the catalog so we don't send upon restart
            this->sendCmd_CLEAR_CATALOG(0, 124);
            // Stop Sequence Complete

            // Ensure we cleared out the catalog
            // Start up and expect an error + no additional xmit
            this->sendCmd_START_XMIT_CATALOG(0, 125, Fw::Wait::NO_WAIT, false);
        }

        // Potentially dispatch file done port call that is sent on fileOut_handler
        // Since files are "instantly" marked done, delay the doDispatch to simulate a delay
        if (stopAfter == 0 && numRuntime > 0) {
            if (STest::Pick::lowerUpper(0, 1) < 1) {
                this->component.doDispatch();
            }
        } else {
            this->component.doDispatch();
        }
    }

    // Finish out any outstanding messages
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    if (stopAfter > 0 && stopAfter < numDps) {
        ASSERT_EVENTS_CatalogXmitCompleted_SIZE(0);
        ASSERT_EVENTS_CatalogXmitStopped_SIZE(1);
        ASSERT_EVENTS_XmitUnbuiltCatalog_SIZE(1);
        ASSERT_from_fileOut_SIZE(stopAfter);

        // BUILD, START, STOP, CLEAR, START
        ASSERT_CMD_RESPONSE_SIZE(5);
        ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
        ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 123, Fw::CmdResponse::OK);
        ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_CLEAR_CATALOG, 124, Fw::CmdResponse::OK);
        // This should fail since we just cleaned up the catalog
        ASSERT_CMD_RESPONSE(4, DpCatalog::OPCODE_START_XMIT_CATALOG, 125, Fw::CmdResponse::EXECUTION_ERROR);
    } else {
        ASSERT_EVENTS_DpFileAdded_SIZE(numDps);
        ASSERT_from_fileOut_SIZE(numDps);

        ASSERT_CMD_RESPONSE_SIZE(2);
        ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);

        if (numRuntime == 0) {
            // Remain active w/ runtime elements would not satisfy this
            ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
        }
    }

    this->component.shutdown();

    // clean old Dps
    for (FwSizeType dp = 0; dp < numDps; dp++) {
        this->delDp(dpSet[dp].id, dpSet[dp].time, dpSet[dp].dir);
    }
}

void DpCatalogTester::stateFileSkipsTransmitted() {
    Fw::FileNameString dir("./DpTest_StateFile");
    Fw::FileNameString stateFile("./DpTest_StateFile/dpState.dat");
    const FwDpIdType id = 0x321;
    const Fw::Time time(2000, 200);
    FwSizeType fileSize = 0;

    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->delDp(id, time, dir.toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    ASSERT_EQ(Os::FileSystem::getFileSize(dpFile.toChar(), fileSize), Os::FileSystem::Status::OP_OK);

    Fw::MallocAllocator alloc;
    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, dpFile.toChar());

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, dpFile, dpFile, 0, 0);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, fileSize);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    this->component.shutdown();

    ASSERT_EQ(Os::FileSystem::getFileSize(dpFile.toChar(), fileSize), Os::FileSystem::Status::OP_OK);

    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 20);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    EXPECT_EQ(this->component.m_pendingFiles, 0);
    EXPECT_EQ(this->component.m_pendingDpBytes, 0);

    this->sendCmd_START_XMIT_CATALOG(0, 21, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(0);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, 0);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 21, Fw::CmdResponse::OK);
    this->component.shutdown();

    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 30);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 30, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    EXPECT_EQ(this->component.m_pendingFiles, 0);
    EXPECT_EQ(this->component.m_pendingDpBytes, 0);
    this->component.shutdown();

    this->delDp(id, time, dir.toChar());
    ASSERT_EQ(Os::FileSystem::removeFile(stateFile.toChar()), Os::FileSystem::Status::OP_OK);
}

Fw::String DpCatalogTester::genDP(FwDpIdType id,
                                  FwDpPriorityType prio,
                                  const Fw::Time& time,
                                  FwSizeType dataSize,
                                  Fw::DpState dpState,
                                  bool hdrHashError,
                                  const char* dir) {
    // Fill DP container
    const FwSizeType packetSize = Fw::DpContainer::getPacketSizeForDataSize(dataSize);
    std::vector<U8> packetData(packetSize);
    Fw::Buffer packetBuffer(packetData.data(), packetSize);
    Fw::DpContainer cont(id, packetBuffer);
    cont.setPriority(prio);
    cont.setTimeTag(time);
    cont.setDpState(dpState);
    cont.setDataSize(dataSize);

    // fill data with ramp
    U8* const dpData = &packetData[Fw::DpContainer::DATA_OFFSET];
    for (FwSizeType byte = 0; byte < dataSize; byte++) {
        dpData[byte] = static_cast<U8>(byte);
    }
    // serialize file data and hashes
    cont.serializeHeader();
    cont.updateDataHash();
    if (hdrHashError) {
        packetData[Fw::DpContainer::HEADER_HASH_OFFSET]++;
    }
    // open file to write data
    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir, id, time.getSeconds(), time.getUSeconds());
    COMMENT(fileName.toChar());
    Os::File dpFile;
    Os::File::Status stat = dpFile.open(fileName.toChar(), Os::File::Mode::OPEN_CREATE);
    if (stat != Os::File::Status::OP_OK) {
        printf("Error opening file %s: status: %d\n", fileName.toChar(), stat);
        return "";
    }
    FwSizeType size = packetSize;
    stat = dpFile.write(packetData.data(), size);
    if (stat != Os::File::Status::OP_OK) {
        printf("Error writing DP file %s: status: %d\n", fileName.toChar(), stat);
        return "";
    }
    if (static_cast<FwSizeType>(size) != packetSize) {
        printf("Dp file %s write size didn't match. Req: %" PRI_FwSizeType "Act: %" PRI_FwSizeType "\n",
               fileName.toChar(), packetSize, size);
        return "";
    }
    dpFile.close();

    return fileName;
}

void DpCatalogTester::delDp(FwDpIdType id, const Fw::Time& time, const char* dir) {
    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir, id, time.getSeconds(), time.getUSeconds());
    Os::FileSystem::removeFile(fileName.toChar());
}

void DpCatalogTester::makeDpDir(const char* dir) {
    Os::FileSystem::Status stat = Os::FileSystem::createDirectory(dir);
    if (stat != Os::FileSystem::Status::OP_OK) {
        printf("Couldn't create directory %s\n", dir);
    }
}

//! Handle a text event
void DpCatalogTester::textLogIn(FwEventIdType id,                //!< The event ID
                                const Fw::Time& timeTag,         //!< The time
                                const Fw::LogSeverity severity,  //!< The severity
                                const Fw::TextLogString& text    //!< The event string
) {
    TextLogEntry e = {id, timeTag, severity, text};

    printTextLogHistoryEntry(e, stdout);
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Svc::SendFileResponse DpCatalogTester ::from_fileOut_handler(FwIndexType portNum,
                                                             const Fw::StringBase& sourceFileName,
                                                             const Fw::StringBase& destFileName,
                                                             U32 offset,
                                                             U32 length) {
    // Accept the send with a distinct context per call, as FileDownlink does, so fileDone can be matched
    this->pushFromPortEntry_fileOut(sourceFileName, destFileName, offset, length);
    this->m_lastContext = this->m_nextContext++;
    const Svc::SendFileResponse resp(Svc::SendFileStatus::STATUS_OK, this->m_lastContext);
    if (this->m_autoFileDone) {
        this->invoke_to_fileDone(0, resp);
    }

    return resp;
}

void DpCatalogTester ::from_pingOut_handler(FwIndexType portNum, U32 key) {
    this->pushFromPortEntry_pingOut(key);
}

// ----------------------------------------------------------------------
// Moved Tests due to private/protected access
// ----------------------------------------------------------------------

void DpCatalogTester ::test_TreeTestRandomTransmitted() {
    static const FwIndexType NUM_ENTRIES = 100;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];

        Svc::DpCatalogTester tester;
        Fw::FileNameString dir;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(randVal);
            randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_id(randVal);
            randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_tSec(randVal);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
            // randomly set if it is untransmitted or partial
            // Transmitted Dps are skipped in processFile
            randVal = STest::Pick::lowerUpper(0, 1);
            if (randVal == 0) {
                inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            } else if (randVal == 1) {
                inputs[entry].record.set_state(Fw::DpState::PARTIAL);
            }
        }

        this->testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestManual1() {
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[1];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    testTree(inputs, 1);
}

void DpCatalogTester ::test_TreeTestManual2() {
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[2];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    inputs[1].record.set_id(2);
    inputs[1].record.set_priority(1);
    inputs[1].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[1].record.set_tSec(1000);
    inputs[1].record.set_tSub(1500);
    inputs[1].record.set_size(100);

    testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
}

void DpCatalogTester ::test_TreeTestManual3() {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[3];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    inputs[1].record.set_id(2);
    inputs[1].record.set_priority(1);
    inputs[1].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[1].record.set_tSec(1000);
    inputs[1].record.set_tSub(1500);
    inputs[1].record.set_size(100);

    inputs[2].record.set_id(3);
    inputs[2].record.set_priority(3);
    inputs[2].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[2].record.set_tSec(1000);
    inputs[2].record.set_tSub(1500);
    inputs[2].record.set_size(100);

    testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
}

void DpCatalogTester ::test_TreeTestManual5() {
    Svc::DpCatalog::DpStateEntry inputs[5];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    inputs[1].record.set_id(2);
    inputs[1].record.set_priority(1);
    inputs[1].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[1].record.set_tSec(1000);
    inputs[1].record.set_tSub(1500);
    inputs[1].record.set_size(100);

    inputs[2].record.set_id(3);
    inputs[2].record.set_priority(3);
    inputs[2].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[2].record.set_tSec(1000);
    inputs[2].record.set_tSub(1500);
    inputs[2].record.set_size(100);

    inputs[3].record.set_id(4);
    inputs[3].record.set_priority(5);
    inputs[3].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[3].record.set_tSec(1000);
    inputs[3].record.set_tSub(1500);
    inputs[3].record.set_size(100);

    inputs[4].record.set_id(5);
    inputs[4].record.set_priority(4);
    inputs[4].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[4].record.set_tSec(1000);
    inputs[4].record.set_tSub(1500);
    inputs[4].record.set_size(100);

    testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
}

void DpCatalogTester ::test_TreeTestRandomPriority() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];

        Svc::DpCatalogTester tester;
        Fw::FileNameString dir;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(randVal);
            inputs[entry].record.set_id(entry);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            inputs[entry].record.set_tSec(1000);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
        }

        tester.testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestRandomTime() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];

        Svc::DpCatalogTester tester;
        Fw::FileNameString dir;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(100);
            inputs[entry].record.set_id(entry);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            inputs[entry].record.set_tSec(randVal);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
        }

        testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestRandomId() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];

        Svc::DpCatalogTester tester;
        Fw::FileNameString dir;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(100);
            inputs[entry].record.set_id(randVal);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            inputs[entry].record.set_tSec(1000);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
        }

        testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestRandomPrioIdTime() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];

        Svc::DpCatalogTester tester;
        Fw::FileNameString dir;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(randVal);
            randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_id(randVal);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_tSec(randVal);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
        }

        tester.testTree(inputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_RandomDp() {
    static constexpr FwIndexType NUM_ENTRIES = DP_MAX_FILES;
    static constexpr FwIndexType NUM_ITERS = 100;
    static constexpr FwIndexType NUM_DIRS = DP_MAX_DIRECTORIES;

    static constexpr FwSizeStoreType MAX_SIZE = 1000;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Fw::FileNameString dirs[NUM_DIRS];

        for (FwIndexType ind = 0; ind < NUM_DIRS; ind++) {
            char tmp[256];
            snprintf(tmp, sizeof(tmp), "./DpTest_Random_%03d", ind);
            dirs[ind] = tmp;
            std::cout << dirs[ind] << std::endl;
        }

        Fw::FileNameString stateFile("./DpTest/dpState.dat");
        Svc::DpCatalogTester::DpSet dpSet[NUM_ENTRIES];

        FwIndexType entries = STest::Pick::startLength(1, NUM_ENTRIES);
        FwIndexType runtimeEntries = STest::Pick::startLength(0, entries);

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < entries; entry++) {
            dpSet[entry].id = STest::Pick::startLength(0, NUM_ENTRIES);
            dpSet[entry].prio = STest::Pick::startLength(0, NUM_ENTRIES);

            dpSet[entry].time.set(STest::Pick::startLength(0, 10000), STest::Pick::startLength(0, 10000));

            dpSet[entry].dataSize = STest::Pick::startLength(0, MAX_SIZE);
            dpSet[entry].dir = dirs[STest::Pick::startLength(0, NUM_DIRS)].toChar();

            // randomly set if it is untransmitted or partial
            // Transmitted Dps are skipped in processFile
            U32 randVal = STest::Pick::lowerUpper(0, 1);
            if (randVal == 0) {
                dpSet[entry].state = Fw::DpState::UNTRANSMITTED;
            } else if (randVal == 1) {
                dpSet[entry].state = Fw::DpState::PARTIAL;
            }
        }

        Fw::Wait wait = static_cast<Fw::Wait::T>(STest::Pick::lowerUpper(0, 1));

        this->readDps(dirs, NUM_DIRS, stateFile, dpSet, entries, runtimeEntries, 0, wait);
    }
}

void DpCatalogTester ::test_XmitBeforeInit() {
    // Start xmit before init
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComponentNotInitialized_SIZE(1);
}

void DpCatalogTester ::test_StopWarn() {
    this->sendCmd_STOP_XMIT_CATALOG(0, 111);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 111, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_XmitNotActive_SIZE(1);
}

void DpCatalogTester ::test_CompareEntries() {
    DpCatalog::DpStateEntry left = {0, {1, 1, 1, 1, 1, 1, Fw::DpState::UNTRANSMITTED}};
    DpCatalog::DpStateEntry right = {0, {1, 1, 2, 1, 1, 1, Fw::DpState::UNTRANSMITTED}};
    FW_ASSERT(right == right);
    FW_ASSERT(left != right);
    FW_ASSERT(left < right);
    FW_ASSERT(right > left);
}

void DpCatalogTester ::test_PingIn() {
    const U32 key = 0xDEADBEEF;
    this->invoke_to_pingIn(0, key);
    this->component.doDispatch();
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, key);
}

void DpCatalogTester ::test_BadFileDone() {
    // With no send in flight a fileDone is stale, whatever its status
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, 0xDEADC0DE));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(1);
    ASSERT_EVENTS_StaleFileDone(0, 0xDEADC0DE, Svc::SendFileStatus::STATUS_ERROR);
    ASSERT_EVENTS_DpFileXmitError_SIZE(0);

    // Now configure with one DP so a transmit stays in flight
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;

    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_BadFileDone";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x111, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    // Suppress the automatic successful fileDone so the transmit stays in flight
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    // Waited command: no response until the transmit finishes
    ASSERT_CMD_RESPONSE_SIZE(1);

    // A failed fileDone for the send in flight halts the transmit and answers the waited command
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileXmitError_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::EXECUTION_ERROR);

    // A further fileDone for that send is stale: no second delayed cmd response
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(2);
    ASSERT_EVENTS_DpFileXmitError_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(2);

    this->delDp(0x111, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_StaleFileDoneAfterStopBuild() {
    // #5777: STOP_XMIT_CATALOG then BUILD_CATALOG while a file is still in flight. The late
    // fileDone for that file used to trip FW_ASSERT(m_hasCurrentXmit) and take the FSW down
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_StaleAfterStopBuild";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x222, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    // Start a transmit and leave the send in flight
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    const U32 abandoned = this->m_lastContext;

    // STOP answers the waited START and itself; BUILD is accepted again
    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 12, Fw::CmdResponse::OK);
    this->sendCmd_BUILD_CATALOG(0, 13);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_BUILD_CATALOG, 13, Fw::CmdResponse::OK);

    // The late fileDone for the abandoned send is reported and ignored
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, abandoned));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(1);
    ASSERT_EVENTS_StaleFileDone(0, abandoned, Svc::SendFileStatus::STATUS_OK);
    ASSERT_EVENTS_ProductComplete_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(4);

    // The rebuilt catalog still transmits the product normally
    this->m_autoFileDone = true;
    this->sendCmd_START_XMIT_CATALOG(0, 14, Fw::Wait::WAIT, false);
    this->component.doDispatch();  // command: sends the file and queues its fileDone
    this->component.doDispatch();  // fileDone: completes the product and the transmit
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(5);
    ASSERT_CMD_RESPONSE(4, DpCatalog::OPCODE_START_XMIT_CATALOG, 14, Fw::CmdResponse::OK);

    this->delDp(0x222, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_StaleFileDoneAfterClear() {
    // CLEAR_CATALOG while a file is in flight, the recovery sdd.md documents, drops the send and
    // closes the transmit session; the late fileDone is reported and ignored rather than asserting
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_StaleAfterClear";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x333, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    const U32 abandoned = this->m_lastContext;

    // CLEAR is accepted mid-transmit and closes the session: the waited START is answered now
    this->sendCmd_CLEAR_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_CLEAR_CATALOG, 12, Fw::CmdResponse::OK);
    // The clear is reported with the state it discards: the one product (a 16-byte payload) was still pending
    ASSERT_EVENTS_CatalogCleared_SIZE(1);
    ASSERT_EVENTS_CatalogCleared(0, 1, Fw::DpContainer::getPacketSizeForDataSize(16));

    // The late fileDone is stale: reported, nothing else
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, abandoned));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(1);
    ASSERT_EVENTS_StaleFileDone(0, abandoned, Svc::SendFileStatus::STATUS_OK);
    ASSERT_EVENTS_ProductComplete_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(3);

    // With the session closed, BUILD is no longer refused as in progress
    this->sendCmd_BUILD_CATALOG(0, 13);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_BUILD_CATALOG, 13, Fw::CmdResponse::OK);

    this->delDp(0x333, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_LateFileDoneNotAppliedToNewSend() {
    // A late fileDone from an abandoned send arrives while a newer send is in flight: it must
    // neither complete nor abort the newer send. Only the matching context is applied
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_LateFileDone";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x444, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    // Send A in flight, then abandon it with STOP + BUILD and start send B of the same product
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    const U32 sendA = this->m_lastContext;
    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    this->sendCmd_BUILD_CATALOG(0, 13);
    this->component.doDispatch();
    this->sendCmd_START_XMIT_CATALOG(0, 14, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(4);
    const U32 sendB = this->m_lastContext;
    ASSERT_NE(sendA, sendB);

    // Late completion of A must not complete B
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, sendA));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(1);
    ASSERT_EVENTS_ProductComplete_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(4);

    // Late error of A must not abort B
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, sendA));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(2);
    ASSERT_EVENTS_StaleFileDone(1, sendA, Svc::SendFileStatus::STATUS_ERROR);
    ASSERT_EVENTS_DpFileXmitError_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(4);

    // StaleFileDone is throttle 10: only 10 of 11 stale callbacks are logged
    for (U32 i = 0; i < 9; i++) {
        this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, sendA));
        this->component.doDispatch();
    }
    ASSERT_EVENTS_StaleFileDone_SIZE(10);
    ASSERT_EVENTS_DpFileXmitError_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(4);

    // B's own completion is applied normally
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, sendB));
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(5);
    ASSERT_CMD_RESPONSE(4, DpCatalog::OPCODE_START_XMIT_CATALOG, 14, Fw::CmdResponse::OK);

    this->delDp(0x444, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_StopRecordsInFlightCompletion() {
    // STOP_XMIT_CATALOG starts no further sends, but the file in flight completes normally and
    // is recorded: it must not be re-sent on the next START_XMIT_CATALOG
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_StopRecords";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x555, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    const U32 inFlight = this->m_lastContext;

    // STOP answers the waited START and itself
    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_EVENTS_CatalogXmitStopped_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 12, Fw::CmdResponse::OK);

    // The in-flight file's completion is still applied, and nothing further is sent
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(0);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(3);

    // A new START has nothing left to send: the product is not re-sent
    this->sendCmd_START_XMIT_CATALOG(0, 13, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_START_XMIT_CATALOG, 13, Fw::CmdResponse::OK);

    this->delDp(0x555, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_StartAfterStopResumesInFlight() {
    // START_XMIT_CATALOG issued after STOP but before the in-flight file completed must not re-send
    // that file: the pending completion resumes the walk, and the session's byte tally is kept
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_StartAfterStop";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    const FwSizeType dataSize = 16;
    const U64 fileSize = Fw::DpContainer::getPacketSizeForDataSize(dataSize);
    Fw::String dpFile = this->genDP(0x666, 10, time, dataSize, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    Fw::String dpFile2 = this->genDP(0x667, 5, time, dataSize, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile2.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    // First product completes, second is in flight
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_from_fileOut_SIZE(2);
    const U32 inFlight = this->m_lastContext;

    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_EVENTS_CatalogXmitStopped_SIZE(1);
    ASSERT_EVENTS_CatalogXmitStopped(0, fileSize);
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 12, Fw::CmdResponse::OK);

    // Re-START while the second file is still in flight: nothing new is sent and the waited
    // command stays pending
    this->sendCmd_START_XMIT_CATALOG(0, 13, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(3);

    // The in-flight completion is applied, the transmit completes with both products counted,
    // and the re-START is answered
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(0);
    ASSERT_EVENTS_ProductComplete_SIZE(2);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, 2 * fileSize);
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_START_XMIT_CATALOG, 13, Fw::CmdResponse::OK);

    this->delDp(0x666, time, dirs[0].toChar());
    this->delDp(0x667, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_StopThenErrorCompletion() {
    // The in-flight file fails after STOP: the error is reported for that file, not as stale, and the
    // waited START (already answered by STOP) gets no second response
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_StopThenError";
    this->makeDpDir(dirs[0].toChar());
    Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x777, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    const U32 inFlight = this->m_lastContext;

    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 12, Fw::CmdResponse::OK);

    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, inFlight));
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileXmitError_SIZE(1);
    ASSERT_EVENTS_DpFileXmitError(0, dpFile.toChar(), Svc::SendFileStatus::STATUS_ERROR);
    ASSERT_EVENTS_StaleFileDone_SIZE(0);
    ASSERT_EVENTS_ProductComplete_SIZE(0);
    ASSERT_CMD_RESPONSE_SIZE(3);

    // The product is still untransmitted: the next START re-sends it
    this->sendCmd_START_XMIT_CATALOG(0, 13, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_START_XMIT_CATALOG, 13, Fw::CmdResponse::OK);

    this->delDp(0x777, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessFileInvalidDir() {
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_InvalidDir";
    Fw::FileNameString stateFile("");
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    ASSERT_DEATH_IF_SUPPORTED(this->component.processFile("somefile.dp", DP_MAX_DIRECTORIES), "Assert");

    this->component.shutdown();
}

void DpCatalogTester::test_MalformedFile() {
    // 1. Setup paths and corrupted data
    Fw::FileNameString stateFile("DpState.dat");

    BYTE buffer[sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE] = {};
    memset(buffer, 0xFF, sizeof(buffer));  // Force deserialization failure

    // 2. Write the malformed data to disk
    Os::File f;
    ASSERT_EQ(Os::File::OP_OK, f.open(stateFile.toChar(), Os::File::OPEN_CREATE, Os::FileInterface::OVERWRITE));

    FwSizeType writeSize = sizeof(buffer);
    ASSERT_EQ(Os::File::OP_OK, f.write(buffer, writeSize));
    f.close();

    // 3. Configure the Component
    Fw::MallocAllocator mockAllocator;
    Fw::FileNameString dirs[DP_MAX_DIRECTORIES];
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 0), stateFile, 0, mockAllocator);

    // 4. Dispatch the BUILD_CATALOG command
    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();

    // 5. Command should fail with an event instead of ASSERT
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // High-priority warning event should be caught by this test
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_FileCorruptedDataError_SIZE(1);
    ASSERT_EVENTS_FileCorruptedDataError(0, stateFile.toChar(), static_cast<I32>(Fw::FW_DESERIALIZE_FORMAT_ERROR));

    // 6. Cleanup
    Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_TruncatedDpRejected() {
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_TruncatedDpRejected");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());

    const FwDpIdType id = 0x123;
    const FwDpPriorityType priority = 10;
    Fw::Time time(1000, 100);
    std::vector<U8> packetData(Fw::DpContainer::MIN_PACKET_SIZE);
    Fw::Buffer packetBuffer(packetData.data(), Fw::DpContainer::MIN_PACKET_SIZE);
    Fw::DpContainer container(id, packetBuffer);
    container.setPriority(priority);
    container.setTimeTag(time);
    container.setDpState(Fw::DpState::UNTRANSMITTED);
    container.setDataSize(0);
    container.serializeHeader();
    container.updateDataHash();

    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir.toChar(), id, time.getSeconds(), time.getUSeconds());
    Os::File dpFile;
    Os::File::Status stat = dpFile.open(fileName.toChar(), Os::File::Mode::OPEN_CREATE);
    ASSERT_EQ(stat, Os::File::Status::OP_OK);
    const FwSizeType headerSize = Fw::DpContainer::Header::SIZE;
    FwSizeType size = headerSize;
    stat = dpFile.write(packetData.data(), size);
    ASSERT_EQ(stat, Os::File::Status::OP_OK);
    ASSERT_EQ(size, headerSize);
    dpFile.close();

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EVENTS_FileReadError_SIZE(1);
    ASSERT_EVENTS_FileReadError(0, fileName.toChar(), static_cast<I32>(Os::File::BAD_SIZE));

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_from_fileOut_SIZE(0);

    this->component.shutdown();
}

void DpCatalogTester::test_NonCanonicalDpRejected() {
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_NonCanonicalDpRejected");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());

    Fw::Time time(1000, 100);
    Fw::String canonicalFile = this->genDP(0x123, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(canonicalFile.toChar(), "");

    Fw::String rogueFile;
    rogueFile.format("%s/rogue.fdp", dir.toChar());
    ASSERT_EQ(Os::FileSystem::moveFile(canonicalFile.toChar(), rogueFile.toChar()), Os::FileSystem::OP_OK);

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EVENTS_InvalidFileName_SIZE(1);
    ASSERT_EVENTS_InvalidFileName(0, rogueFile.toChar(), canonicalFile.toChar());

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_from_fileOut_SIZE(0);

    this->component.shutdown();
}

void DpCatalogTester::test_NonDpFilesDoNotConsumeSlots() {
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_NonDpFiles");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());

    // fill the directory with as many non-DP files as there are catalog slots
    for (FwIndexType junk = 0; junk < DP_MAX_FILES; junk++) {
        Fw::String junkName;
        junkName.format("%s/junk_%03" PRI_FwIndexType ".txt", dir.toChar(), junk);
        Os::File junkFile;
        ASSERT_EQ(junkFile.open(junkName.toChar(), Os::File::Mode::OPEN_CREATE), Os::File::Status::OP_OK);
        junkFile.close();
    }

    // generate enough DP files to fill every catalog slot
    Fw::Time time(1000, 100);
    for (FwIndexType dp = 0; dp < DP_MAX_FILES; dp++) {
        Fw::String dpFile =
            this->genDP(static_cast<FwDpIdType>(dp), 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
        ASSERT_STRNE(dpFile.toChar(), "");
    }

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    // every DP file must be cataloged despite the non-DP files
    ASSERT_EVENTS_DpFileAdded_SIZE(DP_MAX_FILES);
    ASSERT_EVENTS_CatalogFull_SIZE(1);

    this->component.shutdown();
}

void DpCatalogTester::test_BadHeaderHashRejected() {
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_BadHeaderHashRejected");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());

    Fw::Time time(1000, 100);
    Fw::String fileName = this->genDP(0x123, 10, time, 16, Fw::DpState::UNTRANSMITTED, true, dir.toChar());
    ASSERT_STRNE(fileName.toChar(), "");

    // Rebuild the same header to derive the expected (computed) and corrupted (stored) header hashes
    const FwSizeType packetSize = Fw::DpContainer::getPacketSizeForDataSize(16);
    std::vector<U8> packetData(packetSize);
    Fw::Buffer packetBuffer(packetData.data(), packetSize);
    Fw::DpContainer cont(0x123, packetBuffer);
    cont.setPriority(10);
    cont.setTimeTag(time);
    cont.setDpState(Fw::DpState::UNTRANSMITTED);
    cont.setDataSize(16);
    cont.serializeHeader();
    const U32 computedHash = cont.getHeaderHash().asBigEndianU32();
    packetData[Fw::DpContainer::HEADER_HASH_OFFSET]++;
    const U32 storedHash = cont.getHeaderHash().asBigEndianU32();

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EVENTS_FileHdrError_SIZE(1);
    ASSERT_EVENTS_FileHdrError(0, fileName.toChar(), DpHdrField::CRC, computedHash, storedHash);

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_from_fileOut_SIZE(0);

    this->component.shutdown();
}

void DpCatalogTester ::configureAndBuild(Fw::FileNameString* dirs,
                                         FwSizeType numDirs,
                                         Fw::FileNameString& stateFile,
                                         Fw::MemAllocator& alloc,
                                         U32 cmdSeq,
                                         FwSizeType expectedAdded) {
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, numDirs), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, cmdSeq);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, cmdSeq, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(expectedAdded);
}

void DpCatalogTester ::test_DeleteDp() {
    // DELETE_DP removes the file and the catalog entry; the product is no longer transmitted
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_Delete";
    this->makeDpDir(dirs[0].toChar());
    const Fw::Time timeA(1000, 100);
    const Fw::Time timeB(1000, 200);
    Fw::String fileA = this->genDP(0x600, 10, timeA, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    Fw::String fileB = this->genDP(0x601, 20, timeB, 32, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(fileA.toChar(), "");
    ASSERT_STRNE(fileB.toChar(), "");
    FwSizeType sizeA = 0;
    FwSizeType sizeB = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(fileA.toChar(), sizeA), Os::FileSystem::Status::OP_OK);
    ASSERT_EQ(Os::FileSystem::getFileSize(fileB.toChar(), sizeB), Os::FileSystem::Status::OP_OK);
    this->configureAndBuild(dirs, 1, stateFile, alloc, 10, 2);
    EXPECT_EQ(this->component.m_pendingFiles, 2);
    EXPECT_EQ(this->component.m_pendingDpBytes, sizeA + sizeB);

    this->sendCmd_DELETE_DP(0, 11, 0x600, timeA.getSeconds(), timeA.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, fileA.toChar(), Fw::DpState::UNTRANSMITTED);
    ASSERT_EVENTS_DpDeleteError_SIZE(0);
    ASSERT_EVENTS_DpFileRemoveError_SIZE(0);
    ASSERT_FALSE(Os::FileSystem::exists(fileA.toChar()));
    ASSERT_TRUE(Os::FileSystem::exists(fileB.toChar()));
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);
    EXPECT_EQ(this->component.m_pendingFiles, 1);
    EXPECT_EQ(this->component.m_pendingDpBytes, sizeB);

    // Only the remaining product is transmitted
    this->sendCmd_START_XMIT_CATALOG(0, 12, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, fileB, fileB, 0, 0);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, sizeB);
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_START_XMIT_CATALOG, 12, Fw::CmdResponse::OK);
    EXPECT_EQ(this->component.m_pendingFiles, 0);
    EXPECT_EQ(this->component.m_pendingDpBytes, 0);

    this->delDp(0x601, timeB, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_DeleteDpNotFound() {
    // DELETE_DP is rejected before configure, before the catalog is built, and for an unknown product;
    // nothing is touched
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_DeleteNotFound";
    this->makeDpDir(dirs[0].toChar());
    const Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x610, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");

    // Before configure: refused by checkInit, with no DpDeleteError
    this->sendCmd_DELETE_DP(0, 9, 0x610, time.getSeconds(), time.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 9, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_ComponentNotInitialized_SIZE(1);
    ASSERT_EVENTS_DpDeleteError_SIZE(0);
    ASSERT_TRUE(Os::FileSystem::exists(dpFile.toChar()));
    this->clearHistory();

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 1), stateFile, 100, alloc);

    this->sendCmd_DELETE_DP(0, 10, 0x610, time.getSeconds(), time.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 10, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(1);
    ASSERT_EVENTS_DpDeleteError(0, 0x610, time.getSeconds(), time.getUSeconds(), Svc::DpDeleteReason::NOT_BUILT);
    ASSERT_TRUE(Os::FileSystem::exists(dpFile.toChar()));

    this->sendCmd_BUILD_CATALOG(0, 11);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);

    // Same id, different time stamp: not the same product
    this->sendCmd_DELETE_DP(0, 12, 0x610, time.getSeconds(), time.getUSeconds() + 1);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(2);
    ASSERT_EVENTS_DpDeleteError(1, 0x610, time.getSeconds(), time.getUSeconds() + 1, Svc::DpDeleteReason::NOT_FOUND);
    ASSERT_EVENTS_DpDeleted_SIZE(0);
    ASSERT_TRUE(Os::FileSystem::exists(dpFile.toChar()));
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);
    EXPECT_EQ(this->component.m_pendingFiles, 1);

    this->delDp(0x610, time, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_DeleteDpInFlight() {
    // The product being sent cannot be deleted; a pending product deleted mid-transmit is never
    // sent and the walk completes over what is left. After STOP, the pending completion still
    // protects the product in flight
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_DeleteInFlight";
    this->makeDpDir(dirs[0].toChar());
    const Fw::Time timeA(1000, 100);
    const Fw::Time timeB(1000, 200);
    Fw::String fileA = this->genDP(0x620, 10, timeA, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    Fw::String fileB = this->genDP(0x621, 20, timeB, 32, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(fileA.toChar(), "");
    ASSERT_STRNE(fileB.toChar(), "");
    FwSizeType sizeA = 0;
    FwSizeType sizeB = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(fileA.toChar(), sizeA), Os::FileSystem::Status::OP_OK);
    ASSERT_EQ(Os::FileSystem::getFileSize(fileB.toChar(), sizeB), Os::FileSystem::Status::OP_OK);
    this->configureAndBuild(dirs, 1, stateFile, alloc, 10, 2);

    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(2);
    const U32 inFlight = this->m_lastContext;
    // Whichever product went first is in flight; the other is still pending
    const bool aInFlight = (this->fromPortHistory_fileOut->at(0).sourceFileName == fileA);
    const FwDpIdType sentId = aInFlight ? 0x620 : 0x621;
    const FwDpIdType pendingId = aInFlight ? 0x621 : 0x620;
    const Fw::Time& sentTime = aInFlight ? timeA : timeB;
    const Fw::Time& pendingTime = aInFlight ? timeB : timeA;
    const Fw::String& sentFile = aInFlight ? fileA : fileB;
    const Fw::String& pendingFile = aInFlight ? fileB : fileA;
    const FwSizeType sentSize = aInFlight ? sizeA : sizeB;

    this->sendCmd_DELETE_DP(0, 12, sentId, sentTime.getSeconds(), sentTime.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(1);
    ASSERT_EVENTS_DpDeleteError(0, sentId, sentTime.getSeconds(), sentTime.getUSeconds(),
                                Svc::DpDeleteReason::IN_FLIGHT);
    ASSERT_TRUE(Os::FileSystem::exists(sentFile.toChar()));

    // The pending product can be deleted while the other is being sent
    this->sendCmd_DELETE_DP(0, 13, pendingId, pendingTime.getSeconds(), pendingTime.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_DELETE_DP, 13, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, pendingFile.toChar(), Fw::DpState::UNTRANSMITTED);
    ASSERT_FALSE(Os::FileSystem::exists(pendingFile.toChar()));
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);
    EXPECT_EQ(this->component.m_pendingFiles, 1);

    // The in-flight completion resumes the walk, which finds nothing left and completes
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_EVENTS_StaleFileDone_SIZE(0);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, sentSize);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 0);
    EXPECT_EQ(this->component.m_pendingFiles, 0);
    EXPECT_EQ(this->component.m_pendingDpBytes, 0);
    this->delDp(sentId, sentTime, dirs[0].toChar());

    // Second round: STOP leaves the send in flight, so the product stays protected until it completes
    this->clearHistory();
    const Fw::Time timeC(1000, 300);
    const Fw::Time timeD(1000, 400);
    Fw::String fileC = this->genDP(0x622, 10, timeC, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    Fw::String fileD = this->genDP(0x623, 20, timeD, 32, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(fileC.toChar(), "");
    ASSERT_STRNE(fileD.toChar(), "");
    this->sendCmd_CLEAR_CATALOG(0, 20);
    this->component.doDispatch();
    this->sendCmd_BUILD_CATALOG(0, 21);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_BUILD_CATALOG, 21, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(2);

    this->sendCmd_START_XMIT_CATALOG(0, 22, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(3);
    const U32 inFlight2 = this->m_lastContext;
    const bool cInFlight = (this->fromPortHistory_fileOut->at(0).sourceFileName == fileC);
    const FwDpIdType sentId2 = cInFlight ? 0x622 : 0x623;
    const Fw::Time& sentTime2 = cInFlight ? timeC : timeD;
    const Fw::String& sentFile2 = cInFlight ? fileC : fileD;

    this->sendCmd_STOP_XMIT_CATALOG(0, 23);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    this->sendCmd_DELETE_DP(0, 24, sentId2, sentTime2.getSeconds(), sentTime2.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(5);
    ASSERT_CMD_RESPONSE(4, DpCatalog::OPCODE_DELETE_DP, 24, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(1);
    ASSERT_EVENTS_DpDeleteError(0, sentId2, sentTime2.getSeconds(), sentTime2.getUSeconds(),
                                Svc::DpDeleteReason::IN_FLIGHT);
    ASSERT_TRUE(Os::FileSystem::exists(sentFile2.toChar()));
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 2);

    // Once the completion arrives the product is no longer in flight and can be deleted
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight2));
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_from_fileOut_SIZE(1);
    this->sendCmd_DELETE_DP(0, 25, sentId2, sentTime2.getSeconds(), sentTime2.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(6);
    ASSERT_CMD_RESPONSE(5, DpCatalog::OPCODE_DELETE_DP, 25, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, sentFile2.toChar(), Fw::DpState::TRANSMITTED);
    ASSERT_FALSE(Os::FileSystem::exists(sentFile2.toChar()));
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);

    this->delDp(0x622, timeC, dirs[0].toChar());
    this->delDp(0x623, timeD, dirs[0].toChar());
    this->component.shutdown();
}

void DpCatalogTester ::test_DeleteDpStateFileReload() {
    // Deleting a transmitted product drops its state file record, so a rebuild no longer sees it;
    // a product whose file already vanished is still removed from the state file
    Fw::FileNameString dir("./DpTest_DeleteState");
    Fw::FileNameString stateFile("./DpTest_DeleteState/dpState.dat");
    const FwSizeType entrySize = sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE;
    const Fw::Time timeA(2000, 100);
    const Fw::Time timeB(2000, 200);
    FwSizeType size = 0;

    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    Fw::String fileA = this->genDP(0x630, 10, timeA, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    Fw::String fileB = this->genDP(0x631, 20, timeB, 32, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(fileA.toChar(), "");
    ASSERT_STRNE(fileB.toChar(), "");

    Fw::MallocAllocator alloc;
    this->configureAndBuild(&dir, 1, stateFile, alloc, 10, 2);

    // Transmit both: the state file now records two transmitted products
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_EVENTS_ProductComplete_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), size), Os::FileSystem::Status::OP_OK);
    EXPECT_EQ(size, 2 * entrySize);

    // Delete a transmitted product: file and state file record are gone
    this->sendCmd_DELETE_DP(0, 12, 0x630, timeA.getSeconds(), timeA.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, fileA.toChar(), Fw::DpState::TRANSMITTED);
    ASSERT_FALSE(Os::FileSystem::exists(fileA.toChar()));
    ASSERT_TRUE(Os::FileSystem::exists(fileB.toChar()));
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), size), Os::FileSystem::Status::OP_OK);
    EXPECT_EQ(size, entrySize);
    this->component.shutdown();

    // Simulated reboot: only the remaining transmitted product is known
    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 20);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, fileB.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    EXPECT_EQ(this->component.m_pendingFiles, 0);

    // The file vanished out from under the catalog: the state file record is still removed
    this->delDp(0x631, timeB, dir.toChar());
    this->sendCmd_DELETE_DP(0, 21, 0x631, timeB.getSeconds(), timeB.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 21, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, fileB.toChar(), Fw::DpState::TRANSMITTED);
    ASSERT_EVENTS_DpFileRemoveError_SIZE(0);
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), size), Os::FileSystem::Status::OP_OK);
    EXPECT_EQ(size, 0);

    // Nothing is left to find after another reload
    this->component.shutdown();
    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 30);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped_SIZE(0);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    this->sendCmd_DELETE_DP(0, 31, 0x631, timeB.getSeconds(), timeB.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 31, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(1);
    ASSERT_EVENTS_DpDeleteError(0, 0x631, timeB.getSeconds(), timeB.getUSeconds(), Svc::DpDeleteReason::NOT_FOUND);
    this->component.shutdown();

    ASSERT_EQ(Os::FileSystem::removeFile(stateFile.toChar()), Os::FileSystem::Status::OP_OK);
}

void DpCatalogTester ::test_DeleteDpRemoveError() {
    // The file cannot be removed: DpFileRemoveError, EXECUTION_ERROR, catalog left untouched
    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_DeleteRemoveError";
    this->makeDpDir(dirs[0].toChar());
    const Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x640, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[0].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    FwSizeType size = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(dpFile.toChar(), size), Os::FileSystem::Status::OP_OK);
    this->configureAndBuild(dirs, 1, stateFile, alloc, 10, 1);

    // Replace the cataloged file with a directory of the same name so removal fails (not DOESNT_EXIST)
    this->delDp(0x640, time, dirs[0].toChar());
    ASSERT_EQ(Os::FileSystem::createDirectory(dpFile.toChar()), Os::FileSystem::Status::OP_OK);
    const Os::FileSystem::Status expected = Os::FileSystem::removeFile(dpFile.toChar());
    ASSERT_NE(expected, Os::FileSystem::Status::OP_OK);
    ASSERT_NE(expected, Os::FileSystem::Status::DOESNT_EXIST);

    this->sendCmd_DELETE_DP(0, 11, 0x640, time.getSeconds(), time.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpFileRemoveError_SIZE(1);
    ASSERT_EVENTS_DpFileRemoveError(0, dpFile.toChar(), static_cast<I32>(expected));
    ASSERT_EVENTS_DpDeleted_SIZE(0);
    ASSERT_EVENTS_DpDeleteError_SIZE(0);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);
    EXPECT_EQ(this->component.m_pendingFiles, 1);
    EXPECT_EQ(this->component.m_pendingDpBytes, size);

    ASSERT_EQ(Os::FileSystem::removeDirectory(dpFile.toChar()), Os::FileSystem::Status::OP_OK);
    this->component.shutdown();
}

void DpCatalogTester ::test_DeleteDpNameError() {
    // A state file record whose directory index is no longer configured cannot be named:
    // rejected as NAME_ERROR and the in-memory state table is left untouched
    Fw::FileNameString dir("./DpTest_DeleteNameError");
    Fw::FileNameString stateFile("./DpTest_DeleteNameError/dpState.dat");
    this->makeDpDir(dir.toChar());
    const FwIndexType staleDir = 1;  // only directory 0 is configured below
    const DpRecord record(0x650, 3000, 100, 10, 64, 0, Fw::DpState::TRANSMITTED);
    BYTE buffer[sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE] = {};
    Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(entryBuffer.serializeFrom(staleDir), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(entryBuffer.serializeFrom(record), Fw::FW_SERIALIZE_OK);
    Os::File f;
    ASSERT_EQ(f.open(stateFile.toChar(), Os::File::OPEN_CREATE, Os::FileInterface::OVERWRITE), Os::File::OP_OK);
    FwSizeType size = entryBuffer.getSize();
    ASSERT_EQ(f.write(buffer, size), Os::File::OP_OK);
    f.close();

    Fw::MallocAllocator alloc;
    this->configureAndBuild(&dir, 1, stateFile, alloc, 10, 0);
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    this->sendCmd_DELETE_DP(0, 11, 0x650, 3000, 100);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(1);
    ASSERT_EVENTS_DpDeleteError(0, 0x650, 3000, 100, Svc::DpDeleteReason::NAME_ERROR);
    ASSERT_EVENTS_DpDeleted_SIZE(0);
    ASSERT_EVENTS_DpFileRemoveError_SIZE(0);
    EXPECT_EQ(this->component.m_stateFileEntries, 1);

    this->component.shutdown();
    ASSERT_EQ(Os::FileSystem::removeFile(stateFile.toChar()), Os::FileSystem::Status::OP_OK);
}

void DpCatalogTester ::test_DeleteDpStateTableRecycle() {
    // With the loaded state data full, a transmitted record reuses the slot of a loaded record whose
    // file is gone; both transmitted products stay deletable
    Fw::FileNameString dir("./DpTest_DeleteRecycle");
    Fw::FileNameString stateFile("./DpTest_DeleteRecycle/dpState.dat");
    this->makeDpDir(dir.toChar());
    const FwIndexType staleDir = 0;
    const DpRecord stale(0x670, 2000, 100, 10, 64, 0, Fw::DpState::TRANSMITTED);
    BYTE buffer[sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE] = {};
    Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));
    ASSERT_EQ(entryBuffer.serializeFrom(staleDir), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(entryBuffer.serializeFrom(stale), Fw::FW_SERIALIZE_OK);
    Os::File f;
    ASSERT_EQ(f.open(stateFile.toChar(), Os::File::OPEN_CREATE, Os::FileInterface::OVERWRITE), Os::File::OP_OK);
    FwSizeType size = entryBuffer.getSize();
    ASSERT_EQ(f.write(buffer, size), Os::File::OP_OK);
    f.close();

    const Fw::Time timeA(1000, 100);
    const Fw::Time timeB(1000, 200);
    Fw::String fileA = this->genDP(0x671, 10, timeA, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    Fw::String fileB = this->genDP(0x672, 20, timeB, 32, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(fileA.toChar(), "");
    ASSERT_STRNE(fileB.toChar(), "");
    Fw::MallocAllocator alloc;
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    // Two slots (the allocator grants the full request): the stale record and one transmitted product fill them
    this->component.m_numDpSlots = 2;
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(2);
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_DpStateRecordDropped_SIZE(0);
    ASSERT_EQ(this->component.m_stateFileEntries, 2);
    ASSERT_CMD_RESPONSE_SIZE(2);

    this->sendCmd_DELETE_DP(0, 12, 0x671, timeA.getSeconds(), timeA.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, fileA.toChar(), Fw::DpState::TRANSMITTED);
    ASSERT_FALSE(Os::FileSystem::exists(fileA.toChar()));
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    this->sendCmd_DELETE_DP(0, 13, 0x672, timeB.getSeconds(), timeB.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_DELETE_DP, 13, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(2);
    ASSERT_EVENTS_DpDeleted(1, fileB.toChar(), Fw::DpState::TRANSMITTED);
    ASSERT_FALSE(Os::FileSystem::exists(fileB.toChar()));
    ASSERT_EQ(this->component.m_stateFileEntries, 0);
    FwSizeType stateSize = 1;
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), stateSize), Os::FileSystem::Status::OP_OK);
    ASSERT_EQ(stateSize, 0);

    this->component.shutdown();
    ASSERT_EQ(Os::FileSystem::removeFile(stateFile.toChar()), Os::FileSystem::Status::OP_OK);
}

void DpCatalogTester ::test_DeleteDpStateTableFull() {
    // With the loaded state data full of records whose files exist, a transmitted record is not kept
    // in memory: DpStateRecordDropped, NOT_FOUND until the next BUILD_CATALOG, then cataloged again
    // as untransmitted and deletable. The drop event is throttled and re-armed by BUILD_CATALOG
    Fw::FileNameString dir("./DpTest_DeleteFull");
    Fw::FileNameString stateFile("./DpTest_DeleteFull/dpState.dat");
    this->makeDpDir(dir.toChar());
    const Fw::Time timeA(1000, 100);
    const Fw::Time timeB(1000, 200);
    Fw::String fileA = this->genDP(0x680, 10, timeA, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(fileA.toChar(), "");
    Fw::MallocAllocator alloc;
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    // One slot (the allocator grants the full request): the first transmitted product fills it
    this->component.m_numDpSlots = 1;
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_DpStateRecordDropped_SIZE(0);
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    // A second product arrives at runtime and is transmitted; its record cannot be kept in memory
    Fw::String fileB = this->genDP(0x681, 20, timeB, 32, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(fileB.toChar(), "");
    this->invoke_to_addToCat(0, fileB, 0, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileAdded_SIZE(2);
    this->sendCmd_START_XMIT_CATALOG(0, 12, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_EVENTS_DpStateRecordDropped_SIZE(1);
    ASSERT_EVENTS_DpStateRecordDropped(0, 0x681, timeB.getSeconds(), timeB.getUSeconds());
    ASSERT_EQ(this->component.m_stateFileEntries, 1);
    ASSERT_CMD_RESPONSE_SIZE(3);

    this->sendCmd_DELETE_DP(0, 13, 0x681, timeB.getSeconds(), timeB.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_DELETE_DP, 13, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpDeleteError_SIZE(1);
    ASSERT_EVENTS_DpDeleteError(0, 0x681, timeB.getSeconds(), timeB.getUSeconds(), Svc::DpDeleteReason::NOT_FOUND);
    ASSERT_TRUE(Os::FileSystem::exists(fileB.toChar()));

    // After a rebuild the product is cataloged again (its record was not loaded) and can be deleted
    this->clearHistory();
    this->sendCmd_CLEAR_CATALOG(0, 14);
    this->component.doDispatch();
    this->sendCmd_BUILD_CATALOG(0, 15);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_BUILD_CATALOG, 15, Fw::CmdResponse::OK);
    this->sendCmd_DELETE_DP(0, 16, 0x681, timeB.getSeconds(), timeB.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_DELETE_DP, 16, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, fileB.toChar(), Fw::DpState::UNTRANSMITTED);
    ASSERT_FALSE(Os::FileSystem::exists(fileB.toChar()));

    // Delete the loaded record too, so the first transmitted product fills the single slot and
    // every later one is dropped
    this->sendCmd_DELETE_DP(0, 17, 0x680, timeA.getSeconds(), timeA.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(4);
    ASSERT_CMD_RESPONSE(3, DpCatalog::OPCODE_DELETE_DP, 17, Fw::CmdResponse::OK);
    ASSERT_EQ(this->component.m_stateFileEntries, 0);

    // Throttle: only the first 10 drops are reported, then a rebuild re-arms the event
    const U32 throttle = 10;
    this->clearHistory();
    for (U32 drop = 0; drop < throttle + 2; drop++) {
        const Fw::Time timeN(1000, 500 + drop);
        Fw::String fileN = this->genDP(0x682, 20, timeN, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
        ASSERT_STRNE(fileN.toChar(), "");
        this->invoke_to_addToCat(0, fileN, 0, 0);
        this->component.doDispatch();
        this->sendCmd_START_XMIT_CATALOG(0, 20 + drop, Fw::Wait::WAIT, false);
        while (this->component.m_queue.getMessagesAvailable() > 0) {
            this->component.doDispatch();
        }
        // The first product keeps its file so its loaded record is not reusable after the rebuild
        if (drop > 0) {
            this->delDp(0x682, timeN, dir.toChar());
        }
    }
    ASSERT_from_fileOut_SIZE(throttle + 2);
    ASSERT_EVENTS_DpStateRecordDropped_SIZE(throttle);
    ASSERT_EVENTS_DpStateRecordDropped(0, 0x682, 1000, 501);
    ASSERT_EVENTS_DpStateRecordDropped(throttle - 1, 0x682, 1000, 500 + throttle);
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    this->clearHistory();
    this->sendCmd_CLEAR_CATALOG(0, 40);
    this->component.doDispatch();
    this->sendCmd_BUILD_CATALOG(0, 41);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_BUILD_CATALOG, 41, Fw::CmdResponse::OK);
    const Fw::Time timeC(1000, 600);
    Fw::String fileC = this->genDP(0x683, 20, timeC, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(fileC.toChar(), "");
    this->invoke_to_addToCat(0, fileC, 0, 0);
    this->component.doDispatch();
    this->sendCmd_START_XMIT_CATALOG(0, 42, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_EVENTS_DpStateRecordDropped_SIZE(1);
    ASSERT_EVENTS_DpStateRecordDropped(0, 0x683, timeC.getSeconds(), timeC.getUSeconds());
    this->delDp(0x683, timeC, dir.toChar());
    this->delDp(0x682, Fw::Time(1000, 500), dir.toChar());

    this->component.shutdown();
    ASSERT_EQ(Os::FileSystem::removeFile(stateFile.toChar()), Os::FileSystem::Status::OP_OK);
}

void DpCatalogTester ::test_DeleteDpStateFileWriteError() {
    // The state file cannot be rewritten: the file is deleted and DpDeleted emitted, but the command
    // completes with EXECUTION_ERROR after the StateFileOpenError
    Fw::FileNameString dir("./DpTest_DeleteWriteError");
    Fw::FileNameString stateFile("./DpTest_DeleteWriteError/missing/dpState.dat");
    this->makeDpDir(dir.toChar());
    const Fw::Time time(1000, 100);
    Fw::String dpFile = this->genDP(0x690, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    Fw::MallocAllocator alloc;
    this->configureAndBuild(&dir, 1, stateFile, alloc, 10, 1);

    // Transmit the product so it is held in the loaded state data only
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EQ(this->component.m_stateFileEntries, 1);
    this->clearHistory();

    this->sendCmd_DELETE_DP(0, 12, 0x690, time.getSeconds(), time.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_StateFileOpenError_SIZE(1);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpDeleted(0, dpFile.toChar(), Fw::DpState::TRANSMITTED);
    ASSERT_EVENTS_DpDeleteError_SIZE(0);
    ASSERT_EVENTS_DpFileRemoveError_SIZE(0);
    ASSERT_FALSE(Os::FileSystem::exists(dpFile.toChar()));
    ASSERT_EQ(this->component.m_stateFileEntries, 0);

    this->component.shutdown();
}

}  // namespace Svc
