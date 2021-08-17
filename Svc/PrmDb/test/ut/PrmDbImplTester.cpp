/*
 * PrmDbImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/PrmDb/test/ut/PrmDbImplTester.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Os/Stubs/FileStubs.hpp>

#include <cstdio>
#include <gtest/gtest.h>

#include <Fw/Test/UnitTest.hpp>

namespace Svc {

    void PrmDbImplTester::runNominalPopulate(void) {

        // clear database
        this->m_impl.clearDb();

        // build a test parameter value with a simple value
        U32 val = 0x10;
        FwPrmIdType id = 0x21;

        Fw::ParamBuffer pBuff;

        Fw::SerializeStatus stat = pBuff.serialize(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        static bool pdb003 = false;
        if (not pdb003) {
            REQUIREMENT("PDB-003");
            pdb003 = true;
        }

        this->invoke_to_setPrm(0,id,pBuff);
        // dispatch message
        this->m_impl.doDispatch();
        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded(0,id);

        // retrieve it
        U32 testVal;

        static bool pdb002 = false;
        if (not pdb002) {
            REQUIREMENT("PDB-002");
            pdb002 = true;
        }

        this->invoke_to_getPrm(0,id,pBuff);
        // deserialize it
        stat = pBuff.deserialize(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
        EXPECT_EQ(testVal,val);

        // update the value
        val = 0x15;
        pBuff.resetSer();

        stat = pBuff.serialize(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        this->invoke_to_setPrm(0,id,pBuff);
        // dispatch message
        this->m_impl.doDispatch();

        // retrieve it
        pBuff.resetSer();
        testVal = 0;
        this->invoke_to_getPrm(0,id,pBuff);
        // deserialize it
        stat = pBuff.deserialize(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
        EXPECT_EQ(testVal,val);

        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdUpdated_SIZE(1);
        ASSERT_EVENTS_PrmIdUpdated(0,id);

        // add a new entry
        id = 0x25;
        val = 0x30;
        pBuff.resetSer();
        stat = pBuff.serialize(val);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

        // clear all events
        this->clearEvents();
        // send it to the database
        this->invoke_to_setPrm(0,id,pBuff);
        // dispatch message
        this->m_impl.doDispatch();

        // Verify event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(1);
        ASSERT_EVENTS_PrmIdAdded(0,id);
    }

    void PrmDbImplTester::runNominalSaveFile(void) {
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

        this->sendCmd_PRM_SAVE_FILE(0,12);
        Fw::QueuedComponentBase::MsgDispatchStatus stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete(0,2);

    }

    void PrmDbImplTester::runNominalLoadFile(void) {
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
        ASSERT_EVENTS_PrmFileLoadComplete(0,2);

        // verify values (populated by runNominalPopulate())

        // first
        Fw::ParamBuffer pBuff;
        U32 testVal;
        this->invoke_to_getPrm(0,0x21,pBuff);
        // deserialize it
        Fw::SerializeStatus stat = pBuff.deserialize(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
        EXPECT_EQ(testVal,(U32)0x15);

        // second
        pBuff.resetSer();
        this->invoke_to_getPrm(0,0x25,pBuff);
        // deserialize it
        stat = pBuff.deserialize(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
        EXPECT_EQ(testVal,(U32)0x30);

    }

    void PrmDbImplTester::runMissingExtraParams(void) {

        // load up database
        this->runNominalPopulate();
        // ask for ID that isn't present
        this->clearEvents();
        Fw::ParamBuffer pBuff;
        EXPECT_EQ(Fw::PARAM_INVALID,this->invoke_to_getPrm(0,0x1000,pBuff));
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdNotFound_SIZE(1);
        ASSERT_EVENTS_PrmIdNotFound(0,0x1000);

        // clear database
        this->m_impl.clearDb();

        this->clearEvents();
        // write too many entries
        for (NATIVE_INT_TYPE entry = 0; entry <= PRMDB_NUM_DB_ENTRIES; entry++) {
            Fw::ParamBuffer pBuff;
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,pBuff.serialize((U32)10));
            this->invoke_to_setPrm(0,(FwPrmIdType)entry,pBuff);
            // dispatch message
            this->m_impl.doDispatch();
        }

        ASSERT_EVENTS_SIZE(PRMDB_NUM_DB_ENTRIES+1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(PRMDB_NUM_DB_ENTRIES);
        ASSERT_EVENTS_PrmDbFull_SIZE(1);
        ASSERT_EVENTS_PrmDbFull(0,PRMDB_NUM_DB_ENTRIES);

    }

    void PrmDbImplTester::runRefPrmFile(void) {

        {
            // ID = 00x1000
            U32 val = 14;
            FwPrmIdType id = 0x1000;

            Fw::ParamBuffer pBuff;

            Fw::SerializeStatus stat = pBuff.serialize(val);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

            // clear all events
            this->clearEvents();
            // send it to the database
            this->invoke_to_setPrm(0,id,pBuff);
            // dispatch message
            this->m_impl.doDispatch();
            // Verify event
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded(0,id);

            // retrieve it
            U32 testVal;
            this->invoke_to_getPrm(0,id,pBuff);
            // deserialize it
            stat = pBuff.deserialize(testVal);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
            EXPECT_EQ(testVal,val);
        }

        {
            // ID = 0x1001
            I16 val = 15;
            FwPrmIdType id = 0x1001;

            Fw::ParamBuffer pBuff;

            Fw::SerializeStatus stat = pBuff.serialize(val);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

            // clear all events
            this->clearEvents();
            // send it to the database
            this->invoke_to_setPrm(0,id,pBuff);
            // dispatch message
            this->m_impl.doDispatch();
            // Verify event
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded(0,id);

            // retrieve it
            I16 testVal;
            this->invoke_to_getPrm(0,id,pBuff);
            // deserialize it
            stat = pBuff.deserialize(testVal);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
            EXPECT_EQ(testVal,val);
        }

        {
            // ID = 0x1100
            U8 val = 32;
            FwPrmIdType id = 0x1100;

            Fw::ParamBuffer pBuff;

            Fw::SerializeStatus stat = pBuff.serialize(val);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

            // clear all events
            this->clearEvents();
            // send it to the database
            this->invoke_to_setPrm(0,id,pBuff);
            // dispatch message
            this->m_impl.doDispatch();
            // Verify event
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded(0,id);

            // retrieve it
            U8 testVal;
            this->invoke_to_getPrm(0,id,pBuff);
            // deserialize it
            stat = pBuff.deserialize(testVal);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
            EXPECT_EQ(testVal,val);
        }

        {
            // ID = 0x1101
            F32 val = 33.34;
            FwPrmIdType id = 0x1101;

            Fw::ParamBuffer pBuff;

            Fw::SerializeStatus stat = pBuff.serialize(val);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);

            // clear all events
            this->clearEvents();
            // send it to the database
            this->invoke_to_setPrm(0,id,pBuff);
            // dispatch message
            this->m_impl.doDispatch();
            // Verify event
            ASSERT_EVENTS_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded_SIZE(1);
            ASSERT_EVENTS_PrmIdAdded(0,id);

            // retrieve it
            F32 testVal;
            this->invoke_to_getPrm(0,id,pBuff);
            // deserialize it
            stat = pBuff.deserialize(testVal);
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
            EXPECT_EQ(testVal,val);
        }

        this->clearEvents();
        this->clearHistory();
        this->sendCmd_PRM_SAVE_FILE(0,12);
        Fw::QueuedComponentBase::MsgDispatchStatus mstat = this->m_impl.doDispatch();
        EXPECT_EQ(mstat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);

        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_OK);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete(0,4);

    }

    void PrmDbImplTester::init(NATIVE_INT_TYPE instance) {
        PrmDbGTestBase::init();
    }


void PrmDbImplTester::runFileReadError(void) {

        // File open error

        this->clearEvents();
        // register interceptor
        Os::registerOpenInterceptor(this->OpenInterceptor,static_cast<void*>(this));
        this->m_testOpenStatus = Os::File::DOESNT_EXIST;
        // call function to read parameter file
        this->m_impl.readParamFile();
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_OPEN,0,Os::File::DOESNT_EXIST);
        Os::clearOpenInterceptor();

        // Test delimiter read error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // delimiter is first read
        this->m_readsToWait = 0;
        // set read status to bad
        this->m_testReadStatus = Os::File::NOT_OPENED;
        // set test type to read error
        this->m_readTestType = FILE_READ_READ_ERROR;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_DELIMITER,0,Os::File::NOT_OPENED);
        Os::clearReadInterceptor();

        // Test delimiter read size error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // delimiter is first read
        this->m_readsToWait = 0;
        // set read status to okay
        this->m_testReadStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_readTestType = FILE_READ_SIZE_ERROR;
        // set size to size of byte + 1
        this->m_readSize = sizeof(U8)+1;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_DELIMITER_SIZE,0,sizeof(U8) + 1);
        Os::clearReadInterceptor();

        // Test delimiter value error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // delimiter is first read
        this->m_readsToWait = 0;
        // set read status to okay
        this->m_testReadStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_readTestType = FILE_READ_DATA_ERROR;
        // set incorrect read value
        this->m_readData[0] = 0x11;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_DELIMITER_VALUE,0,0x11);

        Os::clearReadInterceptor();

        // Test record size read error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // size is second read
        this->m_readsToWait = 1;
        // set read status to bad
        this->m_testReadStatus = Os::File::NOT_OPENED;
        // set test type to read error
        this->m_readTestType = FILE_READ_READ_ERROR;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_RECORD_SIZE,0,Os::File::NOT_OPENED);

        Os::clearReadInterceptor();

        // Test record size read size error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // size is second read
        this->m_readsToWait = 1;
        // set read status to okay
        this->m_testReadStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_readTestType = FILE_READ_SIZE_ERROR;
        // set size to size of byte + 1
        this->m_readSize = sizeof(U32)+1;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_RECORD_SIZE_SIZE,0,sizeof(U32)+1);

        Os::clearReadInterceptor();

        // Test record size value too big error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // size is second read
        this->m_readsToWait = 1;
        // set read status to okay
        this->m_testReadStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_readTestType = FILE_READ_DATA_ERROR;
        Fw::ParamBuffer pBuff;
        pBuff.serialize((U32) (FW_PARAM_BUFFER_MAX_SIZE + sizeof(U32) + 1));
        memcpy(this->m_readData,pBuff.getBuffAddr(),pBuff.getBuffLength());
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_RECORD_SIZE_VALUE,0,FW_PARAM_BUFFER_MAX_SIZE + sizeof(U32) + 1);

        Os::clearReadInterceptor();

        // Test parameter ID read error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // ID is third read
        this->m_readsToWait = 2;
        // set read status to bad
        this->m_testReadStatus = Os::File::NOT_OPENED;
        // set test type to read error
        this->m_readTestType = FILE_READ_READ_ERROR;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_PARAMETER_ID,0,Os::File::NOT_OPENED);

        Os::clearReadInterceptor();

        // Test parameter ID read size error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // size is second read
        this->m_readsToWait = 2;
        // set read status to okay
        this->m_testReadStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_readTestType = FILE_READ_SIZE_ERROR;
        // set size to size of byte + 1
        this->m_readSize = sizeof(FwPrmIdType)+1;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_PARAMETER_ID_SIZE,0,sizeof(FwPrmIdType)+1);

        Os::clearReadInterceptor();

        // Test parameter value read error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // record is fourth read
        this->m_readsToWait = 3;
        // set read status to bad
        this->m_testReadStatus = Os::File::NOT_OPENED;
        // set test type to read error
        this->m_readTestType = FILE_READ_READ_ERROR;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_PARAMETER_VALUE,0,Os::File::NOT_OPENED);

        Os::clearReadInterceptor();

        // Test parameter value read size error

        this->clearEvents();
        Os::registerReadInterceptor(this->ReadInterceptor,static_cast<void*>(this));
        // record is fourth read
        this->m_readsToWait = 3;
        // set read status to okay
        this->m_testReadStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_readTestType = FILE_READ_SIZE_ERROR;
        // set size to size of U32 + 1, which is nominal populate test value
        this->m_readSize = sizeof(U32)+1;
        // call function to read file
        this->m_impl.readParamFile();
        // check event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError_SIZE(1);
        ASSERT_EVENTS_PrmFileReadError(0,PrmDbImpl::PRM_READ_PARAMETER_VALUE_SIZE,0,sizeof(U32)+1);

        Os::clearReadInterceptor();

    }

    void PrmDbImplTester::runFileWriteError(void) {

        // File open error

        this->clearEvents();
        // register interceptor
        Os::registerOpenInterceptor(this->OpenInterceptor,static_cast<void*>(this));
        this->m_testOpenStatus = Os::File::DOESNT_EXIST;
        // dispatch command
        this->sendCmd_PRM_SAVE_FILE(0,12);
        Fw::QueuedComponentBase::MsgDispatchStatus stat = this->m_impl.doDispatch();
        ASSERT_EQ(stat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_OPEN,0,Os::File::DOESNT_EXIST);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);

        Os::clearOpenInterceptor();

        // Test delimiter write error

        // populate file again
        this->runNominalPopulate();

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // delimiter is first read
        this->m_writesToWait = 0;
        // set read status to bad
        this->m_testWriteStatus = Os::File::NOT_OPENED;
        // set test type to read error
        this->m_writeTestType = FILE_WRITE_WRITE_ERROR;

        // send command to save file
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_DELIMITER,0,Os::File::NOT_OPENED);
        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);
        Os::clearWriteInterceptor();

        // Test delimiter write size error

        // populate file again
        this->runNominalPopulate();

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // delimiter is first read
        this->m_writesToWait = 0;
        // set write status to okay
        this->m_testWriteStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_writeTestType = FILE_WRITE_SIZE_ERROR;
        // set size to size of byte + 1
        this->m_writeSize = sizeof(U8)+1;
        // send command to save file
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_DELIMITER_SIZE,0,sizeof(U8)+1);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);

        Os::clearWriteInterceptor();

        // Test record size write error

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // size is second write
        this->m_writesToWait = 1;
        // set write status to bad
        this->m_testWriteStatus = Os::File::NOT_OPENED;
        // set test type to write error
        this->m_writeTestType = FILE_WRITE_WRITE_ERROR;
        // send command to save file
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_RECORD_SIZE,0,Os::File::NOT_OPENED);
        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);
        Os::clearWriteInterceptor();

        // Test record size write size error

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // size is second read
        this->m_writesToWait = 1;
        // set read status to okay
        this->m_testWriteStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_writeTestType = FILE_WRITE_SIZE_ERROR;
        // set size to size of U32 + 1
        this->m_writeSize = sizeof(U32)+1;
        // send command to save file
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_RECORD_SIZE_SIZE,0,sizeof(U32)+1);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);

        Os::clearWriteInterceptor();

        // Test parameter ID write error

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // ID is third read
        this->m_writesToWait = 2;
        // set write status to bad
        this->m_testWriteStatus = Os::File::NOT_OPENED;
        // set test type to write error
        this->m_writeTestType = FILE_WRITE_WRITE_ERROR;
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_PARAMETER_ID,0,Os::File::NOT_OPENED);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);

        Os::clearWriteInterceptor();

        // Test parameter ID write size error

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // size is second read
        this->m_writesToWait = 2;
        // set read status to okay
        this->m_testWriteStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_writeTestType = FILE_WRITE_SIZE_ERROR;
        // set size to size of byte + 1
        this->m_writeSize = sizeof(FwPrmIdType)+1;
        // send command to save file
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_PARAMETER_ID_SIZE,0,sizeof(FwPrmIdType)+1);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);

        Os::clearWriteInterceptor();

        // Test parameter value write error

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // record is fourth read
        this->m_writesToWait = 3;
        // set read status to bad
        this->m_testWriteStatus = Os::File::NOT_OPENED;
        // set test type to read error
        this->m_writeTestType = FILE_WRITE_WRITE_ERROR;
        // send write command
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_PARAMETER_VALUE,0,Os::File::NOT_OPENED);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);
        Os::clearWriteInterceptor();

        // Test parameter value write size error

        this->clearEvents();
        this->clearHistory();
        Os::registerWriteInterceptor(this->WriteInterceptor,static_cast<void*>(this));
        // record is fourth read
        this->m_writesToWait = 3;
        // set read status to okay
        this->m_testWriteStatus = Os::File::OP_OK;
        // set test type to read error
        this->m_writeTestType = FILE_WRITE_SIZE_ERROR;
        // set size to size of U32 + 1, which is nominal populate test value
        this->m_writeSize = sizeof(U32)+1;
        // send command
        this->sendCmd_PRM_SAVE_FILE(0,12);
        stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmDbImpl::PRM_WRITE_PARAMETER_VALUE_SIZE,0,sizeof(U32)+1);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::COMMAND_EXECUTION_ERROR);
        Os::clearWriteInterceptor();

    }

    PrmDbImplTester::PrmDbImplTester(Svc::PrmDbImpl& inst) :
            PrmDbGTestBase("testerbase",100), m_impl(inst) {

    }

    PrmDbImplTester::~PrmDbImplTester() {
    }

    bool PrmDbImplTester::OpenInterceptor(Os::File::Status& stat, const char* fileName, Os::File::Mode mode, void* ptr) {
        EXPECT_TRUE(ptr);
        PrmDbImplTester* compPtr = static_cast<PrmDbImplTester*>(ptr);
        stat = compPtr->m_testOpenStatus;
        return false;
    }

    bool PrmDbImplTester::ReadInterceptor(Os::File::Status& stat, void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr) {
        EXPECT_TRUE(ptr);
        PrmDbImplTester* compPtr = static_cast<PrmDbImplTester*>(ptr);
        if (not compPtr->m_readsToWait--) {
            // check test scenario
            switch (compPtr->m_readTestType) {
                case FILE_READ_READ_ERROR:
                    stat = compPtr->m_testReadStatus;
                    break;
                case FILE_READ_SIZE_ERROR:
                    size = compPtr->m_readSize;
                    stat =  Os::File::OP_OK;
                    break;
                case FILE_READ_DATA_ERROR:
                    memcpy(buffer,compPtr->m_readData,size);
                    stat =  Os::File::OP_OK;
                    break;
                default:
                    EXPECT_TRUE(false);
                    break;
            }
            return false;
        } else {
            return true;
        }
    }

    bool PrmDbImplTester::WriteInterceptor(Os::File::Status& stat, const void * buffer, NATIVE_INT_TYPE &size, bool waitForFull, void* ptr) {
        EXPECT_TRUE(ptr);
        PrmDbImplTester* compPtr = static_cast<PrmDbImplTester*>(ptr);
        if (not compPtr->m_writesToWait--) {
            // check test scenario
            switch (compPtr->m_writeTestType) {
                case FILE_WRITE_WRITE_ERROR:
                    stat = compPtr->m_testWriteStatus;
                    break;
                case FILE_WRITE_SIZE_ERROR:
                    size = compPtr->m_writeSize;
                    stat =  Os::File::OP_OK;
                    break;
                default:
                    EXPECT_TRUE(false);
                    break;
            }
            return false;
        } else {
            return true;
        }
    }
    void PrmDbImplTester ::
      from_pingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
      this->pushFromPortEntry_pingOut(key);
    }

} /* namespace SvcTest */
