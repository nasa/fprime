// ======================================================================
// \title  DpCatalogTester.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test harness implementation class
// ======================================================================

#include "DpCatalogTester.hpp"
#include <algorithm>
#include <cstdlib>
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
    this->component.configure(dirs, FW_NUM_ARRAY_ELEMENTS(dirs), stateFile, 100, alloc);
    this->component.shutdown();
}

void DpCatalogTester::testTree(DpCatalog::DpStateEntry* input,
                               DpCatalog::DpStateEntry* output,
                               FwIndexType numEntries) {
    ASSERT_TRUE(input != nullptr);
    ASSERT_TRUE(output != nullptr);
    ASSERT_TRUE(numEntries > 0);

    Fw::MallocAllocator alloc;

    Fw::FileNameString dirs[1];
    dirs[0] = "dir0";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->component.configure(dirs, FW_NUM_ARRAY_ELEMENTS(dirs), stateFile, 100, alloc);

    // reset tree
    this->component.resetBinaryTree();

    // add entries
    for (FwIndexType entry = 0; entry < numEntries; entry++) {
        ASSERT_TRUE(this->component.insertEntry(input[entry]));
    }

    // hot wire in progress
    this->component.m_xmitInProgress = true;

    // retrieve entries - they should match expected output
    for (FwIndexType entry = 0; entry < numEntries + 1; entry++) {
        if (entry == numEntries) {
            // final request should indicate empty
            ASSERT_TRUE(this->component.findNextTreeNode() == nullptr);
        } else if (output[entry].record.get_state() != Fw::DpState::TRANSMITTED) {
            // Outputs is only composed of the UNTRANSMITTED data products
            DpCatalog::DpBtreeNode* res = this->component.findNextTreeNode();
            ASSERT_TRUE(res != nullptr) << "nullptr findNextTreeNode() at " << entry << " out of " << numEntries;

            //  should match expected entry
            if (res != nullptr) {
                ASSERT_EQ(res->entry.record, output[entry].record) << "entry mismatch at " << entry;
            }
            // Deallocate the "sent" node
            this->component.deallocateNode(res);
        }
    }

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

    this->component.configure(dpDirs, numDirs, stateFile, 100, alloc);

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

Fw::String DpCatalogTester::genDP(FwDpIdType id,
                                  FwDpPriorityType prio,
                                  const Fw::Time& time,
                                  FwSizeType dataSize,
                                  Fw::DpState dpState,
                                  bool hdrHashError,
                                  const char* dir) {
    // Fill DP container
    U8 hdrData[Fw::DpContainer::MIN_PACKET_SIZE];
    Fw::Buffer hdrBuffer(hdrData, Fw::DpContainer::MIN_PACKET_SIZE);
    Fw::DpContainer cont(id, hdrBuffer);
    cont.setPriority(prio);
    cont.setTimeTag(time);
    cont.setDpState(dpState);
    cont.setDataSize(dataSize);
    // serialize file data
    cont.serializeHeader();
    // fill data with ramp
    U8 dpData[dataSize];
    for (FwIndexType byte = 0; byte < static_cast<FwIndexType>(dataSize); byte++) {
        dpData[byte] = byte;
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
    FwSizeType size = Fw::DpContainer::Header::SIZE;
    stat = dpFile.write(hdrData, size);
    if (stat != Os::File::Status::OP_OK) {
        printf("Error writing DP file header %s: status: %d\n", fileName.toChar(), stat);
        return "";
    }
    if (static_cast<FwSizeType>(size) != Fw::DpContainer::Header::SIZE) {
        printf("Dp file header %s write size didn't match. Req: %" PRI_FwSizeType "Act: %" PRI_FwSizeType "\n",
               fileName.toChar(), Fw::DpContainer::Header::SIZE, size);
        return "";
    }
    size = dataSize;
    stat = dpFile.write(dpData, size);
    if (stat != Os::File::Status::OP_OK) {
        printf("Error writing DP file data %s: status: %" PRI_FwEnumStoreType "\n", fileName.toChar(),
               static_cast<FwEnumStoreType>(stat));
        return "";
    }
    if (static_cast<FwSizeType>(size) != dataSize) {
        printf("Dp file header %s write size didn't match. Req: %" PRI_FwSizeType " Act: %" PRI_FwSizeType "\n",
               fileName.toChar(), dataSize, size);
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
    // Tell the DpCatalog that the xmit succeeded
    this->pushFromPortEntry_fileOut(sourceFileName, destFileName, offset, length);
    this->invoke_to_fileDone(0, Svc::SendFileResponse());

    return Svc::SendFileResponse();
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
        Svc::DpCatalog::DpStateEntry outputs[NUM_ENTRIES];

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

        std::partial_sort_copy(std::begin(inputs), std::end(inputs), std::begin(outputs), std::end(outputs));

        this->testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestManual1() {
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[1];
    Svc::DpCatalog::DpStateEntry outputs[1];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::UNTRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    outputs[0].record.set_id(1);
    outputs[0].record.set_priority(2);
    outputs[0].record.set_state(Fw::DpState::UNTRANSMITTED);
    outputs[0].record.set_tSec(1000);
    outputs[0].record.set_tSub(1500);
    outputs[0].record.set_size(100);

    testTree(inputs, outputs, 1);
}

void DpCatalogTester ::test_TreeTestManual2() {
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[2];
    Svc::DpCatalog::DpStateEntry outputs[2];

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

    outputs[0].record = inputs[1].record;
    outputs[1].record = inputs[0].record;

    testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
}

void DpCatalogTester ::test_TreeTestManual3() {
    Svc::DpCatalogTester tester;
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[3];
    Svc::DpCatalog::DpStateEntry outputs[3];

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

    outputs[0].record = inputs[1].record;
    outputs[1].record = inputs[0].record;
    outputs[2].record = inputs[2].record;

    testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
}

void DpCatalogTester ::test_TreeTestManual5() {
    Svc::DpCatalog::DpStateEntry inputs[5];
    Svc::DpCatalog::DpStateEntry outputs[5];

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

    outputs[0].record = inputs[1].record;
    outputs[1].record = inputs[0].record;
    outputs[2].record = inputs[2].record;
    outputs[3].record = inputs[4].record;
    outputs[4].record = inputs[3].record;

    testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
}

void DpCatalogTester ::test_TreeTestRandomPriority() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];
        Svc::DpCatalog::DpStateEntry outputs[NUM_ENTRIES];

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

        std::partial_sort_copy(std::begin(inputs), std::end(inputs), std::begin(outputs), std::end(outputs));

        tester.testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestRandomTime() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];
        Svc::DpCatalog::DpStateEntry outputs[NUM_ENTRIES];

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

        std::partial_sort_copy(std::begin(inputs), std::end(inputs), std::begin(outputs), std::end(outputs));

        testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestRandomId() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];
        Svc::DpCatalog::DpStateEntry outputs[NUM_ENTRIES];

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

        std::partial_sort_copy(std::begin(inputs), std::end(inputs), std::begin(outputs), std::end(outputs));

        testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

void DpCatalogTester ::test_TreeTestRandomPrioIdTime() {
    static const FwIndexType NUM_ENTRIES = Svc::DP_MAX_FILES;
    static const FwIndexType NUM_ITERS = 100;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];
        Svc::DpCatalog::DpStateEntry outputs[NUM_ENTRIES];

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

        std::partial_sort_copy(std::begin(inputs), std::end(inputs), std::begin(outputs), std::end(outputs));

        tester.testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
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
    // Test on unconfigured non-waiting component
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, 0xDEADC0DE));
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileXmitError_SIZE(1);

    // Now configure and place component in wait operations

    Fw::FileNameString stateFile("");
    Fw::MallocAllocator alloc;

    Fw::FileNameString dirs[1];
    this->component.configure(dirs, 0, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::WAIT, false);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    // Clear that catalog to short circuit removal logic
    // Simulate a file that failed after cleanup (otherwise it'd be in the catalog)
    this->sendCmd_CLEAR_CATALOG(0, 12);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_CLEAR_CATALOG, 12, Fw::CmdResponse::OK);

    // Now send a file that will generate a wait response
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, 0xDEADC0DE));
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileXmitError_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(3);
    ASSERT_CMD_RESPONSE(2, DpCatalog::OPCODE_START_XMIT_CATALOG, 11, Fw::CmdResponse::EXECUTION_ERROR);

    // Finally, a file done that won't generate a delayed cmd response
    this->invoke_to_fileDone(0, Svc::SendFileResponse(Svc::SendFileStatus::STATUS_ERROR, 0xDEADC0DE));
    this->component.doDispatch();
    ASSERT_EVENTS_DpFileXmitError_SIZE(3);
    ASSERT_CMD_RESPONSE_SIZE(3);
    this->component.shutdown();
}

void DpCatalogTester::test_DeleteDp_NotFound() {
    // Create some DPs and build catalog
    Fw::FileNameString dir;
    dir = "./DpTest_DeleteNotFound";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    this->clearHistory();

    // Try to delete a non-existent DP
    this->sendCmd_DELETE_DP(0, 11, 999, 9999, 9999);
    this->component.doDispatch();

    // Should get DpNotFound event and EXECUTION_ERROR response
    ASSERT_EVENTS_DpNotFound_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_DeleteDp_Success() {
    // Create 3 DPs and build catalog
    Fw::FileNameString dir;
    dir = "./DpTest_DeleteSuccess";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    Fw::Time time2(1000, 200);
    Fw::Time time3(1000, 300);

    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 10, time2, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(3, 10, time3, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 10, Fw::CmdResponse::OK);

    this->clearHistory();

    // Delete the middle DP (ID=2)
    this->sendCmd_DELETE_DP(0, 11, 2, 1000, 200);
    this->component.doDispatch();

    // Should get DpDeleted event and OK response
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::OK);

    // Verify file no longer exists
    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir.toChar(), 2, 1000, 200);
    FwSizeType fileSize = 0;
    Os::FileSystem::Status stat = Os::FileSystem::getFileSize(fileName.toChar(), fileSize);
    ASSERT_NE(stat, Os::FileSystem::OP_OK);

    // Verify counters updated (should have 2 pending files now)
    ASSERT_EQ(this->component.m_pendingFiles, 2);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
    this->delDp(3, time3, dir.toChar());
}

void DpCatalogTester::test_DeleteDp_CurrentlyTransmitting() {
    // Create 2 DPs and start transmission
    Fw::FileNameString dir;
    dir = "./DpTest_DeleteXmit";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    Fw::Time time2(1000, 200);

    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 10, time2, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Start transmission
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();  // Process START_XMIT command

    // Verify first file is being transmitted
    ASSERT_from_fileOut_SIZE(1);
    // m_currentXmitNode should be set (test class is friend, can access)
    ASSERT_TRUE(this->component.m_currentXmitNode != nullptr);

    // Try to delete the currently transmitting DP (ID=1) before fileDone is processed
    this->sendCmd_DELETE_DP(0, 12, 1, 1000, 100);
    // Process queued messages: fileDone and DELETE_DP
    this->component.doDispatch();  // Process fileDone (completes file 1, starts file 2)
    this->component.doDispatch();  // Process DELETE_DP command (tries to delete file 1)

    // Note: file 1 has completed by the time DELETE_DP is processed,
    // so it's no longer "currently transmitting". File 2 is now current.
    // Therefore, the deletion should SUCCEED since file 1 is done.
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(2);  // START_XMIT and DELETE_DP responses
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::OK);

    // Verify file no longer exists (was deleted)
    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir.toChar(), 1, 1000, 100);
    FwSizeType fileSize = 0;
    Os::FileSystem::Status stat = Os::FileSystem::getFileSize(fileName.toChar(), fileSize);
    ASSERT_NE(stat, Os::FileSystem::OP_OK);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
    this->delDp(2, time2, dir.toChar());
}

void DpCatalogTester::test_DeleteDp_DuringTransmission() {
    // Create 3 DPs and start transmission
    Fw::FileNameString dir;
    dir = "./DpTest_DeleteDuring";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    Fw::Time time2(1000, 200);
    Fw::Time time3(1000, 300);

    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 10, time2, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(3, 10, time3, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Start transmission
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();  // Process START_XMIT and send first file

    // First file should be sent
    ASSERT_from_fileOut_SIZE(1);

    // Delete third DP while first is still pending (fileDone not yet processed)
    this->sendCmd_DELETE_DP(0, 12, 3, 1000, 300);
    // Process all queued messages: fileDone for file 1, DELETE_DP command
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    // Should succeed - verify event and response
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    // 2 responses: START_XMIT and DELETE_DP
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_DELETE_DP, 12, Fw::CmdResponse::OK);

    // Verify file 3 no longer exists
    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir.toChar(), 3, 1000, 300);
    FwSizeType fileSize = 0;
    Os::FileSystem::Status stat = Os::FileSystem::getFileSize(fileName.toChar(), fileSize);
    ASSERT_NE(stat, Os::FileSystem::OP_OK);

    // Continue transmission - should only transmit 2 files total (not 3)
    // File 1 already transmitted, now transmit file 2
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    // Should have transmitted only 2 files (1 and 2, not 3)
    ASSERT_from_fileOut_SIZE(2);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
    this->delDp(2, time2, dir.toChar());
    // Note: file 3 already deleted by DELETE_DP command
}

void DpCatalogTester::test_DeleteDp_AlreadyTransmitted() {
    // Create a DP with TRANSMITTED state
    Fw::FileNameString dir;
    dir = "./DpTest_DeleteTransmitted";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);

    // Generate DP with TRANSMITTED state
    this->genDP(1, 10, time1, 100, Fw::DpState::TRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Delete the transmitted DP - it won't be in the tree but file exists
    this->sendCmd_DELETE_DP(0, 11, 1, 1000, 100);
    this->component.doDispatch();

    // Should succeed (file removed even though not in catalog tree)
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::OK);

    // Verify file no longer exists
    Fw::String fileName;
    fileName.format(DP_FILENAME_FORMAT, dir.toChar(), 1, 1000, 100);
    FwSizeType fileSize = 0;
    Os::FileSystem::Status stat = Os::FileSystem::getFileSize(fileName.toChar(), fileSize);
    ASSERT_NE(stat, Os::FileSystem::OP_OK);

    // Cleanup
    this->component.shutdown();
}

void DpCatalogTester::test_ChangeDpPriority_NotFound() {
    Fw::FileNameString dir;
    dir = "./DpTest_ChangePrioNotFound";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Try to change priority of non-existent DP
    this->sendCmd_CHANGE_DP_PRIORITY(0, 11, 999, 2000, 200, 5);
    this->component.doDispatch();

    // Should fail
    ASSERT_EVENTS_DpPriorityNotFound_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_CHANGE_DP_PRIORITY, 11, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_ChangeDpPriority_Success() {
    Fw::FileNameString dir;
    dir = "./DpTest_ChangePrioSuccess";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Change priority
    this->sendCmd_CHANGE_DP_PRIORITY(0, 11, 1, 1000, 100, 5);
    this->component.doDispatch();

    // Should succeed
    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged(0, 1, 1000, 100, 10, 5);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_CHANGE_DP_PRIORITY, 11, Fw::CmdResponse::OK);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_ChangeDpPriority_CurrentlyTransmitting() {
    Fw::FileNameString dir;
    dir = "./DpTest_ChangePrioXmit";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    // Start transmission
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();

    this->clearHistory();

    // Try to change priority while/after transmitting
    // Note: In the test harness, file transmission completes immediately,
    // so the file is removed from the catalog tree by the time this command executes
    this->sendCmd_CHANGE_DP_PRIORITY(0, 12, 1, 1000, 100, 5);
    this->component.doDispatch();

    // Drain message queue
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    // Should fail because file has been transmitted and removed from catalog
    ASSERT_EVENTS_DpPriorityNotFound_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_CHANGE_DP_PRIORITY, 12, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_ChangeDpPriority_SamePriority() {
    Fw::FileNameString dir;
    dir = "./DpTest_ChangePrioSame";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Change priority to same value
    this->sendCmd_CHANGE_DP_PRIORITY(0, 11, 1, 1000, 100, 10);
    this->component.doDispatch();

    // Should succeed (but no actual change)
    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged(0, 1, 1000, 100, 10, 10);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_CHANGE_DP_PRIORITY, 11, Fw::CmdResponse::OK);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_ChangeDpPriority_ReorderTree() {
    Fw::FileNameString dir;
    dir = "./DpTest_ChangePrioReorder";
    this->makeDpDir(dir.toChar());

    // Create 3 DPs with different priorities
    Fw::Time time1(1000, 100);
    Fw::Time time2(2000, 200);
    Fw::Time time3(3000, 300);

    // DP1: priority 10 (should be sent 2nd)
    // DP2: priority 5 (should be sent 1st)
    // DP3: priority 15 (should be sent 3rd)
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 5, time2, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(3, 15, time3, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Change DP1 priority from 10 to 20 (should move it to last position)
    this->sendCmd_CHANGE_DP_PRIORITY(0, 11, 1, 1000, 100, 20);
    this->component.doDispatch();

    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged(0, 1, 1000, 100, 10, 20);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_CHANGE_DP_PRIORITY, 11, Fw::CmdResponse::OK);

    this->clearHistory();

    // Now transmit and verify order: DP2 (prio 5), DP3 (prio 15), DP1 (prio 20)
    this->sendCmd_START_XMIT_CATALOG(0, 12, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();

    // Drain message queue
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    // Verify transmission order
    ASSERT_from_fileOut_SIZE(3);

    // First should be DP2 (priority 5)
    Fw::String expectedFile2;
    expectedFile2.format(DP_FILENAME_FORMAT, dir.toChar(), 2, 2000, 200);
    ASSERT_from_fileOut(0, expectedFile2, expectedFile2, 0, 0);

    // Second should be DP3 (priority 15)
    Fw::String expectedFile3;
    expectedFile3.format(DP_FILENAME_FORMAT, dir.toChar(), 3, 3000, 300);
    ASSERT_from_fileOut(1, expectedFile3, expectedFile3, 0, 0);

    // Third should be DP1 (priority 20, changed from 10)
    Fw::String expectedFile1;
    expectedFile1.format(DP_FILENAME_FORMAT, dir.toChar(), 1, 1000, 100);
    ASSERT_from_fileOut(2, expectedFile1, expectedFile1, 0, 0);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
    this->delDp(2, time2, dir.toChar());
    this->delDp(3, time3, dir.toChar());
}

void DpCatalogTester::test_RetransmitDp_NotFound() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitNotFound";
    this->makeDpDir(dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Try to retransmit non-existent DP
    this->sendCmd_RETRANSMIT_DP(0, 11, 999, 2000, 200, 0xFFFFFFFF);
    this->component.doDispatch();

    // Should fail
    ASSERT_EVENTS_DpNotFound_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 11, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    this->component.shutdown();
}

void DpCatalogTester::test_RetransmitDp_Success_FilePriority() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitFilePrio";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    // Generate DP with TRANSMITTED state and priority 10
    this->genDP(1, 10, time1, 100, Fw::DpState::TRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Retransmit DP using file priority (0xFFFFFFFF)
    this->sendCmd_RETRANSMIT_DP(0, 11, 1, 1000, 100, 0xFFFFFFFF);
    this->component.doDispatch();

    // Should succeed with priority from file (10)
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted(0, "./DpTest_RetransmitFilePrio/Dp_00000001_00001000_00000100.fdp", 10);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 11, Fw::CmdResponse::OK);

    // Verify DP is now in catalog
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 10);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_RetransmitDp_Success_OverridePriority() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitOverride";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    // Generate DP with TRANSMITTED state and priority 10
    this->genDP(1, 10, time1, 100, Fw::DpState::TRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Retransmit DP with overridden priority of 5
    this->sendCmd_RETRANSMIT_DP(0, 11, 1, 1000, 100, 5);
    this->component.doDispatch();

    // Should succeed with overridden priority (5)
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted(0, "./DpTest_RetransmitOverride/Dp_00000001_00001000_00000100.fdp", 5);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 11, Fw::CmdResponse::OK);

    // Verify DP is in catalog with priority 5
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 5);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_RetransmitDp_AlreadyInCatalog() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitAlready";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    // Generate DP with UNTRANSMITTED state (still in catalog) with priority 10
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Retransmit DP that's already in catalog with new priority
    this->sendCmd_RETRANSMIT_DP(0, 11, 1, 1000, 100, 5);
    this->component.doDispatch();

    // Should succeed and update priority
    ASSERT_EVENTS_DpPriorityUpdated_SIZE(1);
    ASSERT_EVENTS_DpPriorityUpdated(0, 1, 1000, 100, 10, 5);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 11, Fw::CmdResponse::OK);

    // Verify DP priority was updated in catalog
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 5);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_RetransmitDp_AlreadyInCatalog_FilePriority() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitAlreadyFile";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);
    // Generate DP with UNTRANSMITTED state (still in catalog) with priority 10
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    this->clearHistory();

    // Retransmit DP that's already in catalog using file priority (0xFFFFFFFF)
    // Since file has priority 10 and catalog has priority 10, should be no change
    this->sendCmd_RETRANSMIT_DP(0, 11, 1, 1000, 100, 0xFFFFFFFF);
    this->component.doDispatch();

    // Should succeed with same priority (no actual change)
    ASSERT_EVENTS_DpPriorityUpdated_SIZE(1);
    ASSERT_EVENTS_DpPriorityUpdated(0, 1, 1000, 100, 10, 10);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 11, Fw::CmdResponse::OK);

    // Verify DP priority unchanged in catalog
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 10);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_RetransmitDp_CurrentlyTransmitting() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitXmit";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);

    // Generate DP with UNTRANSMITTED state
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    // Start transmission
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();

    this->clearHistory();

    // Try to retransmit DP1 while it's still in the catalog (not yet transmitted or immediately after)
    // Note: in test harness, transmission completes immediately, so DP is already sent
    this->sendCmd_RETRANSMIT_DP(0, 12, 1, 1000, 100, 0xFFFFFFFF);
    this->component.doDispatch();

    // Drain message queue
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    // In the test harness, transmission completes immediately, so the DP has been
    // transmitted and removed from catalog by the time RETRANSMIT_DP processes.
    // This means it will successfully re-add the DP for retransmission.
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 12, Fw::CmdResponse::OK);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_RetransmitDp_AfterTransmission() {
    Fw::FileNameString dir;
    dir = "./DpTest_RetransmitAfter";
    this->makeDpDir(dir.toChar());

    Fw::Time time1(1000, 100);

    // Generate DP with UNTRANSMITTED state
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = dir;
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);

    // Start and complete transmission
    this->sendCmd_START_XMIT_CATALOG(0, 11, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();

    // Drain message queue to complete transmission
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    this->clearHistory();

    // Now retransmit the DP with new priority
    this->sendCmd_RETRANSMIT_DP(0, 12, 1, 1000, 100, 3);
    this->component.doDispatch();

    // Should succeed
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted(0, "./DpTest_RetransmitAfter/Dp_00000001_00001000_00000100.fdp", 3);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_RETRANSMIT_DP, 12, Fw::CmdResponse::OK);

    // Verify DP is back in catalog with new priority
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 3);

    // Start transmission again to verify it gets sent
    this->clearHistory();
    this->sendCmd_START_XMIT_CATALOG(0, 13, Fw::Wait::NO_WAIT, false);
    this->component.doDispatch();

    // Drain message queue
    while (this->component.m_queue.getMessagesAvailable() > 0) {
        this->component.doDispatch();
    }

    // Should have sent the file
    ASSERT_from_fileOut_SIZE(1);

    // Cleanup
    this->component.shutdown();
    this->delDp(1, time1, dir.toChar());
}

void DpCatalogTester::test_ProcessDpFile_InvalidFile() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Try to process non-existent file
    Fw::FileNameString opFile("./nonexistent_ops.dat");
    this->sendCmd_PROCESS_DP_FILE(0, 10, opFile);
    this->component.doDispatch();

    // Should get error event and response
    ASSERT_EVENTS_DpFileOpenError_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 10, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessDpFile_InvalidSize() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Create file with invalid size (not multiple of 17)
    Fw::FileNameString opFile;
    opFile.format("%s/inv_size.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);
    U8 data[10] = {0};  // 10 bytes, not a multiple of 17
    FwSizeType size = 10;
    file.write(data, size);
    file.close();

    this->sendCmd_PROCESS_DP_FILE(0, 11, opFile);
    this->component.doDispatch();

    // Should get invalid size event and error response
    ASSERT_EVENTS_DpFileInvalidSize_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 11, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    Os::FileSystem::removeFile(opFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessDpFile_InvalidOp() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Create file with invalid operation code
    Fw::FileNameString opFile;
    opFile.format("%s/inv_op.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);
    U8 data[17] = {0};
    data[0] = 99;  // Invalid operation code
    // ID, tSec, tSub, priority all zeros
    FwSizeType size = 17;
    file.write(data, size);
    file.close();

    this->sendCmd_PROCESS_DP_FILE(0, 12, opFile);
    this->component.doDispatch();

    // Should get invalid op event and error response
    ASSERT_EVENTS_DpFileInvalidOp_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 12, Fw::CmdResponse::EXECUTION_ERROR);

    // Cleanup
    Os::FileSystem::removeFile(opFile.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessDpFile_DeleteOps() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Create 3 DPs
    Fw::Time time1(1000, 100);
    Fw::Time time2(2000, 200);
    Fw::Time time3(3000, 300);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 15, time2, 150, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(3, 20, time3, 200, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    // Build catalog
    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 0, Fw::CmdResponse::OK);
    this->clearHistory();

    // Create operations file with DELETE operations for DP 1 and 3
    Fw::FileNameString opFile;
    opFile.format("%s/del.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);

    // Record 1: DELETE DP 1
    U8 rec1[17] = {
        1,  // DELETE operation
        0, 0, 0, 1,  // ID = 1
        0, 0, 0x03, 0xE8,  // tSec = 1000
        0, 0, 0, 0x64,  // tSub = 100
        0, 0, 0, 0  // priority (ignored for DELETE)
    };
    FwSizeType size = 17;
    file.write(rec1, size);

    // Record 2: DELETE DP 3
    U8 rec2[17] = {
        1,  // DELETE operation
        0, 0, 0, 3,  // ID = 3
        0, 0, 0x0B, 0xB8,  // tSec = 3000
        0, 0, 0x01, 0x2C,  // tSub = 300
        0, 0, 0, 0  // priority (ignored for DELETE)
    };
    file.write(rec2, size);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_FILE(0, 13, opFile);
    this->component.doDispatch();

    // Should get processing started/complete events and success response
    ASSERT_EVENTS_DpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpDeleted_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 13, Fw::CmdResponse::OK);

    // Verify DP 1 and 3 are deleted, DP 2 still exists
    Fw::FileNameString dp1File;
    dp1File.format(DP_FILENAME_FORMAT, dir.toChar(), 1, 1000, 100);
    Fw::FileNameString dp2File;
    dp2File.format(DP_FILENAME_FORMAT, dir.toChar(), 2, 2000, 200);
    Fw::FileNameString dp3File;
    dp3File.format(DP_FILENAME_FORMAT, dir.toChar(), 3, 3000, 300);

    FwSizeType fileSize;
    ASSERT_EQ(Os::FileSystem::getFileSize(dp1File.toChar(), fileSize), Os::FileSystem::DOESNT_EXIST);
    ASSERT_EQ(Os::FileSystem::getFileSize(dp2File.toChar(), fileSize), Os::FileSystem::OP_OK);
    ASSERT_EQ(Os::FileSystem::getFileSize(dp3File.toChar(), fileSize), Os::FileSystem::DOESNT_EXIST);

    // Cleanup
    Os::FileSystem::removeFile(opFile.toChar());
    this->delDp(2, time2, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessDpFile_ReprioritizeOps() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Create 2 DPs
    Fw::Time time1(1000, 100);
    Fw::Time time2(2000, 200);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 15, time2, 150, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    // Build catalog
    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    this->clearHistory();

    // Create operations file with REPRIORITIZE operations
    Fw::FileNameString opFile;
    opFile.format("%s/reprio.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);

    // Record: REPRIORITIZE DP 1 to priority 5
    U8 rec[17] = {
        2,  // REPRIORITIZE operation
        0, 0, 0, 1,  // ID = 1
        0, 0, 0x03, 0xE8,  // tSec = 1000
        0, 0, 0, 0x64,  // tSub = 100
        0, 0, 0, 5  // new priority = 5
    };
    FwSizeType size = 17;
    file.write(rec, size);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_FILE(0, 14, opFile);
    this->component.doDispatch();

    // Should get success events
    ASSERT_EVENTS_DpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 14, Fw::CmdResponse::OK);

    // Verify priority was changed
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 5);

    // Cleanup
    Os::FileSystem::removeFile(opFile.toChar());
    this->delDp(1, time1, dir.toChar());
    this->delDp(2, time2, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessDpFile_RetransmitOps() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Create 1 DP
    Fw::Time time1(1000, 100);
    this->genDP(1, 10, time1, 100, Fw::DpState::TRANSMITTED, false, dir.toChar());

    // Build catalog (won't include TRANSMITTED DP)
    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    this->clearHistory();

    // Create operations file with RETRANSMIT operation
    Fw::FileNameString opFile;
    opFile.format("%s/retx.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);

    // Record: RETRANSMIT DP 1 with priority 5
    U8 rec[17] = {
        3,  // RETRANSMIT operation
        0, 0, 0, 1,  // ID = 1
        0, 0, 0x03, 0xE8,  // tSec = 1000
        0, 0, 0, 0x64,  // tSub = 100
        0, 0, 0, 5  // priority = 5
    };
    FwSizeType size = 17;
    file.write(rec, size);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_FILE(0, 15, opFile);
    this->component.doDispatch();

    // Should get success events
    ASSERT_EVENTS_DpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 15, Fw::CmdResponse::OK);

    // Verify DP was added to catalog
    DpCatalog::DpBtreeNode* node = this->component.findTreeNode(1, 1000, 100);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->entry.record.get_priority(), 5);

    // Cleanup
    Os::FileSystem::removeFile(opFile.toChar());
    this->delDp(1, time1, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_ProcessDpFile_MixedOps() {
    // Initialize component
    Fw::MallocAllocator alloc;
    Fw::FileNameString dir;
    dir = "./DpTest_ProcessFile";
    Fw::FileNameString stateFile("./DpTest/dpState.dat");
    this->makeDpDir(dir.toChar());
    this->component.configure(&dir, 1, stateFile, 0, alloc);

    // Create 4 DPs: 3 UNTRANSMITTED, 1 TRANSMITTED
    Fw::Time time1(1000, 100);
    Fw::Time time2(2000, 200);
    Fw::Time time3(3000, 300);
    Fw::Time time4(4000, 400);
    this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(2, 15, time2, 150, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(3, 20, time3, 200, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    this->genDP(4, 25, time4, 250, Fw::DpState::TRANSMITTED, false, dir.toChar());

    // Build catalog
    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    this->clearHistory();

    // Create operations file with mixed operations
    Fw::FileNameString opFile;
    opFile.format("%s/mixed.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);

    // Record 1: DELETE DP 1
    U8 rec1[17] = {1, 0, 0, 0, 1, 0, 0, 0x03, 0xE8, 0, 0, 0, 0x64, 0, 0, 0, 0};
    FwSizeType size = 17;
    file.write(rec1, size);

    // Record 2: REPRIORITIZE DP 2 to priority 5
    U8 rec2[17] = {2, 0, 0, 0, 2, 0, 0, 0x07, 0xD0, 0, 0, 0, 0xC8, 0, 0, 0, 5};
    file.write(rec2, size);

    // Record 3: RETRANSMIT DP 4 with priority 3
    U8 rec3[17] = {3, 0, 0, 0, 4, 0, 0, 0x0F, 0xA0, 0, 0, 0x01, 0x90, 0, 0, 0, 3};
    file.write(rec3, size);

    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_FILE(0, 16, opFile);
    this->component.doDispatch();

    // Should get success events
    ASSERT_EVENTS_DpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_FILE, 16, Fw::CmdResponse::OK);

    // Verify results
    // DP 1 should be deleted
    Fw::FileNameString dp1File;
    dp1File.format(DP_FILENAME_FORMAT, dir.toChar(), 1, 1000, 100);
    FwSizeType fileSize;
    ASSERT_EQ(Os::FileSystem::getFileSize(dp1File.toChar(), fileSize), Os::FileSystem::DOESNT_EXIST);

    // DP 2 should have new priority
    DpCatalog::DpBtreeNode* node2 = this->component.findTreeNode(2, 2000, 200);
    ASSERT_TRUE(node2 != nullptr);
    ASSERT_EQ(node2->entry.record.get_priority(), 5);

    // DP 4 should be in catalog with priority 3
    DpCatalog::DpBtreeNode* node4 = this->component.findTreeNode(4, 4000, 400);
    ASSERT_TRUE(node4 != nullptr);
    ASSERT_EQ(node4->entry.record.get_priority(), 3);

    // Cleanup
    Os::FileSystem::removeFile(opFile.toChar());
    this->delDp(2, time2, dir.toChar());
    this->delDp(3, time3, dir.toChar());
    this->delDp(4, time4, dir.toChar());
    this->component.shutdown();
}

}  // namespace Svc
