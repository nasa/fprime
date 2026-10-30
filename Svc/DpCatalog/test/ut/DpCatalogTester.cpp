// ======================================================================
// \title  DpCatalogTester.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test harness implementation class
// ======================================================================

#include "DpCatalogTester.hpp"
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
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

namespace {

bool hasDuplicateDpSet(const DpCatalogTester::DpSet* dpSet, FwIndexType entry) {
    for (FwIndexType earlier = 0; earlier < entry; earlier++) {
        if (dpSet[entry].id == dpSet[earlier].id &&
            dpSet[entry].time.getSeconds() == dpSet[earlier].time.getSeconds() &&
            dpSet[entry].time.getUSeconds() == dpSet[earlier].time.getUSeconds() &&
            std::strcmp(dpSet[entry].dir, dpSet[earlier].dir) == 0) {
            return true;
        }
    }
    return false;
}

}  // namespace

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
            do {
                dpSet[entry].id = STest::Pick::startLength(0, NUM_ENTRIES);
                dpSet[entry].time.set(STest::Pick::startLength(0, 10000), STest::Pick::startLength(0, 10000));
                dpSet[entry].dir = dirs[STest::Pick::startLength(0, NUM_DIRS)].toChar();
            } while (hasDuplicateDpSet(dpSet, entry));

            dpSet[entry].prio = STest::Pick::startLength(0, NUM_ENTRIES);
            dpSet[entry].dataSize = STest::Pick::startLength(0, MAX_SIZE);

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

    BYTE buffer[sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE];
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

void DpCatalogTester::configureAndBuild(Fw::FileNameString& dir,
                                        Fw::FileNameString& stateFile,
                                        U32 cmdSeq,
                                        Fw::MemAllocator& alloc) {
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, cmdSeq);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, cmdSeq, Fw::CmdResponse::OK);
}

void DpCatalogTester::test_InsertIntoEmptyBuiltCatalog() {
    // A product written after BUILD_CATALOG of an empty directory is inserted and, since the previous
    // transmission completed with remainActive set, sent without a new START_XMIT_CATALOG
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertEmpty");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());
    const FwDpIdType id = 0x100;
    Fw::Time time(1000, 100);
    this->delDp(id, time, dir.toChar());

    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, true);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::OK);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_from_fileOut_SIZE(0);

    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(dpFile.toChar(), fileSize), Os::FileSystem::Status::OP_OK);

    this->invoke_to_addToCat(0, dpFile, 10, fileSize);
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, dpFile.toChar());
    ASSERT_EQ(this->component.m_pendingFiles, 1);
    ASSERT_EQ(this->component.m_pendingDpBytes, fileSize);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, dpFile, dpFile, 0, 0);

    // the automatic fileDone completes the product and the (resumed) transmission
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EVENTS_ProductComplete(0, dpFile.toChar(), 0, 0);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(2);
    ASSERT_EVENTS_CatalogXmitCompleted(1, fileSize);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    this->delDp(id, time, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_InsertDuringTransmitInPriorityOrder() {
    // Catalog built with priorities 10 and 30. While 10 is in flight, products of priority 20 and 40 are
    // inserted. The file in flight is not disturbed and the send order is 10, 20, 30, 40: the new entries
    // take their place in the existing priority order
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertOrder");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());
    Fw::Time time(1000, 100);
    const FwDpIdType ids[4] = {0x1, 0x2, 0x3, 0x4};
    const FwDpPriorityType prios[4] = {10, 20, 30, 40};
    Fw::String files[4];
    for (FwIndexType i = 0; i < 4; i++) {
        this->delDp(ids[i], time, dir.toChar());
    }
    files[0] = this->genDP(ids[0], prios[0], time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    files[2] = this->genDP(ids[2], prios[2], time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(2);

    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, files[0], files[0], 0, 0);
    const U32 inFlight = this->m_lastContext;

    // insert while priority 10 is in flight
    files[1] = this->genDP(ids[1], prios[1], time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    files[3] = this->genDP(ids[3], prios[3], time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->invoke_to_addToCat(0, files[3], prios[3], 0);
    this->component.doDispatch();
    this->invoke_to_addToCat(0, files[1], prios[1], 0);
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileAdded_SIZE(4);
    ASSERT_EVENTS_DpFileAdded(2, files[3].toChar());
    ASSERT_EVENTS_DpFileAdded(3, files[1].toChar());
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 4);
    ASSERT_EQ(this->component.m_pendingFiles, 4);
    // nothing new is sent while a file is in flight
    ASSERT_from_fileOut_SIZE(1);

    // completing each send yields the next in priority order, new entries included
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    for (FwIndexType i = 1; i < 4; i++) {
        ASSERT_from_fileOut_SIZE(static_cast<U32>(i + 1));
        ASSERT_from_fileOut(static_cast<U32>(i), files[i], files[i], 0, 0);
        this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(4);
    ASSERT_EVENTS_ProductComplete_SIZE(4);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    for (FwIndexType i = 0; i < 4; i++) {
        this->delDp(ids[i], time, dir.toChar());
    }
    this->component.shutdown();
}

void DpCatalogTester::test_InsertWhenFull() {
    // With every slot in use, a runtime insert is refused with DpCatalogFull and leaves the catalog
    // untouched; once a slot is freed by a completed send, the same insert succeeds
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertFull");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());
    const FwDpIdType extraId = DP_MAX_FILES + 1;
    for (FwDpIdType id = 1; id <= extraId; id++) {
        this->delDp(id, Fw::Time(1000 + id, 0), dir.toChar());
    }
    for (FwDpIdType id = 1; id <= DP_MAX_FILES; id++) {
        Fw::String file =
            this->genDP(id, 10, Fw::Time(1000 + id, 0), 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
        ASSERT_STRNE(file.toChar(), "");
    }

    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(DP_MAX_FILES);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), DP_MAX_FILES);
    const U32 pendingFiles = this->component.m_pendingFiles;
    const U64 pendingBytes = this->component.m_pendingDpBytes;

    Fw::String extra =
        this->genDP(extraId, 10, Fw::Time(1000 + extraId, 0), 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(extra.toChar(), "");
    this->invoke_to_addToCat(0, extra, 10, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_DpCatalogFull_SIZE(1);
    ASSERT_EQ(this->eventHistory_DpCatalogFull->at(0).dp.get_id(), extraId);
    ASSERT_EVENTS_DpInsertError_SIZE(0);
    ASSERT_EVENTS_DpFileAdded_SIZE(DP_MAX_FILES);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), DP_MAX_FILES);
    ASSERT_EQ(this->component.m_pendingFiles, pendingFiles);
    ASSERT_EQ(this->component.m_pendingDpBytes, pendingBytes);

    // free one slot by completing one send
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), DP_MAX_FILES - 1);

    this->invoke_to_addToCat(0, extra, 10, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_DpCatalogFull_SIZE(1);
    ASSERT_EVENTS_DpFileAdded_SIZE(DP_MAX_FILES + 1);
    ASSERT_EVENTS_DpFileAdded(DP_MAX_FILES, extra.toChar());
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), DP_MAX_FILES);

    for (FwDpIdType id = 1; id <= extraId; id++) {
        this->delDp(id, Fw::Time(1000 + id, 0), dir.toChar());
    }
    this->component.shutdown();
}

void DpCatalogTester::test_InsertBeforeBuild() {
    // A product reported before BUILD_CATALOG (or after CLEAR_CATALOG) is not recorded and is reported
    // with NotLoaded; the file on disk is then found by the next BUILD_CATALOG
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertBeforeBuild");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());
    const FwDpIdType id = 0x200;
    Fw::Time time(1000, 100);
    this->delDp(id, time, dir.toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    this->invoke_to_addToCat(0, dpFile, 10, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_NotLoaded_SIZE(1);
    ASSERT_EVENTS_NotLoaded(0, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);
    ASSERT_EQ(this->component.m_pendingFiles, 0);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, dpFile.toChar());
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);

    // CLEAR_CATALOG returns to the unbuilt state
    this->sendCmd_CLEAR_CATALOG(0, 11);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_CLEAR_CATALOG, 11, Fw::CmdResponse::OK);
    this->invoke_to_addToCat(0, dpFile, 10, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_NotLoaded_SIZE(2);
    ASSERT_EVENTS_NotLoaded(1, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    this->delDp(id, time, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_InsertDuplicate() {
    // Reporting a product that is already in the catalog is a DpDuplicate; counters are unchanged and
    // the product is sent once
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertDuplicate");
    Fw::FileNameString stateFile("");
    this->makeDpDir(dir.toChar());
    const FwDpIdType id = 0x300;
    Fw::Time time(1000, 100);
    this->delDp(id, time, dir.toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(dpFile.toChar(), fileSize), Os::FileSystem::Status::OP_OK);

    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EQ(this->component.m_pendingFiles, 1);
    ASSERT_EQ(this->component.m_pendingDpBytes, fileSize);

    this->invoke_to_addToCat(0, dpFile, 10, fileSize);
    this->component.doDispatch();
    ASSERT_EVENTS_DpDuplicate_SIZE(1);
    ASSERT_EQ(this->eventHistory_DpDuplicate->at(0).dp.get_id(), id);
    ASSERT_EVENTS_DpFileSkipped_SIZE(0);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);
    ASSERT_EQ(this->component.m_pendingFiles, 1);
    ASSERT_EQ(this->component.m_pendingDpBytes, fileSize);

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, fileSize);

    this->delDp(id, time, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_InsertPreservesTransmittedState() {
    // A runtime insert must not rewrite the state file: the TRANSMITTED records appended since the build
    // are still honored by the next BUILD_CATALOG
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertState");
    Fw::FileNameString stateFile("./DpTest_InsertState/dpState.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const FwDpIdType ids[3] = {0x10, 0x11, 0x12};
    const Fw::Time times[3] = {Fw::Time(2000, 0), Fw::Time(2001, 0), Fw::Time(2002, 0)};
    Fw::String files[3];
    for (FwIndexType i = 0; i < 3; i++) {
        this->delDp(ids[i], times[i], dir.toChar());
    }
    files[0] = this->genDP(ids[0], 10, times[0], 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    files[1] = this->genDP(ids[1], 10, times[1], 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(2);

    // transmit both: their TRANSMITTED records are appended to the state file
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_EVENTS_ProductComplete_SIZE(2);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);

    // insert a new product without transmitting it
    files[2] = this->genDP(ids[2], 10, times[2], 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->invoke_to_addToCat(0, files[2], 10, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileAdded_SIZE(3);
    ASSERT_EVENTS_DpFileAdded(2, files[2].toChar());
    ASSERT_from_fileOut_SIZE(2);
    this->component.shutdown();

    // the rebuilt catalog skips the two transmitted products and adds only the new one
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    // directory listing order is not defined, so only the set of skipped files is checked
    ASSERT_EVENTS_DpFileSkipped_SIZE(2);
    for (FwIndexType i = 0; i < 2; i++) {
        const Fw::String skipped(this->eventHistory_DpFileSkipped->at(static_cast<U32>(i)).file.toChar());
        ASSERT_TRUE(skipped == files[0] || skipped == files[1]) << skipped.toChar();
    }
    ASSERT_STRNE(this->eventHistory_DpFileSkipped->at(0).file.toChar(),
                 this->eventHistory_DpFileSkipped->at(1).file.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, files[2].toChar());
    ASSERT_EQ(this->component.m_pendingFiles, 1);

    for (FwIndexType i = 0; i < 3; i++) {
        this->delDp(ids[i], times[i], dir.toChar());
    }
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_InsertAlreadyTransmitted() {
    // A runtime insert of a product whose TRANSMITTED record was loaded from the state file at build time is
    // skipped with DpFileSkipped, as at build time, and nothing is inserted or sent
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_InsertTransmitted");
    Fw::FileNameString stateFile("./DpTest_InsertTransmitted/dpState.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const FwDpIdType id = 0x400;
    Fw::Time time(3000, 0);
    this->delDp(id, time, dir.toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");

    // build and transmit: the TRANSMITTED record is appended to the state file
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    this->component.shutdown();

    // rebuild so the record is loaded, then report the same file at runtime
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    this->invoke_to_addToCat(0, dpFile, 10, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileSkipped_SIZE(2);
    ASSERT_EVENTS_DpFileSkipped(1, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EVENTS_DpDuplicate_SIZE(0);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);
    ASSERT_EQ(this->component.m_pendingFiles, 0);
    ASSERT_from_fileOut_SIZE(0);

    this->delDp(id, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

// ----------------------------------------------------------------------
// RETRANSMIT_DP
// ----------------------------------------------------------------------

void DpCatalogTester::retransmit(U32 cmdSeq, FwDpIdType id, const Fw::Time& time, Fw::CmdResponse expected) {
    this->clearHistory();
    this->sendCmd_RETRANSMIT_DP(0, cmdSeq, id, time.getSeconds(), time.getUSeconds());
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, cmdSeq, expected);
}

void DpCatalogTester::xmitAll(U32 cmdSeq, bool remainActive) {
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG(0, cmdSeq, Fw::Wait::NO_WAIT, remainActive);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG, cmdSeq, Fw::CmdResponse::OK);
}

Fw::String DpCatalogTester::transmitOneProduct(const char* dirName,
                                               FwDpIdType id,
                                               const Fw::Time& time,
                                               FwDpPriorityType prio,
                                               bool remainActive,
                                               Fw::FileNameString& dir,
                                               Fw::FileNameString& stateFile,
                                               Fw::MemAllocator& alloc) {
    dir = dirName;
    stateFile.format("%s/dpState.dat", dirName);
    this->makeDpDir(dirName);
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->delDp(id, time, dirName);
    Fw::String dpFile = this->genDP(id, prio, time, 16, Fw::DpState::UNTRANSMITTED, false, dirName);
    EXPECT_STRNE(dpFile.toChar(), "");
    this->configureAndBuild(dir, stateFile, 10, alloc);
    this->xmitAll(11, remainActive);
    EXPECT_EQ(this->eventHistory_ProductComplete->size(), 1u);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 0u);
    return dpFile;
}

void DpCatalogTester::test_RetransmitAfterRebuild() {
    // A product transmitted in an earlier session (TRANSMITTED record loaded by the rebuild) is rescheduled at
    // its header priority, sent again by the next transmit command, and skipped again by a later rebuild
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    Fw::FileNameString stateFile;
    const FwDpIdType id = 0x500;
    const FwDpPriorityType prio = 10;
    const Fw::Time time(4000, 40);
    Fw::String dpFile =
        this->transmitOneProduct("./DpTest_RetransmitRebuild", id, time, prio, false, dir, stateFile, alloc);
    ASSERT_FALSE(::testing::Test::HasFailure());
    this->component.shutdown();

    // the rebuild skips the transmitted product
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    // retransmit: cataloged again from the header, nothing sent without a transmit command
    ASSERT_NO_FATAL_FAILURE(this->retransmit(21, id, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitError_SIZE(0);
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, dpFile.toChar(), prio);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, dpFile.toChar());
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);
    ASSERT_EQ(this->component.m_pendingFiles, 1);
    ASSERT_from_fileOut_SIZE(0);

    // the next transmit command sends it
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(22, false));
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, dpFile, dpFile, 0, 0);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EQ(this->component.m_pendingFiles, 0);
    this->component.shutdown();

    // the record was rewritten TRANSMITTED in place, so the rebuild skips it again and the file holds one record
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 30, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    ASSERT_EQ(this->component.m_pendingFiles, 0);
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(fileSize, DpCatalog::STATE_RECORD_SIZE);

    this->delDp(id, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitPersistsAcrossRebuild() {
    // The reset is recorded in the state file: a rebuild before the product is sent again catalogs it, and so
    // does a further rebuild after that one rewrote the file
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    Fw::FileNameString stateFile;
    const FwDpIdType id = 0x501;
    const Fw::Time time(4001, 0);
    Fw::String dpFile =
        this->transmitOneProduct("./DpTest_RetransmitPersist", id, time, 10, false, dir, stateFile, alloc);
    ASSERT_FALSE(::testing::Test::HasFailure());
    this->component.shutdown();

    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_NO_FATAL_FAILURE(this->retransmit(21, id, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, dpFile.toChar(), 10);
    this->component.shutdown();

    // rebuild without sending: the product is pending, not skipped
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 30, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(0);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, dpFile.toChar());
    ASSERT_EQ(this->component.m_pendingFiles, 1);
    this->component.shutdown();

    // that build rewrote the state file: the UNTRANSMITTED record survives the rewrite
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 40, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(0);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EQ(this->component.m_pendingFiles, 1);

    this->delDp(id, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitSameSession() {
    // A product transmitted since the build (record appended and mirrored in memory) is rescheduled and resent
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    Fw::FileNameString stateFile;
    const FwDpIdType id = 0x502;
    const Fw::Time time(4002, 0);
    Fw::String dpFile = this->transmitOneProduct("./DpTest_RetransmitSame", id, time, 20, false, dir, stateFile, alloc);
    ASSERT_FALSE(::testing::Test::HasFailure());

    ASSERT_NO_FATAL_FAILURE(this->retransmit(12, id, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, dpFile.toChar(), 20);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);
    ASSERT_from_fileOut_SIZE(0);

    ASSERT_NO_FATAL_FAILURE(this->xmitAll(13, false));
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, dpFile, dpFile, 0, 0);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    this->component.shutdown();

    // transmitted twice: the single record reads TRANSMITTED, so the rebuild skips it
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(fileSize, DpCatalog::STATE_RECORD_SIZE);

    this->delDp(id, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitDuringTransmit() {
    // Priorities 10, 20, 30 are cataloged. After 10 completes and 20 is in flight, 10 is retransmitted: the
    // file in flight is not disturbed and 10 is sent before 30, in priority order
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_RetransmitDuring");
    Fw::FileNameString stateFile("./DpTest_RetransmitDuring/dpState.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const Fw::Time time(4003, 0);
    const FwDpIdType ids[3] = {0x1, 0x2, 0x3};
    const FwDpPriorityType prios[3] = {10, 20, 30};
    Fw::String files[3];
    for (FwIndexType i = 0; i < 3; i++) {
        this->delDp(ids[i], time, dir.toChar());
        files[i] = this->genDP(ids[i], prios[i], time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
        ASSERT_STRNE(files[i].toChar(), "");
    }
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(3);

    this->m_autoFileDone = false;
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, files[0], files[0], 0, 0);
    // complete 10; 20 goes in flight
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_from_fileOut(1, files[1], files[1], 0, 0);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    const U32 inFlight = this->m_lastContext;

    // retransmit 10 while 20 is in flight: rescheduled, nothing new sent
    ASSERT_NO_FATAL_FAILURE(this->retransmit(12, ids[0], time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, files[0].toChar(), prios[0]);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 3);
    ASSERT_EQ(this->component.m_pendingFiles, 3);
    ASSERT_from_fileOut_SIZE(0);

    // completing 20 sends 10 again before 30
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, files[0], files[0], 0, 0);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_from_fileOut(1, files[2], files[2], 0, 0);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(3);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);
    ASSERT_EQ(this->component.m_pendingFiles, 0);

    for (FwIndexType i = 0; i < 3; i++) {
        this->delDp(ids[i], time, dir.toChar());
    }
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitRemainActiveResumes() {
    // With remainActive set, a retransmit after the transmission finished resumes it immediately, as a runtime
    // insert does, continuing the session's byte tally; without remainActive it waits for the next transmit
    // command (test_RetransmitSameSession)
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    Fw::FileNameString stateFile;
    const FwDpIdType id = 0x503;
    const Fw::Time time(4004, 0);
    Fw::String dpFile =
        this->transmitOneProduct("./DpTest_RetransmitResume", id, time, 10, true, dir, stateFile, alloc);
    ASSERT_FALSE(::testing::Test::HasFailure());
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_FALSE(this->component.m_xmitInProgress);

    ASSERT_NO_FATAL_FAILURE(this->retransmit(12, id, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, dpFile.toChar(), 10);
    // the send is started by the command itself; its (auto) fileDone is queued, so drain the queue
    ASSERT_from_fileOut_SIZE(1);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_from_fileOut(0, dpFile, dpFile, 0, 0);
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    // the resumed session reports the bytes of the first transmission plus the resend
    ASSERT_EVENTS_CatalogXmitCompleted(0, 2 * Fw::DpContainer::getPacketSizeForDataSize(16));
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);
    ASSERT_FALSE(this->component.m_xmitInProgress);

    this->delDp(id, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitRejected() {
    // Every rejection answers EXECUTION_ERROR with DpRetransmitError naming the reason and changes nothing
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_RetransmitReject");
    Fw::FileNameString stateFile("./DpTest_RetransmitReject/dpState.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const FwDpIdType id = 0x504;
    const Fw::Time time(4005, 0);
    this->delDp(id, time, dir.toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");

    // not configured
    ASSERT_NO_FATAL_FAILURE(this->retransmit(1, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_ComponentNotInitialized_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError_SIZE(0);

    // configured, catalog not built
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dir, 1), stateFile, 100, alloc);
    ASSERT_NO_FATAL_FAILURE(this->retransmit(2, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, id, time.getSeconds(), time.getUSeconds(), DpRetransmitReason::NOT_BUILT);

    // built: the product is pending
    this->clearHistory();
    this->sendCmd_BUILD_CATALOG(0, 3);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 3, Fw::CmdResponse::OK);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);
    ASSERT_NO_FATAL_FAILURE(this->retransmit(4, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, id, time.getSeconds(), time.getUSeconds(), DpRetransmitReason::PENDING);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);
    ASSERT_EQ(this->component.m_pendingFiles, 1);

    // no such product
    ASSERT_NO_FATAL_FAILURE(this->retransmit(5, id + 1, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, id + 1, time.getSeconds(), time.getUSeconds(), DpRetransmitReason::NOT_FOUND);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);

    // in flight
    this->m_autoFileDone = false;
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG(0, 6, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    const U32 inFlight = this->m_lastContext;
    ASSERT_NO_FATAL_FAILURE(this->retransmit(7, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, id, time.getSeconds(), time.getUSeconds(), DpRetransmitReason::IN_FLIGHT);
    ASSERT_from_fileOut_SIZE(0);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    // transmitted, then deleted from disk
    this->delDp(id, time, dir.toChar());
    ASSERT_NO_FATAL_FAILURE(this->retransmit(8, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, id, time.getSeconds(), time.getUSeconds(), DpRetransmitReason::NOT_FOUND);
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(0);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitFileError() {
    // A transmitted product whose file has since been truncated is reported by the file event and FILE_ERROR,
    // the state record is left alone, and a rebuild still skips the product
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    Fw::FileNameString stateFile;
    const FwDpIdType id = 0x505;
    const Fw::Time time(4006, 0);
    Fw::String dpFile =
        this->transmitOneProduct("./DpTest_RetransmitFileErr", id, time, 10, false, dir, stateFile, alloc);
    ASSERT_FALSE(::testing::Test::HasFailure());

    // truncate the file below the header size
    Os::File file;
    ASSERT_EQ(file.open(dpFile.toChar(), Os::File::Mode::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE),
              Os::File::Status::OP_OK);
    const U8 junk[4] = {0, 1, 2, 3};
    FwSizeType size = sizeof(junk);
    ASSERT_EQ(file.write(junk, size), Os::File::Status::OP_OK);
    file.close();

    ASSERT_NO_FATAL_FAILURE(this->retransmit(12, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_FileReadError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, id, time.getSeconds(), time.getUSeconds(), DpRetransmitReason::FILE_ERROR);
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(0);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);
    ASSERT_EQ(this->component.m_pendingFiles, 0);
    // the in-memory record was restored
    ASSERT_EQ(this->component.m_stateFileEntries, 1);
    ASSERT_EQ(this->component.m_stateFileData[0].entry.record.get_state(), Fw::DpState::TRANSMITTED);

    // restore the file: the state file was not touched, so the rebuild skips the transmitted product
    this->component.shutdown();
    this->delDp(id, time, dir.toChar());
    dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileAdded_SIZE(0);

    this->delDp(id, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitNameError() {
    // A managed directory long enough that the canonical name overflows Fw::String is answered NAME_ERROR
    Fw::MallocAllocator alloc;
    char longName[232];
    memset(longName, 'd', sizeof(longName) - 1);
    longName[0] = '.';
    longName[1] = '/';
    longName[sizeof(longName) - 1] = 0;
    Fw::FileNameString dir(longName);
    Fw::FileNameString stateFile("./DpTest_RetransmitNameErr.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    const Fw::Time time(4007, 0);
    ASSERT_NO_FATAL_FAILURE(this->retransmit(11, 0x506, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError_SIZE(1);
    ASSERT_EVENTS_DpRetransmitError(0, 0x506, 4007, 0, DpRetransmitReason::NAME_ERROR);
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    (void)Os::FileSystem::removeDirectory(dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitNoStateFileDir() {
    // With a state file that cannot be written (missing directory) nothing is persisted, but the product is
    // still rescheduled in memory and the command succeeds
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_RetransmitNoState");
    Fw::FileNameString stateFile("./DpTest_NoSuchDir/dpState.dat");
    this->makeDpDir(dir.toChar());
    const FwDpIdType id = 0x507;
    const Fw::Time time(4008, 0);
    this->delDp(id, time, dir.toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(dpFile.toChar(), "");
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_NoStateFile_SIZE(1);
    ASSERT_EVENTS_StateFileOpenError_SIZE(1);
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(11, false));
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_EVENTS_StateFileOpenError_SIZE(1);
    ASSERT_EQ(this->component.m_stateFileEntries, 0);

    ASSERT_NO_FATAL_FAILURE(this->retransmit(12, id, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_StateFileOpenError_SIZE(0);
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, dpFile.toChar(), 10);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 1);

    this->delDp(id, time, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitAfterStop() {
    // Retransmit after STOP_XMIT_CATALOG with a file still in flight (remainActive): the stopped transmission
    // resumes without re-sending the file in flight, and the retransmitted product is sent exactly once after it
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_RetransmitStop");
    Fw::FileNameString stateFile("./DpTest_RetransmitStop/dpState.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const Fw::Time time(4009, 0);
    const FwDpIdType ids[2] = {0x1, 0x2};
    Fw::String files[2];
    for (FwIndexType i = 0; i < 2; i++) {
        this->delDp(ids[i], time, dir.toChar());
        files[i] = this->genDP(ids[i], static_cast<FwDpPriorityType>(10 * (i + 1)), time, 16,
                               Fw::DpState::UNTRANSMITTED, false, dir.toChar());
        ASSERT_STRNE(files[i].toChar(), "");
    }
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));

    this->m_autoFileDone = false;
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, true);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, files[0], files[0], 0, 0);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_from_fileOut(1, files[1], files[1], 0, 0);
    const U32 inFlight = this->m_lastContext;

    // stop with the second file in flight, then retransmit the first: nothing new is sent yet
    this->clearHistory();
    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_EVENTS_CatalogXmitStopped_SIZE(1);
    ASSERT_NO_FATAL_FAILURE(this->retransmit(13, ids[0], time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_from_fileOut_SIZE(0);

    // the completion of the file in flight continues the walk: the first file is sent once more, then done
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, files[0], files[0], 0, 0);
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, this->m_lastContext));
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_ProductComplete_SIZE(2);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    for (FwIndexType i = 0; i < 2; i++) {
        this->delDp(ids[i], time, dir.toChar());
    }
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitSecondDirectory() {
    // The product file is searched in every managed directory: a product in the second one is found, its
    // record (directory index 1) is rewritten, and the rebuild after the resend skips it
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[2] = {Fw::FileNameString("./DpTest_RetransmitDir0"),
                                  Fw::FileNameString("./DpTest_RetransmitDir1")};
    Fw::FileNameString stateFile("./DpTest_RetransmitDir0/dpState.dat");
    this->makeDpDir(dirs[0].toChar());
    this->makeDpDir(dirs[1].toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const FwDpIdType id = 0x508;
    const Fw::Time time(4010, 0);
    this->delDp(id, time, dirs[1].toChar());
    Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirs[1].toChar());
    ASSERT_STRNE(dpFile.toChar(), "");

    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 2), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(11, false));
    ASSERT_EVENTS_ProductComplete_SIZE(1);

    ASSERT_NO_FATAL_FAILURE(this->retransmit(12, id, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitError_SIZE(0);
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, dpFile.toChar(), 10);
    ASSERT_EQ(this->component.m_stateFileData[0].entry.dir, 1);
    ASSERT_EQ(this->component.m_stateFileData[0].entry.record.get_state(), Fw::DpState::UNTRANSMITTED);
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(13, false));
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_from_fileOut(0, dpFile, dpFile, 0, 0);
    this->component.shutdown();

    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(dirs, 2), stateFile, 100, alloc);
    this->sendCmd_BUILD_CATALOG(0, 20);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(0);

    this->delDp(id, time, dirs[1].toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitFullStateFile() {
    // With every slot holding a TRANSMITTED record (a full, fully downlinked catalog) a retransmit still
    // survives a rebuild: the record is rewritten in place, so the file never exceeds one record per product
    Fw::MallocAllocator alloc;
    const char* dirName = "./DpTest_RetransmitFull";
    Fw::FileNameString dir(dirName);
    Fw::FileNameString stateFile("./DpTest_RetransmitFull/dpState.dat");
    this->makeDpDir(dirName);
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const Fw::Time time(4011, 0);
    Fw::String firstFile;
    for (FwDpIdType id = 1; id <= DP_MAX_FILES; id++) {
        this->delDp(id, time, dirName);
        Fw::String dpFile = this->genDP(id, 10, time, 16, Fw::DpState::UNTRANSMITTED, false, dirName);
        ASSERT_STRNE(dpFile.toChar(), "");
        if (id == 1) {
            firstFile = dpFile;
        }
    }
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_EVENTS_DpFileAdded_SIZE(DP_MAX_FILES);
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(11, false));
    ASSERT_EVENTS_ProductComplete_SIZE(DP_MAX_FILES);
    this->component.shutdown();

    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(DP_MAX_FILES);
    ASSERT_EQ(this->component.m_stateFileEntries, static_cast<FwSizeType>(DP_MAX_FILES));
    ASSERT_NO_FATAL_FAILURE(this->retransmit(21, 1, time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_DpRetransmitScheduled_SIZE(1);
    ASSERT_EVENTS_DpRetransmitScheduled(0, firstFile.toChar(), 10);
    this->component.shutdown();

    // the reset survives the reboot and the file still holds exactly one record per product
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 30, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(DP_MAX_FILES - 1);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, firstFile.toChar());
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(31, false));
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(fileSize, static_cast<FwSizeType>(DP_MAX_FILES) * DpCatalog::STATE_RECORD_SIZE);

    for (FwDpIdType id = 1; id <= DP_MAX_FILES; id++) {
        this->delDp(id, time, dirName);
    }
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitAfterPrune() {
    // A record pruned at build shifts the following records down in the file; the in-memory copy must shift too,
    // or the retransmit's in-place rewrite misses (or hits another product's record)
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir("./DpTest_RetransmitPrune");
    Fw::FileNameString stateFile("./DpTest_RetransmitPrune/dpState.dat");
    this->makeDpDir(dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    const Fw::Time time(4013, 0);
    const FwDpIdType ids[2] = {0x1, 0x2};
    Fw::String files[2];
    for (FwIndexType i = 0; i < 2; i++) {
        this->delDp(ids[i], time, dir.toChar());
        files[i] = this->genDP(ids[i], static_cast<FwDpPriorityType>(10 * (i + 1)), time, 16,
                               Fw::DpState::UNTRANSMITTED, false, dir.toChar());
        ASSERT_STRNE(files[i].toChar(), "");
    }
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 10, alloc));
    ASSERT_NO_FATAL_FAILURE(this->xmitAll(11, false));
    ASSERT_EVENTS_ProductComplete_SIZE(2);
    ASSERT_EQ(this->component.m_stateFileEntries, 2);
    this->component.shutdown();

    // the first product is gone: its record is pruned by the rebuild and the second record moves to position 0
    this->delDp(ids[0], time, dir.toChar());
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 20, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, files[1].toChar());
    ASSERT_EQ(this->component.m_stateFileEntries, 1);
    ASSERT_EQ(this->component.m_stateFileData[0].entry.record.get_id(), ids[1]);
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(fileSize, DpCatalog::STATE_RECORD_SIZE);

    // the retransmit rewrites record 0 in place and the reset survives a rebuild
    ASSERT_NO_FATAL_FAILURE(this->retransmit(21, ids[1], time, Fw::CmdResponse::OK));
    ASSERT_EVENTS_StateFileOpenError_SIZE(0);
    ASSERT_EVENTS_StateFileWriteError_SIZE(0);
    ASSERT_EVENTS_DpRetransmitScheduled(0, files[1].toChar(), 20);
    this->component.shutdown();
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 30, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(0);
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, files[1].toChar());
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(fileSize, DpCatalog::STATE_RECORD_SIZE);

    this->delDp(ids[1], time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_BuildFailsOnUnreadableStateFile() {
    // A build whose state-file load fails leaves no catalog behind: the in-memory copy is empty and no longer
    // mirrors the file, so nothing may append to or rewrite the file until a build succeeds
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    Fw::FileNameString stateFile;
    const FwDpIdType id = 0x50A;
    const Fw::Time time(4014, 0);
    Fw::String dpFile =
        this->transmitOneProduct("./DpTest_RetransmitLoadFail", id, time, 10, false, dir, stateFile, alloc);
    ASSERT_FALSE(::testing::Test::HasFailure());
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    // the state file cannot be opened: the rebuild fails and the catalog built above is cleared
    ASSERT_EQ(::chmod(stateFile.toChar(), 0), 0);
    this->clearHistory();
    this->sendCmd_BUILD_CATALOG(0, 20);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 20, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_StateFileOpenError_SIZE(1);
    ASSERT_FALSE(this->component.m_catalogBuilt);
    ASSERT_EQ(this->component.m_stateFileEntries, 0);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), 0);

    // neither a runtime insert nor a retransmit touches the state file while the catalog is not built
    const FwDpIdType otherId = 0x50B;
    this->delDp(otherId, time, dir.toChar());
    Fw::String otherFile = this->genDP(otherId, 20, time, 16, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    ASSERT_STRNE(otherFile.toChar(), "");
    this->invoke_to_addToCat(0, otherFile, 20, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_NotLoaded_SIZE(1);
    ASSERT_NO_FATAL_FAILURE(this->retransmit(21, id, time, Fw::CmdResponse::EXECUTION_ERROR));
    ASSERT_EVENTS_DpRetransmitError(0, id, 4014, 0, DpRetransmitReason::NOT_BUILT);
    ASSERT_EQ(::chmod(stateFile.toChar(), S_IRUSR | S_IWUSR), 0);
    FwSizeType fileSize = 0;
    ASSERT_EQ(Os::FileSystem::getFileSize(stateFile.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(fileSize, DpCatalog::STATE_RECORD_SIZE);
    this->component.shutdown();

    // the next successful build reads the intact record: the transmitted product is skipped, the other is added
    this->clearHistory();
    ASSERT_NO_FATAL_FAILURE(this->configureAndBuild(dir, stateFile, 30, alloc));
    ASSERT_EVENTS_DpFileSkipped_SIZE(1);
    ASSERT_EVENTS_DpFileSkipped(0, dpFile.toChar());
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_EVENTS_DpFileAdded(0, otherFile.toChar());
    ASSERT_EQ(this->component.m_stateFileEntries, 1);

    this->delDp(id, time, dir.toChar());
    this->delDp(otherId, time, dir.toChar());
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    this->component.shutdown();
}

// ----------------------------------------------------------------------
// START_XMIT_CATALOG_WITH_PRIORITY priority range
// ----------------------------------------------------------------------

const Fw::Time DpCatalogTester::PRIORITY_RANGE_TIME(3000, 300);
const FwDpPriorityType DpCatalogTester::PRIORITY_MAX;
constexpr FwSizeType DpCatalogTester::PRIORITY_RANGE_DATA_SIZE;

void DpCatalogTester::buildPriorityCatalog(const char* dir, const FwDpPriorityType* prios, FwSizeType numPrios) {
    Fw::FileNameString dirName(dir);
    Fw::FileNameString stateFile;
    stateFile.format("%s/dpState.dat", dir);
    this->makeDpDir(dir);
    (void)Os::FileSystem::removeFile(stateFile.toChar());
    for (FwSizeType index = 0; index < numPrios; index++) {
        const FwDpIdType id = static_cast<FwDpIdType>(index + 1);
        this->delDp(id, PRIORITY_RANGE_TIME, dir);
        Fw::String dpFile = this->genDP(id, prios[index], PRIORITY_RANGE_TIME, PRIORITY_RANGE_DATA_SIZE,
                                        Fw::DpState::UNTRANSMITTED, false, dir);
        ASSERT_STRNE(dpFile.toChar(), "");
    }

    this->clearHistory();
    this->component.configure(Fw::ExternalArray<Fw::FileNameString>(&dirName, 1), stateFile, 100,
                              this->m_priorityRangeAlloc);
    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);
    ASSERT_EVENTS_DpFileAdded_SIZE(numPrios);
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), numPrios);
}

void DpCatalogTester::xmitPriorityRange(U32 cmdSeq,
                                        FwDpPriorityType startPriority,
                                        FwDpPriorityType endPriority,
                                        const FwDpPriorityType* expectedPrios,
                                        FwSizeType numExpected) {
    const FwSizeType sizeBefore = this->component.m_dpCatalog.getSize();
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG_WITH_PRIORITY(0, cmdSeq, Fw::Wait::WAIT, false, startPriority, endPriority);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG_WITH_PRIORITY, cmdSeq, Fw::CmdResponse::OK);
    ASSERT_EVENTS_XmitPriorityRangeInvalid_SIZE(0);
    ASSERT_EVENTS_CatalogXmitRangeStarted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitRangeStarted(0, startPriority, endPriority);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(
        0, numExpected * Fw::DpContainer::getPacketSizeForDataSize(PRIORITY_RANGE_DATA_SIZE));
    ASSERT_from_fileOut_SIZE(numExpected);
    ASSERT_EVENTS_SendingProduct_SIZE(numExpected);
    for (FwSizeType index = 0; index < numExpected; index++) {
        EXPECT_EQ(this->eventHistory_SendingProduct->at(index).prio, expectedPrios[index]) << "index " << index;
    }
    // Products outside the range stay in the catalog for a later transmit command
    ASSERT_EQ(this->component.m_dpCatalog.getSize(), sizeBefore - numExpected);
    EXPECT_FALSE(this->component.m_xmitInProgress);
}

void DpCatalogTester::cleanPriorityCatalog(const char* dir, FwSizeType numPrios) {
    this->component.shutdown();
    for (FwSizeType index = 0; index < numPrios; index++) {
        this->delDp(static_cast<FwDpIdType>(index + 1), PRIORITY_RANGE_TIME, dir);
    }
    Fw::FileNameString stateFile;
    stateFile.format("%s/dpState.dat", dir);
    (void)Os::FileSystem::removeFile(stateFile.toChar());
}

void DpCatalogTester::test_XmitPriorityRangeAll() {
    const char* dir = "./DpTest_RangeAll";
    const FwDpPriorityType prios[] = {30, 10, 20, PRIORITY_MAX, 0};
    const FwDpPriorityType expected[] = {0, 10, 20, 30, PRIORITY_MAX};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // The full range sends every product in priority order
    this->xmitPriorityRange(11, 0, PRIORITY_MAX, expected, FW_NUM_ARRAY_ELEMENTS(expected));
    EXPECT_EQ(this->component.m_pendingFiles, 0);

    this->cleanPriorityCatalog(dir, FW_NUM_ARRAY_ELEMENTS(prios));
}

void DpCatalogTester::test_XmitPriorityRangeSubset() {
    const char* dir = "./DpTest_RangeSubset";
    const FwDpPriorityType prios[] = {50, 1, 20, 10, 5};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // Only the products inside [5, 20] are sent; 1 and 50 remain pending
    const FwDpPriorityType expectedInRange[] = {5, 10, 20};
    this->xmitPriorityRange(11, 5, 20, expectedInRange, FW_NUM_ARRAY_ELEMENTS(expectedInRange));
    EXPECT_EQ(this->component.m_pendingFiles, 2);

    // A range that covers nothing pending completes without sending
    this->xmitPriorityRange(12, 2, 49, nullptr, 0);
    EXPECT_EQ(this->component.m_pendingFiles, 2);

    // The remaining products are still available to a later full-range command
    const FwDpPriorityType expectedRest[] = {1, 50};
    this->xmitPriorityRange(13, 0, PRIORITY_MAX, expectedRest, FW_NUM_ARRAY_ELEMENTS(expectedRest));
    EXPECT_EQ(this->component.m_pendingFiles, 0);

    this->cleanPriorityCatalog(dir, FW_NUM_ARRAY_ELEMENTS(prios));
}

void DpCatalogTester::test_XmitPriorityRangeBoundary() {
    const char* dir = "./DpTest_RangeBoundary";
    const FwDpPriorityType prios[] = {4, 5, 6, PRIORITY_MAX};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // Both bounds are inclusive: start == end selects exactly that priority
    const FwDpPriorityType expectedFive[] = {5};
    this->xmitPriorityRange(11, 5, 5, expectedFive, FW_NUM_ARRAY_ELEMENTS(expectedFive));

    // Products exactly at each bound are included
    const FwDpPriorityType expectedEdges[] = {4, 6};
    this->xmitPriorityRange(12, 4, 6, expectedEdges, FW_NUM_ARRAY_ELEMENTS(expectedEdges));

    // The maximum priority value is reachable as an inclusive end bound
    const FwDpPriorityType expectedMax[] = {PRIORITY_MAX};
    this->xmitPriorityRange(13, PRIORITY_MAX, PRIORITY_MAX, expectedMax, FW_NUM_ARRAY_ELEMENTS(expectedMax));
    EXPECT_EQ(this->component.m_pendingFiles, 0);

    this->cleanPriorityCatalog(dir, FW_NUM_ARRAY_ELEMENTS(prios));
}

void DpCatalogTester::test_XmitPriorityRangeInverted() {
    const char* dir = "./DpTest_RangeInverted";
    const FwDpPriorityType prios[] = {5};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // start > end is rejected before any state changes, even for a waited command
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG_WITH_PRIORITY(0, 11, Fw::Wait::WAIT, true, 10, 5);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG_WITH_PRIORITY, 11, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_XmitPriorityRangeInvalid_SIZE(1);
    ASSERT_EVENTS_XmitPriorityRangeInvalid(0, 10, 5);
    ASSERT_EVENTS_CatalogXmitRangeStarted_SIZE(0);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(0);
    ASSERT_from_fileOut_SIZE(0);
    EXPECT_FALSE(this->component.m_xmitInProgress);
    EXPECT_FALSE(this->component.m_xmitCmdWait);
    EXPECT_FALSE(this->component.m_remainActive);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);

    // The catalog is intact and a valid command still transmits it
    const FwDpPriorityType expected[] = {5};
    this->xmitPriorityRange(12, 5, 10, expected, FW_NUM_ARRAY_ELEMENTS(expected));

    this->cleanPriorityCatalog(dir, FW_NUM_ARRAY_ELEMENTS(prios));
}

void DpCatalogTester::test_XmitPriorityRangeRuntimeAdd() {
    const char* dir = "./DpTest_RangeRuntime";
    const FwDpPriorityType prios[] = {10};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // Transmit [10, 20] and remain active for runtime additions
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG_WITH_PRIORITY(0, 11, Fw::Wait::NO_WAIT, true, 10, 20);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG_WITH_PRIORITY, 11, Fw::CmdResponse::OK);
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);

    // A runtime DP outside the range is cataloged but not sent
    Fw::String outside =
        this->genDP(2, 30, PRIORITY_RANGE_TIME, PRIORITY_RANGE_DATA_SIZE, Fw::DpState::UNTRANSMITTED, false, dir);
    ASSERT_STRNE(outside.toChar(), "");
    this->invoke_to_addToCat(0, outside, 30, 0);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_from_fileOut_SIZE(1);
    // No new transmission was started, so no spurious completion or range-started event is reported
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitRangeStarted_SIZE(1);
    EXPECT_FALSE(this->component.m_xmitInProgress);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);

    // A runtime DP inside the range is sent
    Fw::String inside =
        this->genDP(3, 15, PRIORITY_RANGE_TIME, PRIORITY_RANGE_DATA_SIZE, Fw::DpState::UNTRANSMITTED, false, dir);
    ASSERT_STRNE(inside.toChar(), "");
    this->invoke_to_addToCat(0, inside, 15, 0);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_EVENTS_DpFileAdded_SIZE(2);
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_from_fileOut(1, inside, inside, 0, 0);
    ASSERT_EVENTS_SendingProduct_SIZE(2);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(0).prio, 10);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(1).prio, 15);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);
    // The resume reuses the stored range: no new range-started event, but the resume completes
    ASSERT_EVENTS_CatalogXmitRangeStarted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(2);

    this->cleanPriorityCatalog(dir, 3);  // ids 1..3: the built products plus the runtime additions
}

void DpCatalogTester::test_XmitPriorityRangeStopThenRuntimeAdd() {
    const char* dir = "./DpTest_RangeStopRuntime";
    const FwDpPriorityType prios[] = {10, 15};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // Start [10, 20] with remainActive, holding the first file in flight, then stop
    this->clearHistory();
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG_WITH_PRIORITY(0, 11, Fw::Wait::NO_WAIT, true, 10, 20);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG_WITH_PRIORITY, 11, Fw::CmdResponse::OK);
    ASSERT_from_fileOut_SIZE(1);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(0).prio, 10);
    const U32 inFlight = this->m_lastContext;

    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_STOP_XMIT_CATALOG, 12, Fw::CmdResponse::OK);
    ASSERT_EVENTS_CatalogXmitStopped_SIZE(1);

    // The in-flight file completes; priority 15 stays pending because transmission was stopped
    this->m_autoFileDone = true;
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    this->component.doDispatch();
    ASSERT_EVENTS_ProductComplete_SIZE(1);
    ASSERT_from_fileOut_SIZE(1);
    EXPECT_FALSE(this->component.m_xmitInProgress);
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);

    // A runtime DP outside the range arrives: remainActive resumes the stopped walk because the pending
    // priority-15 product is inside the stored range, with no new range-started event
    Fw::String outside =
        this->genDP(3, 30, PRIORITY_RANGE_TIME, PRIORITY_RANGE_DATA_SIZE, Fw::DpState::UNTRANSMITTED, false, dir);
    ASSERT_STRNE(outside.toChar(), "");
    this->invoke_to_addToCat(0, outside, 30, 0);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_EVENTS_DpFileAdded_SIZE(1);
    ASSERT_from_fileOut_SIZE(2);
    ASSERT_EVENTS_SendingProduct_SIZE(2);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(1).prio, 15);
    ASSERT_EVENTS_CatalogXmitRangeStarted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    EXPECT_FALSE(this->component.m_xmitInProgress);
    // Only the out-of-range product remains pending
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);

    this->cleanPriorityCatalog(dir, 3);  // ids 1..3: the built products plus the runtime additions
}

void DpCatalogTester::test_XmitLegacyCommandSendsFullRange() {
    const char* dir = "./DpTest_RangeLegacy";
    const FwDpPriorityType prios[] = {30, 10, PRIORITY_MAX, 0};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // Narrow the active range first, so the legacy command must reset it
    const FwDpPriorityType first[] = {10};
    this->xmitPriorityRange(11, 10, 10, first, FW_NUM_ARRAY_ELEMENTS(first));

    // START_XMIT_CATALOG keeps its original arguments and transmits everything else in priority order
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG(0, 12, Fw::Wait::WAIT, false);
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_START_XMIT_CATALOG, 12, Fw::CmdResponse::OK);
    ASSERT_EVENTS_CatalogXmitRangeStarted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitRangeStarted(0, 0, PRIORITY_MAX);
    ASSERT_EVENTS_SendingProduct_SIZE(3);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(0).prio, 0);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(1).prio, 30);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(2).prio, PRIORITY_MAX);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, 3 * Fw::DpContainer::getPacketSizeForDataSize(PRIORITY_RANGE_DATA_SIZE));
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 0);

    this->cleanPriorityCatalog(dir, FW_NUM_ARRAY_ELEMENTS(prios));
}

void DpCatalogTester::test_XmitPriorityRangeStartAfterStopAppliesRangeToNextFile() {
    const char* dir = "./DpTest_RangeStartAfterStop";
    const FwDpPriorityType prios[] = {10, 20, 30};
    this->buildPriorityCatalog(dir, prios, FW_NUM_ARRAY_ELEMENTS(prios));

    // Hold priority 10 in flight, then STOP
    this->clearHistory();
    this->m_autoFileDone = false;
    this->sendCmd_START_XMIT_CATALOG_WITH_PRIORITY(0, 11, Fw::Wait::NO_WAIT, false, 0, PRIORITY_MAX);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    const U32 inFlight = this->m_lastContext;
    this->sendCmd_STOP_XMIT_CATALOG(0, 12);
    this->component.doDispatch();

    // Re-start with a range that excludes both the in-flight file (10) and priority 20: nothing new
    // is sent until the in-flight completion arrives
    this->m_autoFileDone = true;
    this->sendCmd_START_XMIT_CATALOG_WITH_PRIORITY(0, 13, Fw::Wait::WAIT, false, 25, 35);
    this->component.doDispatch();
    ASSERT_from_fileOut_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(2);

    // The in-flight file completes whatever its priority and is counted; only priority 30 follows
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_OK, inFlight));
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }
    ASSERT_EVENTS_ProductComplete_SIZE(2);
    ASSERT_EVENTS_SendingProduct_SIZE(2);
    EXPECT_EQ(this->eventHistory_SendingProduct->at(1).prio, 30);
    ASSERT_EVENTS_CatalogXmitCompleted_SIZE(1);
    ASSERT_EVENTS_CatalogXmitCompleted(0, 2 * Fw::DpContainer::getPacketSizeForDataSize(PRIORITY_RANGE_DATA_SIZE));
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_START_XMIT_CATALOG_WITH_PRIORITY, 13, Fw::CmdResponse::OK);
    // Priority 20 stays pending: it is outside the new range
    EXPECT_EQ(this->component.m_dpCatalog.getSize(), 1);

    this->cleanPriorityCatalog(dir, FW_NUM_ARRAY_ELEMENTS(prios));
}

}  // namespace Svc
