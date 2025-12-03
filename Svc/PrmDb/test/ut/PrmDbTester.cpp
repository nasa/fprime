/*
 * PrmDbTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <gtest/gtest.h>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <Os/Delegate.hpp>
#include <Os/Stub/test/File.hpp>
#include <Svc/PrmDb/test/ut/PrmDbTester.hpp>
#include <cstdio>
#include "Os/Stub/Directory.hpp"
#include "Os/Stub/FileSystem.hpp"

namespace Svc {

typedef PrmDb_PrmWriteError PrmWriteError;
typedef PrmDb_PrmReadError PrmReadError;

void PrmDbTester::runNominalPopulate() {
    // clear database
    this->m_impl.clearDb(PrmDbType::DB_ACTIVE);

    // build a test parameter value with a simple value
    U32 val = 0x10;
    FwPrmIdType id = 0x21;

    Fw::ParamBuffer pBuff;

    Fw::SerializeStatus stat = pBuff.serializeFrom(val);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

    // clear all events
    this->clearEvents();
    // send it to the database
    static bool pdb003 = false;
    if (not pdb003) {
        REQUIREMENT("PDB-003");
        pdb003 = true;
    }

    this->invoke_to_setPrm(0, id, pBuff);
    // dispatch message
    this->m_impl.doDispatch();
    // Verify event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded(0, id);

    // retrieve it
    U32 testVal;

    static bool pdb002 = false;
    if (not pdb002) {
        REQUIREMENT("PDB-002");
        pdb002 = true;
    }

    this->invoke_to_getPrm(0, id, pBuff);
    // deserialize it
    stat = pBuff.deserializeTo(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal, val);

    // update the value
    val = 0x15;
    pBuff.resetSer();

    stat = pBuff.serializeFrom(val);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

    // clear all events
    this->clearEvents();
    // send it to the database
    this->invoke_to_setPrm(0, id, pBuff);
    // dispatch message
    this->m_impl.doDispatch();

    // retrieve it
    pBuff.resetSer();
    testVal = 0;
    this->invoke_to_getPrm(0, id, pBuff);
    // deserialize it
    stat = pBuff.deserializeTo(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal, val);

    // Verify event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdUpdated_SIZE(1);
    ASSERT_EVENTS_PrmIdUpdated(0, id);

    // add a new entry
    id = 0x25;
    val = 0x30;
    pBuff.resetSer();
    stat = pBuff.serializeFrom(val);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

    // clear all events
    this->clearEvents();
    // send it to the database
    this->invoke_to_setPrm(0, id, pBuff);
    // dispatch message
    this->m_impl.doDispatch();

    // Verify event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded(0, id);
}

void PrmDbTester::runNominalSaveFile() {
    Os::Stub::File::Test::StaticData::setWriteResult(m_io_data, sizeof m_io_data);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    // fill with data
    this->runNominalPopulate();
    // save the data
    this->clearEvents();
    this->clearHistory();

    static bool pdb004 = false;
    if (not pdb004) {
        REQUIREMENT("PDB-004");
        pdb004 = true;
    }

    this->sendCmd_PRM_SAVE_FILE(0, 12);
    Fw::QueuedComponentBase::MsgDispatchStatus stat = this->m_impl.doDispatch();
    EXPECT_EQ(stat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 12, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmFileSaveComplete_SIZE(1);
    ASSERT_EVENTS_PrmFileSaveComplete(0, 2);
}

void PrmDbTester::runNominalLoadFile() {
    // Preconditions to populate the write file
    this->runNominalSaveFile();

    Os::Stub::File::Test::StaticData::setReadResult(m_io_data, Os::Stub::File::Test::StaticData::data.pointer);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);

    // save the data
    this->clearEvents();

    static bool pdb001 = false;
    if (not pdb001) {
        REQUIREMENT("PDB-001");
        pdb001 = true;
    }

    this->m_impl.readParamFile();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmFileLoadComplete_SIZE(1);
    ASSERT_EVENTS_PrmFileLoadComplete(0, "ACTIVE", 2, 2, 0);

    // verify values (populated by runNominalPopulate())

    // first
    Fw::ParamBuffer pBuff;
    U32 testVal;
    this->invoke_to_getPrm(0, 0x21, pBuff);
    // deserialize it
    Fw::SerializeStatus stat = pBuff.deserializeTo(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal, 0x15u);

    // second
    pBuff.resetSer();
    this->invoke_to_getPrm(0, 0x25, pBuff);
    // deserialize it
    stat = pBuff.deserializeTo(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal, 0x30u);
}

void PrmDbTester::runMissingExtraParams() {
    // load up database
    this->runNominalPopulate();
    // ask for ID that isn't present
    this->clearEvents();
    Fw::ParamBuffer pBuff;
    EXPECT_EQ(Fw::ParamValid::INVALID, this->invoke_to_getPrm(0, 0x1000, pBuff).e);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdNotFound_SIZE(1);
    ASSERT_EVENTS_PrmIdNotFound(0, 0x1000);

    // clear database
    this->m_impl.clearDb(PrmDbType::DB_ACTIVE);

    this->clearEvents();
    // write too many entries
    for (FwPrmIdType entry = 0; entry <= PRMDB_NUM_DB_ENTRIES; entry++) {
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, pBuff.serializeFrom(static_cast<U32>(10)));
        this->invoke_to_setPrm(0, entry, pBuff);
        // dispatch message
        this->m_impl.doDispatch();
    }

    ASSERT_EVENTS_SIZE(PRMDB_NUM_DB_ENTRIES + 1);
    ASSERT_EVENTS_PrmIdAdded_SIZE(PRMDB_NUM_DB_ENTRIES);
    ASSERT_EVENTS_PrmDbFull_SIZE(1);
    ASSERT_EVENTS_PrmDbFull(0, PRMDB_NUM_DB_ENTRIES);
}

void PrmDbTester::runRefPrmFile() {
    {
        // ID = 00x1000
        U32 val = 14;
        FwPrmIdType id = 0x1000;

        Fw::ParamBuffer pBuff;

        Fw::SerializeStatus stat = pBuff.serializeFrom(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        this->invoke_to_setPrm(0, id, pBuff);
        // dispatch message
        this->m_impl.doDispatch();
        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded(0, id);

        // retrieve it
        U32 testVal;
        this->invoke_to_getPrm(0, id, pBuff);
        // deserialize it
        stat = pBuff.deserializeTo(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
        EXPECT_EQ(testVal, val);
    }

    {
        // ID = 0x1001
        I16 val = 15;
        FwPrmIdType id = 0x1001;

        Fw::ParamBuffer pBuff;

        Fw::SerializeStatus stat = pBuff.serializeFrom(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        this->invoke_to_setPrm(0, id, pBuff);
        // dispatch message
        this->m_impl.doDispatch();
        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded(0, id);

        // retrieve it
        I16 testVal;
        this->invoke_to_getPrm(0, id, pBuff);
        // deserialize it
        stat = pBuff.deserializeTo(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
        EXPECT_EQ(testVal, val);
    }

    {
        // ID = 0x1100
        U8 val = 32;
        FwPrmIdType id = 0x1100;

        Fw::ParamBuffer pBuff;

        Fw::SerializeStatus stat = pBuff.serializeFrom(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        this->invoke_to_setPrm(0, id, pBuff);
        // dispatch message
        this->m_impl.doDispatch();
        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded(0, id);

        // retrieve it
        U8 testVal;
        this->invoke_to_getPrm(0, id, pBuff);
        // deserialize it
        stat = pBuff.deserializeTo(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
        EXPECT_EQ(testVal, val);
    }

    {
        // ID = 0x1101
        F32 val = 33.34;
        FwPrmIdType id = 0x1101;

        Fw::ParamBuffer pBuff;

        Fw::SerializeStatus stat = pBuff.serializeFrom(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        this->invoke_to_setPrm(0, id, pBuff);
        // dispatch message
        this->m_impl.doDispatch();
        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded(0, id);

        // retrieve it
        F32 testVal;
        this->invoke_to_getPrm(0, id, pBuff);
        // deserialize it
        stat = pBuff.deserializeTo(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
        EXPECT_EQ(testVal, val);
    }

    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_SAVE_FILE(0, 12);
    Fw::QueuedComponentBase::MsgDispatchStatus mstat = this->m_impl.doDispatch();
    EXPECT_EQ(mstat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 12, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmFileSaveComplete_SIZE(1);
    ASSERT_EVENTS_PrmFileSaveComplete(0, 4);
}

void PrmDbTester::runFileReadError() {
    // Preconditions setup and test
    this->runNominalLoadFile();

    this->clearEvents();
    // Loop through all size errors testing each
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    this->m_errorType = FILE_SIZE_ERROR;
    for (FwSizeType i = 0; i < 4; i++) {
        clearEvents();
        this->m_waits = i;
        this->m_impl.readParamFile();
        ASSERT_EVENTS_SIZE(1);
        switch (i) {
            case 0:
                ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::DELIMITER_SIZE, 0, sizeof(U8) + 1);
                break;
            case 1:
                ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::RECORD_SIZE_SIZE, 0, sizeof(U32) + 1);
                break;
            case 2:
                ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::PARAMETER_ID_SIZE, 0, sizeof(FwPrmIdType) + 1);
                break;
            case 3:
                ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::PARAMETER_VALUE_SIZE, 0, sizeof(U32) + 1);
                break;
            default:
                FAIL() << "Reached unknown case";
        }
    }
    // Loop through failure statuses
    for (FwSizeType i = 0; i < 2; i++) {
        this->m_errorType = FILE_STATUS_ERROR;
        // Set various file errors
        switch (i) {
            case 0:
                this->m_status = Os::File::Status::DOESNT_EXIST;
                break;
            case 1:
                this->m_status = Os::File::Status::NOT_OPENED;
                break;
            default:
                FAIL() << "Reached unknown case";
        }
        // Loop through various field reads
        for (FwSizeType j = 0; j < 4; j++) {
            clearEvents();
            this->m_waits = j;
            this->m_impl.readParamFile();
            ASSERT_EVENTS_SIZE(1);
            switch (j) {
                case 0:
                    ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                    ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::DELIMITER, 0, this->m_status);
                    break;
                case 1:
                    ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                    ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::RECORD_SIZE, 0, this->m_status);
                    break;
                case 2:
                    ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                    ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::PARAMETER_ID, 0, this->m_status);
                    break;
                case 3:
                    ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                    ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::PARAMETER_VALUE, 0, this->m_status);
                    break;
                default:
                    FAIL() << "Reached unknown case";
            }
        }
    }
    this->m_errorType = FILE_DATA_ERROR;
    for (FwSizeType i = 0; i < 2; i++) {
        clearEvents();
        this->m_waits = i;
        this->m_impl.readParamFile();
        ASSERT_EVENTS_SIZE(1);
        switch (i) {
            case 0:
                ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                // Parameter read error caused by adding one to the expected read
                ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::DELIMITER_VALUE, 0, PRMDB_ENTRY_DELIMITER + 1);
                break;
            case 1: {
                // Data in this test is corrupted by adding 1 to the first data byte read. Since data is stored in
                // big-endian format the highest order byte of the record size (U32) must have one added to it.
                // Expected result of '8' inherited from original design of test.
                U32 expected_error_value = sizeof(FwPrmIdType) + 4 + (1 << ((sizeof(U32) - 1) * 8));
                ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::RECORD_SIZE_VALUE, 0, expected_error_value);
                break;
            }
            default:
                FAIL() << "Reached unknown case";
        }
    }
}

void PrmDbTester::runFileWriteError() {
    // File open error
    this->clearEvents();
    // register interceptor
    Os::Stub::File::Test::StaticData::setWriteResult(m_io_data, sizeof m_io_data);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::DOESNT_EXIST);
    // dispatch command
    this->sendCmd_PRM_SAVE_FILE(0, 12);
    Fw::QueuedComponentBase::MsgDispatchStatus stat = this->m_impl.doDispatch();
    ASSERT_EQ(stat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // check for failed event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
    ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::OPEN, 0, Os::File::DOESNT_EXIST);

    // check command status
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 12, Fw::CmdResponse::EXECUTION_ERROR);

    this->runNominalPopulate();

    // Loop through all size errors testing each
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    this->m_errorType = FILE_SIZE_ERROR;
    for (FwSizeType i = 0; i < 4; i++) {
        clearEvents();
        this->clearHistory();
        this->m_waits = i;
        this->sendCmd_PRM_SAVE_FILE(0, 12);
        stat = this->m_impl.doDispatch();
        ASSERT_EQ(stat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        ASSERT_EVENTS_SIZE(1);
        switch (i) {
            case 0:
                ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::DELIMITER_SIZE, 0, sizeof(U8) + 1);
                break;
            case 1:
                ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::RECORD_SIZE_SIZE, 0, sizeof(U32) + 1);
                break;
            case 2:
                ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::PARAMETER_ID_SIZE, 0, sizeof(FwPrmIdType) + 1);
                break;
            case 3:
                ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::PARAMETER_VALUE_SIZE, 0, sizeof(U32) + 1);
                break;
            default:
                FAIL() << "Reached unknown case";
        }
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 12, Fw::CmdResponse::EXECUTION_ERROR);
    }

    // Loop through failure statuses
    for (FwSizeType i = 0; i < 2; i++) {
        this->m_errorType = FILE_STATUS_ERROR;
        // Set various file errors
        switch (i) {
            case 0:
                this->m_status = Os::File::Status::DOESNT_EXIST;
                break;
            case 1:
                this->m_status = Os::File::Status::NOT_OPENED;
                break;
            default:
                FAIL() << "Reached unknown case";
        }
        // Loop through various field reads
        for (FwSizeType j = 0; j < 4; j++) {
            clearEvents();
            this->clearHistory();
            this->m_waits = j;
            this->sendCmd_PRM_SAVE_FILE(0, 12);
            stat = this->m_impl.doDispatch();
            ASSERT_EQ(stat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);
            ASSERT_EVENTS_SIZE(1);
            switch (j) {
                case 0:
                    ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                    ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::DELIMITER, 0, this->m_status);
                    break;
                case 1:
                    ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                    ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::RECORD_SIZE, 0, this->m_status);
                    break;
                case 2:
                    ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                    ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::PARAMETER_ID, 0, this->m_status);
                    break;
                case 3:
                    ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
                    ASSERT_EVENTS_PrmFileWriteError(0, PrmWriteError::PARAMETER_VALUE, 0, this->m_status);
                    break;
                default:
                    FAIL() << "Reached unknown case";
            }
            ASSERT_CMD_RESPONSE_SIZE(1);
            ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 12, Fw::CmdResponse::EXECUTION_ERROR);
        }
    }
}

void PrmDbTester::runDbEqualTest() {
    Fw::SerializeStatus serStat;

    // 1. Test with empty databases - should be equal
    this->m_impl.clearDb(PrmDb_PrmDbType::DB_ACTIVE);
    this->m_impl.clearDb(PrmDb_PrmDbType::DB_STAGING);
    EXPECT_TRUE(this->m_impl.dbEqual());

    // 2. Add an entry to active DB only - should not be equal
    U32 val1 = 0x42;
    FwPrmIdType id1 = 0x100;
    Fw::ParamBuffer pBuff;

    serStat = pBuff.serializeFrom(val1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);

    this->m_impl.updateAddPrmImpl(id1, pBuff, PrmDb_PrmDbType::DB_ACTIVE);
    EXPECT_FALSE(this->m_impl.dbEqual());

    // 3. Add same entry to staging DB - should be equal again
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);

    this->m_impl.updateAddPrmImpl(id1, pBuff, PrmDb_PrmDbType::DB_STAGING);
    EXPECT_TRUE(this->m_impl.dbEqual());

    // 4. Update entry in active DB only - should not be equal
    U32 val2 = 0x43;
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);

    this->m_impl.updateAddPrmImpl(id1, pBuff, PrmDb_PrmDbType::DB_STAGING);
    EXPECT_FALSE(this->m_impl.dbEqual());

    // 5. Update staging DB to match - should be equal again
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);

    this->m_impl.updateAddPrmImpl(id1, pBuff, PrmDb_PrmDbType::DB_ACTIVE);
    EXPECT_TRUE(this->m_impl.dbEqual());

    // 6. Add different entry to staging DB - should not be equal
    U32 val3 = 0x44;
    FwPrmIdType id2 = 0x101;
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val3);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);

    this->m_impl.updateAddPrmImpl(id2, pBuff, PrmDb_PrmDbType::DB_STAGING);
    EXPECT_FALSE(this->m_impl.dbEqual());
}

void PrmDbTester::runDbCopyTest() {
    Fw::SerializeStatus serStat;

    // Clear both databases
    this->m_impl.clearDb(PrmDb_PrmDbType::DB_ACTIVE);
    this->m_impl.clearDb(PrmDb_PrmDbType::DB_STAGING);

    // Add entries to active DB only
    U32 val1 = 0x1234;
    FwPrmIdType id1 = 0x100;
    Fw::ParamBuffer pBuff;

    // Add first parameter
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    this->m_impl.updateAddPrmImpl(id1, pBuff, PrmDb_PrmDbType::DB_ACTIVE);

    // Add second parameter
    F32 val2 = 3.14159f;
    FwPrmIdType id2 = 0x200;
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    this->m_impl.updateAddPrmImpl(id2, pBuff, PrmDb_PrmDbType::DB_ACTIVE);

    // Verify databases are not equal
    EXPECT_FALSE(this->m_impl.dbEqual());

    // Copy active DB to staging DB
    this->m_impl.dbCopy(PrmDb_PrmDbType::DB_STAGING, PrmDb_PrmDbType::DB_ACTIVE);

    // Verify databases are now equal
    EXPECT_TRUE(this->m_impl.dbEqual());

    // Verify values in the staging DB
    pBuff.resetSer();
    U32 testVal1;
    FwSizeType idx = 0;
    // Find the parameter and get its index
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        if (this->m_impl.m_activeDb[i].used && this->m_impl.m_stagingDb[i].id == id1) {
            idx = i;
            break;
        }
    }
    EXPECT_TRUE(this->m_impl.m_activeDb[idx].used);
    EXPECT_EQ(id1, this->m_impl.m_stagingDb[idx].id);
    pBuff = this->m_impl.m_stagingDb[idx].val;
    serStat = pBuff.deserializeTo(testVal1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    EXPECT_EQ(val1, testVal1);

    // Clear both databases
    this->m_impl.clearDb(PrmDb_PrmDbType::DB_ACTIVE);
    this->m_impl.clearDb(PrmDb_PrmDbType::DB_STAGING);

    // Add different entries to active and staging DBs

    // active DB - add first parameter
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    this->m_impl.updateAddPrmImpl(id1, pBuff, PrmDb_PrmDbType::DB_ACTIVE);

    // active DB - add second parameter
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    this->m_impl.updateAddPrmImpl(id2, pBuff, PrmDb_PrmDbType::DB_ACTIVE);

    // staging DB - add different parameter
    U16 val3 = 0x5678;
    FwPrmIdType id3 = 0x300;
    pBuff.resetSer();
    serStat = pBuff.serializeFrom(val3);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    this->m_impl.updateAddPrmImpl(id3, pBuff, PrmDb_PrmDbType::DB_STAGING);

    // Verify databases are not equal
    EXPECT_FALSE(this->m_impl.dbEqual());

    // Copy only the second entry from active to staging at the same index
    FwSizeType activeIdx2 = 1;  // Index of second entry in active DB
    this->m_impl.dbCopySingle(PrmDb_PrmDbType::DB_STAGING, PrmDb_PrmDbType::DB_ACTIVE, activeIdx2);

    // Verify the specific entry was copied correctly
    EXPECT_TRUE(this->m_impl.m_stagingDb[activeIdx2].used);
    EXPECT_EQ(id2, this->m_impl.m_stagingDb[activeIdx2].id);

    // Verify value matches
    pBuff = this->m_impl.m_stagingDb[activeIdx2].val;
    F32 testVal2;
    serStat = pBuff.deserializeTo(testVal2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
    EXPECT_EQ(val2, testVal2);

    // Verify the original entry in staging DB is still there
    bool foundOriginal = false;
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        if (this->m_impl.m_stagingDb[i].used && this->m_impl.m_stagingDb[i].id == id3) {
            foundOriginal = true;

            // Verify value is still correct
            pBuff = this->m_impl.m_stagingDb[i].val;
            U16 testVal3;
            serStat = pBuff.deserializeTo(testVal3);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK, serStat);
            EXPECT_EQ(val3, testVal3);
            break;
        }
    }
    EXPECT_TRUE(foundOriginal);

    // Databases should still not be equal since we only copied one entry
    EXPECT_FALSE(this->m_impl.dbEqual());
}

void PrmDbTester::runDbCommitTest() {
    // 1. Set the m_state to FILE_UPDATES_STAGED
    this->m_impl.m_state = PrmDbFileLoadState::FILE_UPDATES_STAGED;

    // 2. Populate both databases with different content
    // Clear both databases first
    this->m_impl.clearDb(PrmDbType::DB_ACTIVE);
    this->m_impl.clearDb(PrmDbType::DB_STAGING);

    // Add parameters to active database
    U32 activeVal1 = 0x1234;
    FwPrmIdType activeId1 = 0x100;
    Fw::ParamBuffer pBuff;

    Fw::SerializeStatus stat = pBuff.serializeFrom(activeVal1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(activeId1, pBuff, PrmDbType::DB_ACTIVE);

    // Add different parameters to staging database
    U32 stagingVal1 = 0x5678;
    FwPrmIdType stagingId1 = 0x100;  // Same ID but different value
    pBuff.resetSer();
    stat = pBuff.serializeFrom(stagingVal1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(stagingId1, pBuff, PrmDbType::DB_STAGING);

    // Add a parameter that's only in the staging database
    F32 stagingVal2 = 3.14159f;
    FwPrmIdType stagingId2 = 0x200;
    pBuff.resetSer();
    stat = pBuff.serializeFrom(stagingVal2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(stagingId2, pBuff, PrmDbType::DB_STAGING);

    // Store pointers to databases before swap for verification
    PrmDbImpl::t_dbStruct* preSwapActiveDb = this->m_impl.m_activeDb;
    PrmDbImpl::t_dbStruct* preSwapStagingDb = this->m_impl.m_stagingDb;

    // Clear events and command history
    this->clearEvents();
    this->clearHistory();

    // 3. Execute the commit command
    this->sendCmd_PRM_COMMIT_STAGED(0, 10);
    Fw::QueuedComponentBase::MsgDispatchStatus dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // 4. Verify results

    // Check command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_COMMIT_STAGED, 10, Fw::CmdResponse::OK);

    // Check event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbCommitComplete_SIZE(1);

    // Verify the state is now IDLE
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::IDLE);

    // Verify that the database pointers have been swapped
    EXPECT_EQ(this->m_impl.m_activeDb, preSwapStagingDb);
    EXPECT_EQ(this->m_impl.m_stagingDb, preSwapActiveDb);

    // Verify that the new staging database is empty
    bool allEntriesCleared = true;
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        if (this->m_impl.m_stagingDb[i].used) {
            allEntriesCleared = false;
            break;
        }
    }
    EXPECT_TRUE(allEntriesCleared);

    // Verify that parameters can be accessed from the newly active database
    // (which was formerly the staging database)
    pBuff.resetSer();
    U32 retrievedVal1;
    this->invoke_to_getPrm(0, stagingId1, pBuff);
    stat = pBuff.deserializeTo(retrievedVal1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(retrievedVal1, stagingVal1);

    pBuff.resetSer();
    F32 retrievedVal2;
    this->invoke_to_getPrm(0, stagingId2, pBuff);
    stat = pBuff.deserializeTo(retrievedVal2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(retrievedVal2, stagingVal2);

    // Test invalid state handling - try to commit again when not in FILE_UPDATES_STAGED state
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_COMMIT_STAGED(0, 11);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Should get validation error and warning event
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_COMMIT_STAGED, 11, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
}

void PrmDbTester::runPrmFileLoadNominal() {
    Fw::String file = "TestFile.prm";
    Fw::QueuedComponentBase::MsgDispatchStatus dispatchStatus;

    // Store pointers to databases before swap for verification
    PrmDbImpl::t_dbStruct* preSwapActiveDb = this->m_impl.m_activeDb;
    PrmDbImpl::t_dbStruct* preSwapStagingDb = this->m_impl.m_stagingDb;

    // Ensure we're in IDLE state
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::IDLE);

    // Populate the active DB and save to file
    runNominalSaveFile();
    printf("Saved into File: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    // Clear both databases
    this->m_impl.clearDb(PrmDbType::DB_ACTIVE);
    this->m_impl.clearDb(PrmDbType::DB_STAGING);

    printf("Cleared: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    // Populate active database with some values so we can test merge=true
    // A new ID
    U32 activeVal1 = 0x1234;
    FwPrmIdType activeId1 = 0x100;
    Fw::ParamBuffer pBuff;

    // Add parameter to active database
    Fw::SerializeStatus stat = pBuff.serializeFrom(activeVal1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(activeId1, pBuff, PrmDbType::DB_ACTIVE);
    // Verify parameter is in active database
    pBuff.resetSer();
    U32 testVal;
    this->invoke_to_getPrm(0, activeId1, pBuff);
    stat = pBuff.deserializeTo(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal, activeVal1);
    // A existing (in file) ID
    U32 activeVal2Original = 0x30;
    U32 activeVal2Update = activeVal2Original + 1;
    FwPrmIdType activeId2 = 0x25;  // This ID exists in the file with a different value
    pBuff.resetSer();

    // Add parameter to active database
    stat = pBuff.serializeFrom(activeVal2Update);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(activeId2, pBuff, PrmDbType::DB_ACTIVE);
    // Verify parameter is in active database
    pBuff.resetSer();
    U32 testVal2;
    this->invoke_to_getPrm(0, activeId2, pBuff);
    stat = pBuff.deserializeTo(testVal2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal2, activeVal2Update);

    printf("Added new: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    // Send PRM_LOAD_FILE command with merge=true to merge with active database
    Os::Stub::File::Test::StaticData::setReadResult(m_io_data, Os::Stub::File::Test::StaticData::data.pointer);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_LOAD_FILE(0, 10, file, PrmDb_Merge::MERGE);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    ASSERT_EVENTS_SIZE(2);
    // Verify EVRs for copy (because we are merging)
    ASSERT_EVENTS_PrmDbCopyAllComplete_SIZE(1);
    ASSERT_EVENTS_PrmDbCopyAllComplete(0, "ACTIVE", "STAGING");
    // Verify EVRs for the file load
    ASSERT_EVENTS_PrmFileLoadComplete_SIZE(1);
    ASSERT_EVENTS_PrmFileLoadComplete(0, "STAGING", 2, 1, 1);

    printf("Parameter Load file complete: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    //  Verify state and command response after PRM_LOAD_FILE
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::FILE_UPDATES_STAGED);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_LOAD_FILE, 10, Fw::CmdResponse::OK);

    // Verify that parameters in staging database have expected values
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        // Check for the parameter that we added after the save file (since we are merging)
        if (this->m_impl.m_stagingDb[i].used && this->m_impl.m_stagingDb[i].id == activeId1) {
            pBuff = this->m_impl.m_stagingDb[i].val;
            U32 checkVal;
            stat = pBuff.deserializeTo(checkVal);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
            EXPECT_EQ(checkVal, activeVal1);
        }
        // Check for the parameter that we updated after the save file (since we are merging)
        if (this->m_impl.m_stagingDb[i].used && this->m_impl.m_stagingDb[i].id == activeId2) {
            pBuff = this->m_impl.m_stagingDb[i].val;
            U32 checkVal;
            stat = pBuff.deserializeTo(checkVal);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
            EXPECT_EQ(checkVal, activeVal2Original);  // Value should match what was in the file, not what we set
        }
    }

    // Send PRM_COMMIT_STAGED command
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_COMMIT_STAGED(0, 11);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify state and command response after PRM_COMMIT_STAGED
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_COMMIT_STAGED, 11, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbCommitComplete_SIZE(1);

    // Verify the database pointers have been swapped
    EXPECT_EQ(this->m_impl.m_activeDb, preSwapStagingDb);
    EXPECT_EQ(this->m_impl.m_stagingDb, preSwapActiveDb);

    // Verify the new staging database (former active) is empty
    bool allEntriesCleared = true;
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        if (this->m_impl.m_stagingDb[i].used) {
            allEntriesCleared = false;
            break;
        }
    }
    EXPECT_TRUE(allEntriesCleared);

    // Verify we can now perform operations only allowed in IDLE state
    // Try setting a parameter - should work in IDLE state
    U32 newVal3 = 0x9ABC;
    FwPrmIdType newId3 = 0x300;
    pBuff.resetSer();
    stat = pBuff.serializeFrom(newVal3);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

    this->clearEvents();
    this->invoke_to_setPrm(0, newId3, pBuff);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify parameter was added
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded_SIZE(1);

    // Verify we can retrieve the newly set parameter
    pBuff.resetSer();
    U32 retrievedVal3;
    Fw::ParamValid valid = this->invoke_to_getPrm(0, newId3, pBuff);
    EXPECT_EQ(Fw::ParamValid::VALID, valid);
    stat = pBuff.deserializeTo(retrievedVal3);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(retrievedVal3, newVal3);
}

void PrmDbTester::runPrmFileLoadWithErrors() {
    Fw::String file = "TestFile.prm";
    Fw::QueuedComponentBase::MsgDispatchStatus dispatchStatus;

    // Store pointers to databases before swap for verification
    PrmDbImpl::t_dbStruct* preSwapActiveDb = this->m_impl.m_activeDb;
    PrmDbImpl::t_dbStruct* preSwapStagingDb = this->m_impl.m_stagingDb;

    // Ensure we're in IDLE state
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::IDLE);

    // Populate the active DB and save to file
    runNominalSaveFile();
    printf("Saved into File: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    // Clear both databases
    this->m_impl.clearDb(PrmDbType::DB_ACTIVE);
    this->m_impl.clearDb(PrmDbType::DB_STAGING);

    printf("Cleared: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    // Populate active database with some values so we can test merge=true
    // A new ID
    U32 activeVal1 = 0x1234;
    FwPrmIdType activeId1 = 0x100;
    Fw::ParamBuffer pBuff;

    // Add parameter to active database
    Fw::SerializeStatus stat = pBuff.serializeFrom(activeVal1);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(activeId1, pBuff, PrmDbType::DB_ACTIVE);
    // Verify parameter is in active database
    pBuff.resetSer();
    U32 testVal;
    this->invoke_to_getPrm(0, activeId1, pBuff);
    stat = pBuff.deserializeTo(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal, activeVal1);
    // A existing (in file) ID
    U32 activeVal2Original = 0x30;
    U32 activeVal2Update = activeVal2Original + 1;
    FwPrmIdType activeId2 = 0x25;  // This ID exists in the file with a different value
    pBuff.resetSer();

    // Add parameter to active database
    stat = pBuff.serializeFrom(activeVal2Update);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    this->m_impl.updateAddPrmImpl(activeId2, pBuff, PrmDbType::DB_ACTIVE);
    // Verify parameter is in active database
    pBuff.resetSer();
    U32 testVal2;
    this->invoke_to_getPrm(0, activeId2, pBuff);
    stat = pBuff.deserializeTo(testVal2);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);
    EXPECT_EQ(testVal2, activeVal2Update);

    printf("Added new: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    // Send PRM_LOAD_FILE command with merge=true to merge with active database
    // but with a file open error
    Os::Stub::File::Test::StaticData::setReadResult(m_io_data, Os::Stub::File::Test::StaticData::data.pointer);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::DOESNT_EXIST);
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_LOAD_FILE(0, 10, file, PrmDb_Merge::MERGE);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    ASSERT_EVENTS_SIZE(3);
    // Verify EVRs for copy (because we are merging)
    ASSERT_EVENTS_PrmDbCopyAllComplete_SIZE(1);
    // Verify EVRs for file load read error
    ASSERT_EVENTS_PrmFileReadError_SIZE(1);
    // Verify EVRs for the file load cmd failure
    ASSERT_EVENTS_PrmDbFileLoadFailed_SIZE(1);

    printf("Parameter Load file complete: \n");
    printDb(PrmDbType::DB_ACTIVE);
    printDb(PrmDbType::DB_STAGING);

    //  Verify state and command response after PRM_LOAD_FILE
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::IDLE);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_LOAD_FILE, 10, Fw::CmdResponse::EXECUTION_ERROR);

    // Verify the database pointers have NOT been swapped
    EXPECT_EQ(this->m_impl.m_activeDb, preSwapActiveDb);
    EXPECT_EQ(this->m_impl.m_stagingDb, preSwapStagingDb);

    // Verify the staging database is empty
    bool allEntriesCleared = true;
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        if (this->m_impl.m_stagingDb[i].used) {
            allEntriesCleared = false;
            break;
        }
    }
    EXPECT_TRUE(allEntriesCleared);
}

void PrmDbTester::runPrmFileLoadIllegal() {
    Fw::QueuedComponentBase::MsgDispatchStatus dispatchStatus;
    Fw::ParamBuffer pBuff;
    U32 testVal = 0x1234;
    FwPrmIdType testId = 0x42;

    // Serialize test value for parameter setting
    Fw::SerializeStatus stat = pBuff.serializeFrom(testVal);
    EXPECT_EQ(Fw::FW_SERIALIZE_OK, stat);

    // -------------------------------------------------------------------
    // 1. Test illegal operations in LOADING_FILE_UPDATES state
    // -------------------------------------------------------------------
    this->m_impl.m_state = PrmDbFileLoadState::LOADING_FILE_UPDATES;

    // 1.1 Attempt PRM_LOAD_FILE while already loading
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_LOAD_FILE(0, 10, Fw::String("file.prm"), PrmDb_Merge::MERGE);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_LOAD_FILE, 10, Fw::CmdResponse::BUSY);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::LOADING_FILE_UPDATES,
                                             PrmDb_PrmLoadAction::LOAD_FILE_COMMAND);

    // 1.2 Attempt PRM_SAVE_FILE during loading
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_SAVE_FILE(0, 11);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 11, Fw::CmdResponse::BUSY);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::LOADING_FILE_UPDATES,
                                             PrmDb_PrmLoadAction::SAVE_FILE_COMMAND);

    // 1.3 Attempt to set parameter during loading
    this->clearEvents();
    this->clearHistory();
    this->invoke_to_setPrm(0, testId, pBuff);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response (warning event only, no added event)
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::LOADING_FILE_UPDATES,
                                             PrmDb_PrmLoadAction::SET_PARAMETER);
    ASSERT_EVENTS_PrmIdAdded_SIZE(0);
    ASSERT_EVENTS_PrmIdUpdated_SIZE(0);

    // 1.4 Attempt PRM_COMMIT_STAGED during loading
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_COMMIT_STAGED(0, 12);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_COMMIT_STAGED, 12, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::LOADING_FILE_UPDATES,
                                             PrmDb_PrmLoadAction::COMMIT_STAGED_COMMAND);

    // Verify state hasn't changed
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::LOADING_FILE_UPDATES);

    // -------------------------------------------------------------------
    // 2. Test illegal operations in FILE_UPDATES_STAGED state
    // -------------------------------------------------------------------
    this->m_impl.m_state = PrmDbFileLoadState::FILE_UPDATES_STAGED;

    // 2.1 Attempt PRM_LOAD_FILE when updates are staged
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_LOAD_FILE(0, 13, Fw::String("file.prm"), PrmDb_Merge::RESET);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_LOAD_FILE, 13, Fw::CmdResponse::BUSY);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::FILE_UPDATES_STAGED,
                                             PrmDb_PrmLoadAction::LOAD_FILE_COMMAND);

    // 2.2 Attempt PRM_SAVE_FILE when updates are staged
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_SAVE_FILE(0, 14);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_SAVE_FILE, 14, Fw::CmdResponse::BUSY);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::FILE_UPDATES_STAGED,
                                             PrmDb_PrmLoadAction::SAVE_FILE_COMMAND);

    // 2.3 Attempt to set parameter when updates are staged
    this->clearEvents();
    this->clearHistory();
    this->invoke_to_setPrm(0, testId, pBuff);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response (warning event only, no added event)
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::FILE_UPDATES_STAGED,
                                             PrmDb_PrmLoadAction::SET_PARAMETER);
    ASSERT_EVENTS_PrmIdAdded_SIZE(0);
    ASSERT_EVENTS_PrmIdUpdated_SIZE(0);

    // Verify state hasn't changed
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::FILE_UPDATES_STAGED);

    // -------------------------------------------------------------------
    // 3. Test illegal operations in IDLE state
    // -------------------------------------------------------------------
    this->m_impl.m_state = PrmDbFileLoadState::IDLE;

    // 3.1 Attempt PRM_COMMIT_STAGED in IDLE state
    this->clearEvents();
    this->clearHistory();
    this->sendCmd_PRM_COMMIT_STAGED(0, 15);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify appropriate error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, PrmDbImpl::OPCODE_PRM_COMMIT_STAGED, 15, Fw::CmdResponse::VALIDATION_ERROR);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction_SIZE(1);
    ASSERT_EVENTS_PrmDbFileLoadInvalidAction(0, PrmDbFileLoadState::IDLE, PrmDb_PrmLoadAction::COMMIT_STAGED_COMMAND);

    // Verify state hasn't changed
    EXPECT_EQ(this->m_impl.m_state, PrmDbFileLoadState::IDLE);

    // 3.2 Verify legal operations are still allowed in IDLE state
    // Setting a parameter should work in IDLE state
    this->clearEvents();
    this->clearHistory();
    this->invoke_to_setPrm(0, testId, pBuff);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify parameter was added
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded_SIZE(1);

    // -------------------------------------------------------------------
    // 4. Test state transitions back to IDLE after errors
    // -------------------------------------------------------------------

    // 4.1 Test that failed file load sets state back to IDLE
    this->m_impl.m_state = PrmDbFileLoadState::LOADING_FILE_UPDATES;
    this->m_impl.clearDb(PrmDbType::DB_STAGING);

    // Simulate failed file load
    this->m_impl.m_state = PrmDbFileLoadState::IDLE;

    // Verify that setPrm now works
    this->clearEvents();
    this->clearHistory();
    this->invoke_to_setPrm(0, testId + 1, pBuff);
    dispatchStatus = this->m_impl.doDispatch();
    EXPECT_EQ(dispatchStatus, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

    // Verify parameter was added
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PrmIdAdded_SIZE(1);
}

PrmDbTester* PrmDbTester::PrmDbTestFile::s_tester = nullptr;

void PrmDbTester::PrmDbTestFile::setTester(Svc::PrmDbTester* tester) {
    ASSERT_NE(tester, nullptr);
    s_tester = tester;
}

Os::File::Status PrmDbTester::PrmDbTestFile::read(U8* buffer, FwSizeType& size, Os::File::WaitType wait) {
    EXPECT_NE(s_tester, nullptr);
    Os::File::Status status = this->Os::Stub::File::Test::TestFile::read(buffer, size, wait);
    if (s_tester->m_waits == 0) {
        switch (s_tester->m_errorType) {
            case FILE_STATUS_ERROR:
                status = s_tester->m_status;
                break;
            case FILE_SIZE_ERROR:
                size += 1;
                break;
            case FILE_DATA_ERROR:
                buffer[0] += 1;
                break;
            default:
                break;
        }
    } else {
        s_tester->m_waits -= 1;
    }
    return status;
}

Os::File::Status PrmDbTester::PrmDbTestFile::write(const U8* buffer, FwSizeType& size, Os::File::WaitType wait) {
    EXPECT_NE(s_tester, nullptr);
    Os::File::Status status = this->Os::Stub::File::Test::TestFile::write(buffer, size, wait);
    if (s_tester->m_waits == 0) {
        switch (s_tester->m_errorType) {
            case FILE_STATUS_ERROR:
                status = s_tester->m_status;
                break;
            case FILE_SIZE_ERROR:
                size += 1;
                break;
            default:
                break;
        }
    } else {
        s_tester->m_waits -= 1;
    }
    return status;
}

PrmDbTester::PrmDbTester(Svc::PrmDbImpl& inst) : PrmDbGTestBase("testerbase", 100), m_impl(inst) {
    PrmDbTester::PrmDbTestFile::setTester(this);
}

PrmDbTester::~PrmDbTester() {
    this->m_impl.deinit();
}

void PrmDbTester ::from_pingOut_handler(const FwIndexType portNum, U32 key) {
    this->pushFromPortEntry_pingOut(key);
}

void PrmDbTester::printDb(PrmDb_PrmDbType dbType) {
    PrmDbImpl::t_dbStruct* db = this->m_impl.getDbPtr(dbType);
    printf("%s Parameter DB @ %p \n", PrmDbImpl::getDbString(dbType).toChar(), static_cast<void*>(db));
    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        U8* data = db[entry].val.getBuffAddr();
        FwSizeType len = db[entry].val.getSize();
        if (db[entry].used) {
            std::cout << "  " << std::setw(2) << entry << " :";
            printf(" ID = %08X", db[entry].id);
            printf(" Value = ");
            for (FwSizeType i = 0; i < len; ++i) {
                printf("%02X ", data[i]);
            }
            printf("\n");
        }
    }
}

} /* namespace Svc */

namespace Os {

//! \brief get a delegate for FileInterface that intercepts calls for parameter database testing
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileInterface* FileInterface::getDelegate(FileHandleStorage& aligned_placement_new_memory,
                                          const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Svc::PrmDbTester::PrmDbTestFile>(aligned_placement_new_memory,
                                                                                      to_copy);
}

//! \brief get a delegate for FileSystemInterface that intercepts calls for stub fileSystem usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileSystemInterface* FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Stub::FileSystem::StubFileSystem>(
        aligned_placement_new_memory);
}

//! \brief get a delegate for DirectoryInterface that intercepts calls for stub Directory usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
DirectoryInterface* DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Stub::Directory::StubDirectory>(
        aligned_placement_new_memory);
}
}  // namespace Os
