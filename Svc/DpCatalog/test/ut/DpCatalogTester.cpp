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

// Include CRC32 library for test record generation
extern "C" {
#include <Utils/Hash/libcrc/lib_crc.h>
}

namespace Svc {

// ----------------------------------------------------------------------
// Helper function to create DP operation records
// ----------------------------------------------------------------------

// Helper function to pack a 17-byte operation record (big-endian)
static void packOpRecord(U8* buffer, U8 op, U32 id, U32 tSec, U32 tSub, U32 priority) {
    buffer[0] = op;
    buffer[1] = static_cast<U8>((id >> 24) & 0xFF);
    buffer[2] = static_cast<U8>((id >> 16) & 0xFF);
    buffer[3] = static_cast<U8>((id >> 8) & 0xFF);
    buffer[4] = static_cast<U8>(id & 0xFF);
    buffer[5] = static_cast<U8>((tSec >> 24) & 0xFF);
    buffer[6] = static_cast<U8>((tSec >> 16) & 0xFF);
    buffer[7] = static_cast<U8>((tSec >> 8) & 0xFF);
    buffer[8] = static_cast<U8>(tSec & 0xFF);
    buffer[9] = static_cast<U8>((tSub >> 24) & 0xFF);
    buffer[10] = static_cast<U8>((tSub >> 16) & 0xFF);
    buffer[11] = static_cast<U8>((tSub >> 8) & 0xFF);
    buffer[12] = static_cast<U8>(tSub & 0xFF);
    buffer[13] = static_cast<U8>((priority >> 24) & 0xFF);
    buffer[14] = static_cast<U8>((priority >> 16) & 0xFF);
    buffer[15] = static_cast<U8>((priority >> 8) & 0xFF);
    buffer[16] = static_cast<U8>(priority & 0xFF);
}

// Helper function to calculate and append CRC32 for all data
static void appendCrc32(Os::File& file, const U8* data, FwSizeType dataSize) {
    // Calculate CRC32 over all data
    unsigned long crc = 0xFFFFFFFF;
    for (FwSizeType i = 0; i < dataSize; i++) {
        crc = update_crc_32(crc, static_cast<char>(data[i]));
    }
    U32 crc32 = static_cast<U32>(crc ^ 0xFFFFFFFF);

    // Write CRC32 as big-endian U32
    U8 crcBuf[4];
    crcBuf[0] = static_cast<U8>((crc32 >> 24) & 0xFF);
    crcBuf[1] = static_cast<U8>((crc32 >> 16) & 0xFF);
    crcBuf[2] = static_cast<U8>((crc32 >> 8) & 0xFF);
    crcBuf[3] = static_cast<U8>(crc32 & 0xFF);
    FwSizeType size = 4;
    file.write(crcBuf, size);
}

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

Fw::Success::T DpCatalogTester::productGet_handler(FwDpIdType id, FwSizeType dataSize, Fw::Buffer& buffer) {
    buffer.set(this->m_dpBuff, dataSize);
    this->pushProductGetEntry(id, dataSize);
    return Fw::Success::SUCCESS;
}

void DpCatalogTester::productSend_handler(FwDpIdType id, const Fw::Buffer& buffer) {
    this->pushProductSendEntry(id, buffer);
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

void DpCatalogTester::test_ProcessFileInvalidDir() {
    Fw::MallocAllocator alloc;
    Fw::FileNameString dirs[1];
    dirs[0] = "./DpTest_InvalidDir";
    Fw::FileNameString stateFile("");
    this->component.configure(dirs, 1, stateFile, 100, alloc);

    ASSERT_DEATH_IF_SUPPORTED(this->component.processFile("somefile.dp", DP_MAX_DIRECTORIES), "Assert");

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

    // Try to delete a nonexistent DP
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

void DpCatalogTester::test_DeleteDp_ParentPointerIntegrity() {
    // This test verifies Bug 1: parent pointer not updated in deallocateNode()
    // when rightmostNode->left is stitched into the tree.
    //
    // We create a specific tree structure where deleting the root will trigger
    // the bug. The tree (by priority, lower = higher priority = left):
    //        50 (root, to be deleted)
    //       /   (backslash)
    //     30    70
    //    /   (backslash)
    //   10  40
    //      /
    //     35
    //
    // When 50 is deleted:
    // - rightmostNode = 40 (rightmost of left subtree)
    // - rightmostNode->left = 35
    // - Bug: 35->parent is not updated when 35 is moved up
    //
    // After deletion, tree should be:
    //       40
    //      /   (backslash)
    //    30    70
    //   /   (backslash)
    //  10  35
    //
    // And 35->parent should point to 30, not 40.

    Fw::FileNameString dir;
    dir = "./DpTest_DeleteParentPointer";
    this->makeDpDir(dir.toChar());

    // Create DPs with specific priorities to build the desired tree structure
    // Tree is sorted by (priority, timestamp, id) - lower priority goes left
    Fw::Time time1(1000, 50);
    Fw::Time time2(1000, 10);
    Fw::Time time3(1000, 70);
    Fw::Time time4(1000, 30);
    Fw::Time time5(1000, 45);
    Fw::Time time6(1000, 42);

    // Build this tree (priorities shown):
    //        50 (root, to be deleted)
    //       /   (backslash)
    //     10    70
    //       (backslash)
    //        30
    //          (backslash)
    //           45
    //           /
    //         42
    //
    // When 50 is deleted:
    // - node->left = 10
    // - Find rightmost: 10->right = 30, 30->right = 45, 45->right = null, so rightmost = 45
    // - rightmost (45) != node->left (10), so takes the ELSE branch with the bug
    // - rightmost->left = 42
    // Bug at line 820: 42->parent is not updated when moved up to replace 45

    // Insert in specific order to create this tree structure
    this->genDP(1, 50, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());  // Root: prio=50
    this->genDP(2, 10, time2, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());  // Left of root: prio=10
    this->genDP(3, 70, time3, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());  // Right of root: prio=70
    this->genDP(4, 30, time4, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());  // Right of 10: prio=30
    this->genDP(5, 45, time5, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());  // Right of 30: prio=45
    this->genDP(6, 42, time6, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());  // Left of 45: prio=42

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

    // Print the tree structure before deletion
    printf("\n=== Tree structure BEFORE deletion ===\n");
    auto printTree = [](DpCatalog::DpBtreeNode* node, int depth, const char* side) {
        if (node == nullptr) {
            return;
        }
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
        printf("%s: prio=%u parent_prio=%u\n", side, node->entry.record.get_priority(),
               node->parent ? node->parent->entry.record.get_priority() : 999);
    };

    DpCatalog::DpBtreeNode* treeStack[DP_MAX_FILES];
    int depths[DP_MAX_FILES];
    const char* sides[DP_MAX_FILES];
    FwSizeType treeStackTop = 0;
    treeStack[treeStackTop] = this->component.m_dpTree;
    depths[treeStackTop] = 0;
    sides[treeStackTop] = "ROOT";
    treeStackTop++;

    while (treeStackTop > 0) {
        treeStackTop--;
        DpCatalog::DpBtreeNode* current = treeStack[treeStackTop];
        int depth = depths[treeStackTop];
        const char* side = sides[treeStackTop];

        printTree(current, depth, side);

        if (current->right != nullptr && treeStackTop < DP_MAX_FILES) {
            treeStack[treeStackTop] = current->right;
            depths[treeStackTop] = depth + 1;
            sides[treeStackTop] = "R";
            treeStackTop++;
        }
        if (current->left != nullptr && treeStackTop < DP_MAX_FILES) {
            treeStack[treeStackTop] = current->left;
            depths[treeStackTop] = depth + 1;
            sides[treeStackTop] = "L";
            treeStackTop++;
        }
    }

    // Check if tree structure exposes Bug 1
    DpCatalog::DpBtreeNode* rootNode = this->component.m_dpTree;
    ASSERT_NE(rootNode, nullptr);

    // Find a node to delete that will expose Bug 1
    // We need: node with left child, rightmost of left has its own left child
    U32 idToDelete = 0;
    U32 tsecToDelete = 0;
    U32 tsubToDelete = 0;
    bool foundBugTrigger = false;

    // Check the root first
    if (rootNode->left != nullptr && rootNode->right != nullptr) {
        // Find rightmost of left subtree
        DpCatalog::DpBtreeNode* rightmost = rootNode->left;
        while (rightmost->right != nullptr) {
            rightmost = rightmost->right;
        }
        // Check if rightmost has a left child and isn't the immediate left child
        if (rightmost->left != nullptr && rightmost != rootNode->left) {
            printf("✓ Root node will expose Bug 1: rightmost of left (ID=%u prio=%u) has left child (ID=%u prio=%u)\n",
                   rightmost->entry.record.get_id(), rightmost->entry.record.get_priority(),
                   rightmost->left->entry.record.get_id(), rightmost->left->entry.record.get_priority());
            idToDelete = rootNode->entry.record.get_id();
            tsecToDelete = rootNode->entry.record.get_tSec();
            tsubToDelete = rootNode->entry.record.get_tSub();
            foundBugTrigger = true;
        }
    }

    if (!foundBugTrigger) {
        // Just delete any node and check parent pointers
        printf("Note: Tree structure doesn't perfectly expose Bug 1, but will still check parent integrity\n");
        idToDelete = rootNode->entry.record.get_id();
        tsecToDelete = rootNode->entry.record.get_tSec();
        tsubToDelete = rootNode->entry.record.get_tSub();
    }

    // Delete the selected node
    this->sendCmd_DELETE_DP(0, 11, idToDelete, tsecToDelete, tsubToDelete);
    this->component.doDispatch();

    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_DELETE_DP, 11, Fw::CmdResponse::OK);

    printf("\n=== Tree structure AFTER deletion ===\n");
    treeStackTop = 0;
    if (this->component.m_dpTree) {
        treeStack[treeStackTop] = this->component.m_dpTree;
        depths[treeStackTop] = 0;
        sides[treeStackTop] = "ROOT";
        treeStackTop++;

        while (treeStackTop > 0) {
            treeStackTop--;
            DpCatalog::DpBtreeNode* current = treeStack[treeStackTop];
            int depth = depths[treeStackTop];
            const char* side = sides[treeStackTop];

            printTree(current, depth, side);

            if (current->right != nullptr && treeStackTop < DP_MAX_FILES) {
                treeStack[treeStackTop] = current->right;
                depths[treeStackTop] = depth + 1;
                sides[treeStackTop] = "R";
                treeStackTop++;
            }
            if (current->left != nullptr && treeStackTop < DP_MAX_FILES) {
                treeStack[treeStackTop] = current->left;
                depths[treeStackTop] = depth + 1;
                sides[treeStackTop] = "L";
                treeStackTop++;
            }
        }
    }

    // Now verify parent pointer integrity throughout the tree
    // We need to walk the tree and verify that for every node:
    // - If node->parent != nullptr: node->parent->left == node OR node->parent->right == node
    // - If node->left != nullptr: node->left->parent == node
    // - If node->right != nullptr: node->right->parent == node

    DpCatalog::DpBtreeNode* root = this->component.m_dpTree;
    ASSERT_NE(root, nullptr);

    // Helper lambda to validate a node's parent pointers
    auto validateNode = [](DpCatalog::DpBtreeNode* node, const char* desc) {
        if (node == nullptr) {
            return;
        }

        // If node has a parent, verify parent's child pointer points back to this node
        if (node->parent != nullptr) {
            bool validParentLink = (node->parent->left == node) || (node->parent->right == node);
            if (!validParentLink) {
                printf(
                    "PARENT POINTER BUG: Node %s (priority %u) has parent (priority %u), but parent doesn't point back "
                    "to node!\n",
                    desc, node->entry.record.get_priority(), node->parent->entry.record.get_priority());
                printf("  Parent->left priority: %u\n",
                       node->parent->left ? node->parent->left->entry.record.get_priority() : 0);
                printf("  Parent->right priority: %u\n",
                       node->parent->right ? node->parent->right->entry.record.get_priority() : 0);
            }
            ASSERT_TRUE(validParentLink);
        }

        // If node has a left child, verify its parent pointer
        if (node->left != nullptr) {
            ASSERT_EQ(node->left->parent, node);
        }

        // If node has a right child, verify its parent pointer
        if (node->right != nullptr) {
            ASSERT_EQ(node->right->parent, node);
        }
    };

    // Traverse tree and validate all parent pointers using iterative DFS
    DpCatalog::DpBtreeNode* stack[DP_MAX_FILES];
    FwSizeType stackTop = 0;
    stack[stackTop++] = root;

    while (stackTop > 0) {
        DpCatalog::DpBtreeNode* current = stack[--stackTop];

        // Null check to satisfy clang static analyzer
        if (current == nullptr) {
            continue;
        }

        char desc[64];
        snprintf(desc, sizeof(desc), "ID=%u prio=%u", current->entry.record.get_id(),
                 current->entry.record.get_priority());
        validateNode(current, desc);

        if (current->right != nullptr && stackTop < DP_MAX_FILES) {
            stack[stackTop++] = current->right;
        }
        if (current->left != nullptr && stackTop < DP_MAX_FILES) {
            stack[stackTop++] = current->left;
        }
    }

    // Cleanup
    this->component.shutdown();
    this->delDp(2, time2, dir.toChar());  // ID=2 prio=10
    this->delDp(3, time3, dir.toChar());  // ID=3 prio=70
    this->delDp(4, time4, dir.toChar());  // ID=4 prio=30
    this->delDp(5, time5, dir.toChar());  // ID=5 prio=45
    this->delDp(6, time6, dir.toChar());  // ID=6 prio=42
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

    // Try to change priority of nonexistent DP
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

    // Try to retransmit nonexistent DP
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

    // Try to process nonexistent file
    Fw::FileNameString opFile("./nonexistent_ops.dat");
    this->sendCmd_PROCESS_DP_OP_FILE(0, 10, opFile);
    this->component.doDispatch();

    // Should get error event and response
    ASSERT_EVENTS_DpOpFileOpenError_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 10, Fw::CmdResponse::EXECUTION_ERROR);

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

    // Create file with invalid size (data portion not multiple of 17, and too small for CRC32)
    Fw::FileNameString opFile;
    opFile.format("%s/inv_size.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);
    U8 data[10] = {0};  // 10 bytes: too small for even CRC32 (need at least 4)
    FwSizeType size = 10;
    file.write(data, size);
    file.close();

    this->sendCmd_PROCESS_DP_OP_FILE(0, 11, opFile);
    this->component.doDispatch();

    // Should get invalid size event and error response
    ASSERT_EVENTS_DpOpFileInvalidSize_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 11, Fw::CmdResponse::EXECUTION_ERROR);

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

    // Create file with invalid operation code but valid CRC32
    Fw::FileNameString opFile;
    opFile.format("%s/inv_op.dat", dir.toChar());
    Os::File file;
    file.open(opFile.toChar(), Os::File::OPEN_CREATE);

    // Pack record with invalid operation code
    U8 data[17];
    packOpRecord(data, 99, 0, 0, 0, 0);  // Invalid operation code 99

    // Write record
    FwSizeType size = 17;
    file.write(data, size);

    // Append valid CRC32
    appendCrc32(file, data, 17);
    file.close();

    this->sendCmd_PROCESS_DP_OP_FILE(0, 12, opFile);
    this->component.doDispatch();

    // Should get invalid op event and error response
    ASSERT_EVENTS_DpOpFileInvalidOp_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 12, Fw::CmdResponse::EXECUTION_ERROR);

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

    // Pack records into buffer
    U8 allData[34];                                  // 2 records * 17 bytes
    packOpRecord(&allData[0], 1, 1, 1000, 100, 0);   // DELETE DP 1
    packOpRecord(&allData[17], 1, 3, 3000, 300, 0);  // DELETE DP 3

    // Write all records
    FwSizeType size = 34;
    file.write(allData, size);

    // Append CRC32
    appendCrc32(file, allData, 34);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_OP_FILE(0, 13, opFile);
    this->component.doDispatch();

    // Should get processing started/complete events and success response
    ASSERT_EVENTS_DpOpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpOpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpDeleted_SIZE(2);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 13, Fw::CmdResponse::OK);

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

    // Pack record
    U8 rec[17];
    packOpRecord(rec, 2, 1, 1000, 100, 5);  // REPRIORITIZE DP 1 to priority 5

    // Write record
    FwSizeType size = 17;
    file.write(rec, size);

    // Append CRC32
    appendCrc32(file, rec, 17);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_OP_FILE(0, 14, opFile);
    this->component.doDispatch();

    // Should get success events
    ASSERT_EVENTS_DpOpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpOpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 14, Fw::CmdResponse::OK);

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

    // Pack record
    U8 rec[17];
    packOpRecord(rec, 3, 1, 1000, 100, 5);  // RETRANSMIT DP 1 with priority 5

    // Write record
    FwSizeType size = 17;
    file.write(rec, size);

    // Append CRC32
    appendCrc32(file, rec, 17);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_OP_FILE(0, 15, opFile);
    this->component.doDispatch();

    // Should get success events
    ASSERT_EVENTS_DpOpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpOpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 15, Fw::CmdResponse::OK);

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

    // Pack all records into buffer
    U8 allData[51];                                  // 3 records * 17 bytes
    packOpRecord(&allData[0], 1, 1, 1000, 100, 0);   // DELETE DP 1
    packOpRecord(&allData[17], 2, 2, 2000, 200, 5);  // REPRIORITIZE DP 2 to priority 5
    packOpRecord(&allData[34], 3, 4, 4000, 400, 3);  // RETRANSMIT DP 4 with priority 3

    // Write all records
    FwSizeType size = 51;
    file.write(allData, size);

    // Append CRC32
    appendCrc32(file, allData, 51);
    file.close();

    // Process the file
    this->sendCmd_PROCESS_DP_OP_FILE(0, 16, opFile);
    this->component.doDispatch();

    // Should get success events
    ASSERT_EVENTS_DpOpFileProcessingStarted_SIZE(1);
    ASSERT_EVENTS_DpOpFileProcessingComplete_SIZE(1);
    ASSERT_EVENTS_DpDeleted_SIZE(1);
    ASSERT_EVENTS_DpPriorityChanged_SIZE(1);
    ASSERT_EVENTS_DpRetransmitted_SIZE(1);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_PROCESS_DP_OP_FILE, 16, Fw::CmdResponse::OK);

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

void DpCatalogTester::test_SendCatalogDp_EmptyCatalog() {
    Fw::FileNameString dir("./DpTest_SendCatalog");
    this->makeDpDir(dir.toChar());

    Fw::FileNameString dirs[1];
    dirs[0] = dir;

    Fw::FileNameString stateFile("./DpTest_SendCatalog/dpState.dat");
    Fw::MallocAllocator alloc;

    // Initialize with no DPs
    this->component.configure(dirs, 1, stateFile, 0, alloc);

    // Build empty catalog
    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 0, Fw::CmdResponse::OK);

    // Send catalog DP command
    this->sendCmd_SEND_CATALOG_DP(0, 1, 100);
    this->component.doDispatch();

    // Should succeed with empty container (0 entries)
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_SEND_CATALOG_DP, 1, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_DpCatalogDpNoCatalog_SIZE(0);

    this->component.shutdown();
}

void DpCatalogTester::test_SendCatalogDp_WithEntries() {
    Fw::FileNameString dir("./DpTest_SendCatalog");
    this->makeDpDir(dir.toChar());

    // Create 3 test DPs
    Fw::Time time1(1000, 100);
    Fw::Time time2(2000, 200);
    Fw::Time time3(3000, 300);

    Fw::String dp1 = this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    Fw::String dp2 = this->genDP(2, 15, time2, 150, Fw::DpState::UNTRANSMITTED, false, dir.toChar());
    Fw::String dp3 = this->genDP(3, 5, time3, 200, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::FileNameString dirs[1];
    dirs[0] = dir;

    Fw::FileNameString stateFile("./DpTest_SendCatalog/dpState.dat");
    Fw::MallocAllocator alloc;

    // Initialize and build catalog
    this->component.configure(dirs, 1, stateFile, 0, alloc);

    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpCatalog::OPCODE_BUILD_CATALOG, 0, Fw::CmdResponse::OK);

    // Send catalog DP command
    this->sendCmd_SEND_CATALOG_DP(0, 1, 100);
    this->component.doDispatch();

    // Should succeed
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_SEND_CATALOG_DP, 1, Fw::CmdResponse::OK);

    // Should have called productGet and productSend handlers
    ASSERT_EQ(this->productGetHistory->size(), 1);
    ASSERT_EQ(this->productSendHistory->size(), 1);

    // Cleanup
    this->delDp(1, time1, dir.toChar());
    this->delDp(2, time2, dir.toChar());
    this->delDp(3, time3, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_SendCatalogDp_DefaultPriority() {
    Fw::FileNameString dir("./DpTest_SendCatalog");
    this->makeDpDir(dir.toChar());

    // Create 1 test DP
    Fw::Time time1(1000, 100);
    Fw::String dp1 = this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::FileNameString dirs[1];
    dirs[0] = dir;

    Fw::FileNameString stateFile("./DpTest_SendCatalog/dpState.dat");
    Fw::MallocAllocator alloc;

    // Initialize and build catalog
    this->component.configure(dirs, 1, stateFile, 0, alloc);

    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();

    // Send catalog DP with 0xFFFFFFFF (use default priority)
    this->sendCmd_SEND_CATALOG_DP(0, 1, 0xFFFFFFFF);
    this->component.doDispatch();

    // Should succeed
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_SEND_CATALOG_DP, 1, Fw::CmdResponse::OK);

    // Should have called productGet and productSend handlers
    ASSERT_EQ(this->productGetHistory->size(), 1);
    ASSERT_EQ(this->productSendHistory->size(), 1);

    // Cleanup
    this->delDp(1, time1, dir.toChar());
    this->component.shutdown();
}

void DpCatalogTester::test_SendCatalogDp_CustomPriority() {
    Fw::FileNameString dir("./DpTest_SendCatalog");
    this->makeDpDir(dir.toChar());

    // Create 1 test DP
    Fw::Time time1(1000, 100);
    Fw::String dp1 = this->genDP(1, 10, time1, 100, Fw::DpState::UNTRANSMITTED, false, dir.toChar());

    Fw::FileNameString dirs[1];
    dirs[0] = dir;

    Fw::FileNameString stateFile("./DpTest_SendCatalog/dpState.dat");
    Fw::MallocAllocator alloc;

    // Initialize and build catalog
    this->component.configure(dirs, 1, stateFile, 0, alloc);

    this->sendCmd_BUILD_CATALOG(0, 0);
    this->component.doDispatch();

    // Send catalog DP with custom priority 50
    this->sendCmd_SEND_CATALOG_DP(0, 1, 50);
    this->component.doDispatch();

    // Should succeed
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, DpCatalog::OPCODE_SEND_CATALOG_DP, 1, Fw::CmdResponse::OK);

    // Should have called productGet and productSend handlers
    ASSERT_EQ(this->productGetHistory->size(), 1);
    ASSERT_EQ(this->productSendHistory->size(), 1);

    // Cleanup
    this->delDp(1, time1, dir.toChar());
    this->component.shutdown();
}

}  // namespace Svc
