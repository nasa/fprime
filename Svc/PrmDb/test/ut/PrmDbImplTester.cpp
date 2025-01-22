/*
 * PrmDbImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/PrmDb/test/ut/PrmDbImplTester.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Os/Stub/test/File.hpp>
#include <Os/Delegate.hpp>
#include <cstdio>
#include <gtest/gtest.h>
#include "Os/Stub/FileSystem.hpp"
#include "Os/Stub/Directory.hpp"
#include <Fw/Test/UnitTest.hpp>

namespace Svc {

    typedef PrmDb_PrmWriteError PrmWriteError;
    typedef PrmDb_PrmReadError PrmReadError;

    void PrmDbImplTester::runNominalPopulate() {
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

    void PrmDbImplTester::runNominalSaveFile() {
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

        this->sendCmd_PRM_SAVE_FILE(0,12);
        Fw::QueuedComponentBase::MsgDispatchStatus stat = this->m_impl.doDispatch();
        EXPECT_EQ(stat,Fw::QueuedComponentBase::MSG_DISPATCH_OK);
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::CmdResponse::OK);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete(0,2);

    }

    void PrmDbImplTester::runNominalLoadFile() {
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
        ASSERT_EVENTS_PrmFileLoadComplete(0,2);

        // verify values (populated by runNominalPopulate())

        // first
        Fw::ParamBuffer pBuff;
        U32 testVal;
        this->invoke_to_getPrm(0,0x21,pBuff);
        // deserialize it
        Fw::SerializeStatus stat = pBuff.deserialize(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
        EXPECT_EQ(testVal,0x15u);

        // second
        pBuff.resetSer();
        this->invoke_to_getPrm(0,0x25,pBuff);
        // deserialize it
        stat = pBuff.deserialize(testVal);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK,stat);
        EXPECT_EQ(testVal,0x30u);

    }

    void PrmDbImplTester::runMissingExtraParams() {

        // load up database
        this->runNominalPopulate();
        // ask for ID that isn't present
        this->clearEvents();
        Fw::ParamBuffer pBuff;
        EXPECT_EQ(Fw::ParamValid::INVALID,this->invoke_to_getPrm(0,0x1000,pBuff).e);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmIdNotFound_SIZE(1);
        ASSERT_EVENTS_PrmIdNotFound(0,0x1000);

        // clear database
        this->m_impl.clearDb();

        this->clearEvents();
        // write too many entries
        for (FwPrmIdType entry = 0; entry <= PRMDB_NUM_DB_ENTRIES; entry++) {
            EXPECT_EQ(Fw::FW_SERIALIZE_OK,pBuff.serialize(static_cast<U32>(10)));
            this->invoke_to_setPrm(0,entry,pBuff);
            // dispatch message
            this->m_impl.doDispatch();
        }

        ASSERT_EVENTS_SIZE(PRMDB_NUM_DB_ENTRIES+1);
        ASSERT_EVENTS_PrmIdAdded_SIZE(PRMDB_NUM_DB_ENTRIES);
        ASSERT_EVENTS_PrmDbFull_SIZE(1);
        ASSERT_EVENTS_PrmDbFull(0,PRMDB_NUM_DB_ENTRIES);

    }

    void PrmDbImplTester::runRefPrmFile() {

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
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::CmdResponse::OK);
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete_SIZE(1);
        ASSERT_EVENTS_PrmFileSaveComplete(0,4);

    }

    void PrmDbImplTester::init(NATIVE_INT_TYPE instance) {
        PrmDbGTestBase::init();
    }

    PrmDbImplTester* PrmDbImplTester::PrmDbTestFile::s_tester = nullptr;

    void PrmDbImplTester::PrmDbTestFile::setTester(Svc::PrmDbImplTester *tester) {
        ASSERT_NE(tester, nullptr);
        s_tester = tester;
    }

    Os::File::Status PrmDbImplTester::PrmDbTestFile::read(U8 *buffer, FwSignedSizeType &size, Os::File::WaitType wait) {
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

    Os::File::Status PrmDbImplTester::PrmDbTestFile::write(const U8* buffer, FwSignedSizeType &size, Os::File::WaitType wait) {
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

    void PrmDbImplTester::runFileReadError() {
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
                    ASSERT_EVENTS_PrmFileReadError(0,PrmReadError::PARAMETER_ID_SIZE, 0, sizeof(FwPrmIdType) + 1);
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
                        ASSERT_EVENTS_PrmFileReadError(0,PrmReadError::DELIMITER, 0, this->m_status);
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
                    U32 expected_error_value = 8 + (1 << ((sizeof(U32) - 1) * 8));
                    ASSERT_EVENTS_PrmFileReadError_SIZE(1);
                    ASSERT_EVENTS_PrmFileReadError(0, PrmReadError::RECORD_SIZE_VALUE, 0, expected_error_value);
                    break;
                }
                default:
                    FAIL() << "Reached unknown case";
            }
        }
    }


    void PrmDbImplTester::runFileWriteError() {
        // File open error
        this->clearEvents();
        // register interceptor
        Os::Stub::File::Test::StaticData::setWriteResult(m_io_data, sizeof m_io_data);
        Os::Stub::File::Test::StaticData::setNextStatus(Os::File::DOESNT_EXIST);
        // dispatch command
        this->sendCmd_PRM_SAVE_FILE(0,12);
        Fw::QueuedComponentBase::MsgDispatchStatus stat = this->m_impl.doDispatch();
        ASSERT_EQ(stat, Fw::QueuedComponentBase::MSG_DISPATCH_OK);

        // check for failed event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError_SIZE(1);
        ASSERT_EVENTS_PrmFileWriteError(0,PrmWriteError::OPEN,0,Os::File::DOESNT_EXIST);

        // check command status
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::CmdResponse::EXECUTION_ERROR);

        this->runNominalPopulate();

        // Loop through all size errors testing each
        Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
        this->m_errorType = FILE_SIZE_ERROR;
        for (FwSizeType i = 0; i < 4; i++) {
            clearEvents();
            this->clearHistory();
            this->m_waits = i;
            this->sendCmd_PRM_SAVE_FILE(0,12);
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
            ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::CmdResponse::EXECUTION_ERROR);
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
                this->sendCmd_PRM_SAVE_FILE(0,12);
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
                ASSERT_CMD_RESPONSE(0,PrmDbImpl::OPCODE_PRM_SAVE_FILE,12,Fw::CmdResponse::EXECUTION_ERROR);
            }
        }
    }

    PrmDbImplTester::PrmDbImplTester(Svc::PrmDbImpl& inst) :
            PrmDbGTestBase("testerbase",100), m_impl(inst) {
        PrmDbImplTester::PrmDbTestFile::setTester(this);
    }

    PrmDbImplTester::~PrmDbImplTester() {
    }

    void PrmDbImplTester ::
      from_pingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
      this->pushFromPortEntry_pingOut(key);
    }
} /* namespace Svc */

namespace Os {

//! \brief get a delegate for FileInterface that intercepts calls for parameter database testing
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileInterface *FileInterface::getDelegate(FileHandleStorage& aligned_placement_new_memory, const FileInterface* to_copy) {
    return Os::Delegate::makeDelegate<FileInterface, Svc::PrmDbImplTester::PrmDbTestFile>(
            aligned_placement_new_memory, to_copy
    );
}

//! \brief get a delegate for FileSystemInterface that intercepts calls for stub fileSystem usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
FileSystemInterface *FileSystemInterface::getDelegate(FileSystemHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<FileSystemInterface, Os::Stub::FileSystem::StubFileSystem>(
        aligned_placement_new_memory
    );
}

//! \brief get a delegate for DirectoryInterface that intercepts calls for stub Directory usage
//! \param aligned_new_memory: aligned memory to fill
//! \return: pointer to delegate
DirectoryInterface *DirectoryInterface::getDelegate(DirectoryHandleStorage& aligned_placement_new_memory) {
    return Os::Delegate::makeDelegate<DirectoryInterface, Os::Stub::Directory::StubDirectory>(
        aligned_placement_new_memory
    );
}
}
