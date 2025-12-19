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
}

}  // namespace Svc
