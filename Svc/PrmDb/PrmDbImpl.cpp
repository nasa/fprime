/*
 * PrmDbImpl.cpp
 *
 *  Created on: March 9, 2015
 *      Author: Timothy Canham
 */

#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Fw/Types/Assert.hpp>

#include <Os/File.hpp>

#include <cstring>
#include <stdio.h>

namespace Svc {

    // anonymous namespace for buffer declaration
    namespace {
        class WorkingBuffer : public Fw::SerializeBufferBase {
            public:

                NATIVE_UINT_TYPE getBuffCapacity(void) const {
                    return sizeof(m_buff);
                }

                U8* getBuffAddr(void) {
                    return m_buff;
                }

                const U8* getBuffAddr(void) const {
                    return m_buff;
                }

            private:
                // Set to max of parameter buffer + id
                U8 m_buff[FW_PARAM_BUFFER_MAX_SIZE + sizeof(FwPrmIdType)];
        };
    }

    PrmDbImpl::PrmDbImpl(const char* name, const char* file) : PrmDbComponentBase(name) {
        this->clearDb();
        this->m_fileName = file;
    }

    void PrmDbImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        PrmDbComponentBase::init(queueDepth,instance);
    }

    void PrmDbImpl::clearDb(void) {
        for (I32 entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
            this->m_db[entry].used = false;
            this->m_db[entry].id = 0;
        }
    }

    // If ports are no longer guarded, these accesses need to be protected from each other
    // If there are a lot of accesses, perhaps an interrupt lock could be used instead of guarded ports

    Fw::ParamValid PrmDbImpl::getPrm_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val) {
        // search for entry
        Fw::ParamValid stat = Fw::PARAM_INVALID;

        for (I32 entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
            if (this->m_db[entry].used) {
                if (this->m_db[entry].id == id) {
                    val = this->m_db[entry].val;
                    stat = Fw::PARAM_VALID;
                    break;
                }
            }
        }

        // if unable to find parameter, send error message
        if (Fw::PARAM_INVALID == stat) {
            this->log_WARNING_LO_PrmIdNotFound(id);
        }

        return stat;
    }

    void PrmDbImpl::setPrm_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val) {

        this->lock();

        // search for existing entry

        bool existingEntry = false;
        bool noSlots = true;

        for (NATIVE_INT_TYPE entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
            if ((this->m_db[entry].used) && (id == this->m_db[entry].id)) {
                this->m_db[entry].val = val;
                existingEntry = true;
                break;
            }
        }

        // if there is no existing entry, add one
        if (!existingEntry) {
            for (I32 entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
                if (!(this->m_db[entry].used)) {
                    this->m_db[entry].val = val;
                    this->m_db[entry].id = id;
                    this->m_db[entry].used = true;
                    noSlots = false;
                    break;
                }
            }
        }

        this->unLock();

        if (existingEntry) {
            this->log_ACTIVITY_HI_PrmIdUpdated(id);
        } else if (noSlots) {
            this->log_FATAL_PrmDbFull(id);
        } else {
            this->log_ACTIVITY_HI_PrmIdAdded(id);
        }

    }

    void PrmDbImpl::PRM_SAVE_FILE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {

        Os::File paramFile;
        WorkingBuffer buff;

        Os::File::Status stat = paramFile.open(this->m_fileName.toChar(),Os::File::OPEN_WRITE);
        if (stat != Os::File::OP_OK) {
            this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_OPEN,0,stat);
            this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
            return;
        }

        this->lock();

        // Traverse the parameter list, saving each entry

        U32 numRecords = 0;

        for (NATIVE_UINT_TYPE entry = 0; entry < FW_NUM_ARRAY_ELEMENTS(this->m_db); entry++) {
            if (this->m_db[entry].used) {
                // write delimiter
                static const U8 delim = PRMDB_ENTRY_DELIMITER;
                NATIVE_INT_TYPE writeSize = sizeof(delim);
                stat = paramFile.write(&delim,writeSize,true);
                if (stat != Os::File::OP_OK) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_DELIMITER,numRecords,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }
                if (writeSize != sizeof(delim)) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_DELIMITER_SIZE,numRecords,writeSize);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }
                // serialize record size = id field + data
                U32 recordSize = sizeof(FwPrmIdType) + this->m_db[entry].val.getBuffLength();

                // reset buffer
                buff.resetSer();
                Fw::SerializeStatus serStat = buff.serialize(recordSize);
                // should always work
                FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat,(NATIVE_INT_TYPE)serStat);

                // write record size
                writeSize = buff.getBuffLength();
                stat = paramFile.write(buff.getBuffAddr(),writeSize,true);
                if (stat != Os::File::OP_OK) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_RECORD_SIZE,numRecords,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }
                if (writeSize != sizeof(writeSize)) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_RECORD_SIZE_SIZE,numRecords,writeSize);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }

                // reset buffer
                buff.resetSer();

                // serialize parameter id

                serStat = buff.serialize(this->m_db[entry].id);
                // should always work
                FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat,(NATIVE_INT_TYPE)serStat);

                // write parameter ID
                writeSize = buff.getBuffLength();
                stat = paramFile.write(buff.getBuffAddr(),writeSize,true);
                if (stat != Os::File::OP_OK) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_PARAMETER_ID,numRecords,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }
                if (writeSize != (NATIVE_INT_TYPE)buff.getBuffLength()) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_PARAMETER_ID_SIZE,numRecords,writeSize);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }

                // write serialized parameter value

                writeSize = this->m_db[entry].val.getBuffLength();
                stat = paramFile.write(this->m_db[entry].val.getBuffAddr(),writeSize,true);
                if (stat != Os::File::OP_OK) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_PARAMETER_VALUE,numRecords,stat);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }
                if (writeSize != (NATIVE_INT_TYPE)this->m_db[entry].val.getBuffLength()) {
                    this->unLock();
                    this->log_WARNING_HI_PrmFileWriteError(PRM_WRITE_PARAMETER_VALUE_SIZE,numRecords,writeSize);
                    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_EXECUTION_ERROR);
                    return;
                }
                numRecords++;
            } // end if record in use
        } // end for each record

        this->unLock();
        this->log_ACTIVITY_HI_PrmFileSaveComplete(numRecords);
        this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);

    }

    PrmDbImpl::~PrmDbImpl() {
    }

    void PrmDbImpl::readParamFile(void) {
        // load file. FIXME: Put more robust file checking, such as a CRC.
        Os::File paramFile;

        Os::File::Status stat = paramFile.open(this->m_fileName.toChar(),Os::File::OPEN_READ);
        if (stat != Os::File::OP_OK) {
            this->log_WARNING_HI_PrmFileReadError(PRM_READ_OPEN,0,stat);
            return;
        }

        WorkingBuffer buff;

        U32 recordNum = 0;

        this->clearDb();

        for (NATIVE_INT_TYPE entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++)  {

            U8 delimiter;
            NATIVE_INT_TYPE readSize = sizeof(delimiter);

            // read delimiter
            Os::File::Status fStat = paramFile.read(&delimiter,readSize,true);

            // check for end of file (read size 0)
            if (0 == readSize) {
                break;
            }

            if (fStat != Os::File::OP_OK) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_DELIMITER,recordNum,fStat);
                return;
            }

            if (sizeof(delimiter) != readSize) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_DELIMITER_SIZE,recordNum,readSize);
                return;
            }

            if (PRMDB_ENTRY_DELIMITER != delimiter) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_DELIMITER_VALUE,recordNum,delimiter);
                return;
            }

            U32 recordSize = 0;
            // read record size
            readSize = sizeof(recordSize);

            fStat = paramFile.read(buff.getBuffAddr(),readSize,true);
            if (fStat != Os::File::OP_OK) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_RECORD_SIZE,recordNum,fStat);
                return;
            }
            if (sizeof(recordSize) != readSize) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_RECORD_SIZE_SIZE,recordNum,readSize);
                return;
            }
            // set serialized size to read size
            Fw::SerializeStatus desStat = buff.setBuffLen(readSize);
            // should never fail
            FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat,(NATIVE_INT_TYPE)desStat);
            // reset deserialization
            buff.resetDeser();
            // deserialize, since record size is serialized in file
            FW_ASSERT(Fw::FW_SERIALIZE_OK == buff.deserialize(recordSize));

            // sanity check value. It can't be larger than the maximum parameter buffer size + id
            // or smaller than the record id
            if ((recordSize > FW_PARAM_BUFFER_MAX_SIZE + sizeof(U32)) or (recordSize < sizeof(U32))) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_RECORD_SIZE_VALUE,recordNum,recordSize);
                return;
            }

            // read the parameter ID
            FwPrmIdType parameterId = 0;
            readSize = sizeof(FwPrmIdType);

            fStat = paramFile.read(buff.getBuffAddr(),readSize,true);
            if (fStat != Os::File::OP_OK) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_PARAMETER_ID,recordNum,fStat);
                return;
            }
            if (sizeof(parameterId) != static_cast<NATIVE_INT_TYPE>(readSize)) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_PARAMETER_ID_SIZE,recordNum,readSize);
                return;
            }

            // set serialized size to read parameter ID
            desStat = buff.setBuffLen(readSize);
            // should never fail
            FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat,(NATIVE_INT_TYPE)desStat);
            // reset deserialization
            buff.resetDeser();
            // deserialize, since parameter ID is serialized in file
            FW_ASSERT(Fw::FW_SERIALIZE_OK == buff.deserialize(parameterId));

            // copy parameter
            this->m_db[entry].used = true;
            this->m_db[entry].id = parameterId;
            readSize = recordSize-sizeof(parameterId);

            fStat = paramFile.read(this->m_db[entry].val.getBuffAddr(),readSize);

            if (fStat != Os::File::OP_OK) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_PARAMETER_VALUE,recordNum,fStat);
                return;
            }
            if (static_cast<U32>(readSize) != recordSize-sizeof(parameterId)) {
                this->log_WARNING_HI_PrmFileReadError(PRM_READ_PARAMETER_VALUE_SIZE,recordNum,readSize);
                return;
            }

            // set serialized size to read size
            desStat = this->m_db[entry].val.setBuffLen(readSize);
            // should never fail
            FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat,(NATIVE_INT_TYPE)desStat);
            recordNum++;

        }

        this->log_ACTIVITY_HI_PrmFileLoadComplete(recordNum);
    }

    void PrmDbImpl::pingIn_handler(NATIVE_INT_TYPE portNum, U32 key) {
        // respond to ping
        this->pingOut_out(0,key);
    }



}
