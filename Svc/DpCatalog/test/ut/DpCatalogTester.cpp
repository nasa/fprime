// ======================================================================
// \title  DpCatalogTester.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test harness implementation class
// ======================================================================

#include "DpCatalogTester.hpp"
#include <list>
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
}

DpCatalogTester ::~DpCatalogTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void DpCatalogTester ::doInit() {
    Fw::MallocAllocator alloc;

    Fw::FileNameString dirs[2];
    dirs[0] = "dir0";
    dirs[1] = "dir1";
    Fw::FileNameString stateFile("dpState.dat");
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
    Fw::FileNameString stateFile("dpState.dat");
    this->component.configure(dirs, FW_NUM_ARRAY_ELEMENTS(dirs), stateFile, 100, alloc);

    // reset tree
    this->component.resetBinaryTree();

    // add entries
    for (FwIndexType entry = 0; entry < numEntries; entry++) {
        ASSERT_TRUE(this->component.insertEntry(input[entry]));
    }

    // reset stack to read tree
    this->component.resetTreeStack();
    // hot wire in progress
    this->component.m_xmitInProgress = true;

    // retrieve entries - they should match expected output
    for (FwIndexType entry = 0; entry < numEntries + 1; entry++) {
        DpCatalog::DpBtreeNode* res = this->component.findNextTreeNode();
        if (entry == numEntries) {
            // final request should indicate empty
            ASSERT_TRUE(res == nullptr);
            break;
        } else if (output[entry].record.get_state() == Fw::DpState::TRANSMITTED) {
            // if transmitted, should not be returned
            ASSERT_TRUE(res == nullptr);
            // continue to next entry
            continue;
        } else {
            ASSERT_TRUE(res != nullptr);
        }
        // printf("CE: %u\n",entry);
        //  should match expected entry
        ASSERT_EQ(res->entry.record, output[entry].record);
    }

    this->component.shutdown();
}

//! Read one DP test
void DpCatalogTester::readDps(Fw::FileNameString* dpDirs,
                              FwSizeType numDirs,
                              Fw::FileNameString& stateFile,
                              const DpSet* dpSet,
                              FwSizeType numDps) {
    // make a directory for the files
    for (FwSizeType dir = 0; dir < numDirs; dir++) {
        this->makeDpDir(dpDirs[dir].toChar());
    }

    // clean up last DP
    for (FwSizeType dp = 0; dp < numDps; dp++) {
        this->delDp(dpSet[dp].id, dpSet[dp].time, dpSet[dp].dir);

        this->genDP(dpSet[dp].id, dpSet[dp].prio, dpSet[dp].time, dpSet[dp].dataSize, dpSet[dp].state, false,
                    dpSet[dp].dir);
    }

    Fw::MallocAllocator alloc;

    this->component.configure(dpDirs, numDirs, stateFile, 100, alloc);

    this->sendCmd_BUILD_CATALOG(0, 10);
    this->component.doDispatch();
    this->sendCmd_START_XMIT_CATALOG(0, 0, Fw::Wait::NO_WAIT);
    this->component.doDispatch();

    // dispatch messages
    for (FwSizeType msg = 0; msg < numDps; msg++) {
        // dispatch file done port call
        this->component.doDispatch();
    }

    this->component.shutdown();
}

void DpCatalogTester::genDP(FwDpIdType id,
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
        return;
    }
    FwSizeType size = Fw::DpContainer::Header::SIZE;
    stat = dpFile.write(hdrData, size);
    if (stat != Os::File::Status::OP_OK) {
        printf("Error writing DP file header %s: status: %d\n", fileName.toChar(), stat);
        return;
    }
    if (static_cast<FwSizeType>(size) != Fw::DpContainer::Header::SIZE) {
        printf("Dp file header %s write size didn't match. Req: %" PRI_FwSizeType "Act: %" PRI_FwSizeType "\n",
               fileName.toChar(), Fw::DpContainer::Header::SIZE, size);
        return;
    }
    size = dataSize;
    stat = dpFile.write(dpData, size);
    if (stat != Os::File::Status::OP_OK) {
        printf("Error writing DP file data %s: status: %" PRI_FwEnumStoreType "\n", fileName.toChar(),
               static_cast<FwEnumStoreType>(stat));
        return;
    }
    if (static_cast<FwSizeType>(size) != dataSize) {
        printf("Dp file header %s write size didn't match. Req: %" PRI_FwSizeType " Act: %" PRI_FwSizeType "\n",
               fileName.toChar(), dataSize, size);
        return;
    }
    dpFile.close();
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
    this->invoke_to_fileDone(0, Svc::SendFileResponse());

    return Svc::SendFileResponse();
}

void DpCatalogTester ::from_pingOut_handler(FwIndexType portNum, U32 key) {
    // TODO
}

// ----------------------------------------------------------------------
// Moved Tests due to private/protected access
// ----------------------------------------------------------------------

bool DpCatalogTester ::EntryCompare(const Svc::DpCatalog::DpStateEntry& a, const Svc::DpCatalog::DpStateEntry& b) {
    if (a.record.get_priority() == b.record.get_priority()) {  // check priority first - lower value = higher priority
        if (a.record.get_tSec() == b.record.get_tSec()) {      // check time next - older = higher priority
            return a.record.get_id() < b.record.get_id();      // finally check ID - lower = higher priority
        } else {
            return a.record.get_tSec() < b.record.get_tSec();
        }
    } else {
        return a.record.get_priority() < b.record.get_priority();
    }
}

void DpCatalogTester ::test_NominalManual_DISABLED_TreeTestRandomTransmitted() {
    static const FwIndexType NUM_ENTRIES = 10;
    static const FwIndexType NUM_ITERS = 1;

    for (FwIndexType iter = 0; iter < NUM_ITERS; iter++) {
        Svc::DpCatalog::DpStateEntry inputs[NUM_ENTRIES];
        Svc::DpCatalog::DpStateEntry outputs[NUM_ENTRIES];

        Svc::DpCatalogTester tester;
        Fw::FileNameString dir;

        std::list<Svc::DpCatalog::DpStateEntry> entryList;

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
            // randomly set if it is transmitted or not
            randVal = STest::Pick::lowerUpper(0, 1);
            if (randVal == 0) {
                inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
                // only put untransmitted products in list, since the catalog algorithm only returns untransmitted
                // product IDs
                entryList.push_back(inputs[entry]);
            } else {
                inputs[entry].record.set_state(Fw::DpState::TRANSMITTED);
            }
        }

        entryList.sort(EntryCompare);

        FwIndexType entryIndex = 0;

        for (const auto& entry : entryList) {
            outputs[entryIndex].record.set_priority(entry.record.get_priority());
            outputs[entryIndex].record.set_id(entry.record.get_id());
            outputs[entryIndex].record.set_state(entry.record.get_state());
            outputs[entryIndex].record.set_tSec(entry.record.get_tSec());
            outputs[entryIndex].record.set_tSub(1500);
            outputs[entryIndex].record.set_size(100);
            entryIndex++;
        }

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

void DpCatalogTester ::test_TreeTestManual1_Transmitted() {
    Fw::FileNameString dir;

    Svc::DpCatalog::DpStateEntry inputs[1];
    Svc::DpCatalog::DpStateEntry outputs[1];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::TRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    outputs[0].record.set_state(Fw::DpState::TRANSMITTED);

    testTree(inputs, outputs, 1);
}

void DpCatalogTester ::test_TreeTestManual_All_Transmitted() {
    Svc::DpCatalog::DpStateEntry inputs[5];
    Svc::DpCatalog::DpStateEntry outputs[5];

    inputs[0].record.set_id(1);
    inputs[0].record.set_priority(2);
    inputs[0].record.set_state(Fw::DpState::TRANSMITTED);
    inputs[0].record.set_tSec(1000);
    inputs[0].record.set_tSub(1500);
    inputs[0].record.set_size(100);

    inputs[1].record.set_id(2);
    inputs[1].record.set_priority(1);
    inputs[1].record.set_state(Fw::DpState::TRANSMITTED);
    inputs[1].record.set_tSec(1000);
    inputs[1].record.set_tSub(1500);
    inputs[1].record.set_size(100);

    inputs[2].record.set_id(3);
    inputs[2].record.set_priority(3);
    inputs[2].record.set_state(Fw::DpState::TRANSMITTED);
    inputs[2].record.set_tSec(1000);
    inputs[2].record.set_tSub(1500);
    inputs[2].record.set_size(100);

    inputs[3].record.set_id(4);
    inputs[3].record.set_priority(5);
    inputs[3].record.set_state(Fw::DpState::TRANSMITTED);
    inputs[3].record.set_tSec(1000);
    inputs[3].record.set_tSub(1500);
    inputs[3].record.set_size(100);

    inputs[4].record.set_id(5);
    inputs[4].record.set_priority(4);
    inputs[4].record.set_state(Fw::DpState::TRANSMITTED);
    inputs[4].record.set_tSec(1000);
    inputs[4].record.set_tSub(1500);
    inputs[4].record.set_size(100);

    outputs[0].record.set_state(Fw::DpState::TRANSMITTED);
    outputs[1].record.set_state(Fw::DpState::TRANSMITTED);
    outputs[2].record.set_state(Fw::DpState::TRANSMITTED);
    outputs[3].record.set_state(Fw::DpState::TRANSMITTED);
    outputs[4].record.set_state(Fw::DpState::TRANSMITTED);

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

        std::list<Svc::DpCatalog::DpStateEntry> entryList;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(randVal);
            inputs[entry].record.set_id(entry);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            inputs[entry].record.set_tSec(1000);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
            entryList.push_back(inputs[entry]);
        }

        entryList.sort(EntryCompare);

        FwIndexType entryIndex = 0;

        for (const auto& entry : entryList) {
            outputs[entryIndex].record.set_priority(entry.record.get_priority());
            outputs[entryIndex].record.set_id(entry.record.get_id());
            outputs[entryIndex].record.set_state(entry.record.get_state());
            outputs[entryIndex].record.set_tSec(1000);
            outputs[entryIndex].record.set_tSub(1500);
            outputs[entryIndex].record.set_size(100);
            entryIndex++;
        }

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

        std::list<Svc::DpCatalog::DpStateEntry> entryList;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(100);
            inputs[entry].record.set_id(entry);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            inputs[entry].record.set_tSec(randVal);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
            entryList.push_back(inputs[entry]);
        }

        entryList.sort(EntryCompare);

        FwIndexType entryIndex = 0;

        for (const auto& entry : entryList) {
            outputs[entryIndex].record.set_priority(entry.record.get_priority());
            outputs[entryIndex].record.set_id(entry.record.get_id());
            outputs[entryIndex].record.set_state(entry.record.get_state());
            outputs[entryIndex].record.set_tSec(entry.record.get_tSec());
            outputs[entryIndex].record.set_tSub(1500);
            outputs[entryIndex].record.set_size(100);
            entryIndex++;
        }

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

        std::list<Svc::DpCatalog::DpStateEntry> entryList;

        // fill the input entries with random priorities
        for (FwIndexType entry = 0; entry < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(inputs)); entry++) {
            U32 randVal = STest::Pick::lowerUpper(0, NUM_ENTRIES - 1);
            inputs[entry].record.set_priority(100);
            inputs[entry].record.set_id(randVal);
            inputs[entry].record.set_state(Fw::DpState::UNTRANSMITTED);
            inputs[entry].record.set_tSec(1000);
            inputs[entry].record.set_tSub(1500);
            inputs[entry].record.set_size(100);
            entryList.push_back(inputs[entry]);
        }

        entryList.sort(EntryCompare);

        FwIndexType entryIndex = 0;

        for (const auto& entry : entryList) {
            outputs[entryIndex].record.set_priority(entry.record.get_priority());
            outputs[entryIndex].record.set_id(entry.record.get_id());
            outputs[entryIndex].record.set_state(entry.record.get_state());
            outputs[entryIndex].record.set_tSec(entry.record.get_tSec());
            outputs[entryIndex].record.set_tSub(1500);
            outputs[entryIndex].record.set_size(100);
            entryIndex++;
        }

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

        std::list<Svc::DpCatalog::DpStateEntry> entryList;

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
            entryList.push_back(inputs[entry]);
        }

        entryList.sort(EntryCompare);

        FwIndexType entryIndex = 0;

        for (const auto& entry : entryList) {
            outputs[entryIndex].record.set_priority(entry.record.get_priority());
            outputs[entryIndex].record.set_id(entry.record.get_id());
            outputs[entryIndex].record.set_state(entry.record.get_state());
            outputs[entryIndex].record.set_tSec(entry.record.get_tSec());
            outputs[entryIndex].record.set_tSub(1500);
            outputs[entryIndex].record.set_size(100);
            entryIndex++;
        }

        tester.testTree(inputs, outputs, FW_NUM_ARRAY_ELEMENTS(inputs));
    }
}

}  // namespace Svc
