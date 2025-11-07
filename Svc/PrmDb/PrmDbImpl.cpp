/*
 * PrmDbImpl.cpp
 *
 *  Created on: March 9, 2015
 *      Author: Timothy Canham
 */

#include <Fw/Types/Assert.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>

#include <Os/File.hpp>

#include <cstdio>
#include <cstring>

static_assert(std::numeric_limits<FwSizeType>::max() >= PRMDB_NUM_DB_ENTRIES,
              "PRMDB_NUM_DB_ENTRIES must fit within range of FwSizeType");

namespace Svc {

// anonymous namespace for buffer declaration
namespace {
class WorkingBuffer : public Fw::SerializeBufferBase {
  public:
    FwSizeType getCapacity() const { return sizeof(m_buff); }

    U8* getBuffAddr() { return m_buff; }

    const U8* getBuffAddr() const { return m_buff; }

  private:
    // Set to max of parameter buffer + id
    U8 m_buff[FW_PARAM_BUFFER_MAX_SIZE + sizeof(FwPrmIdType)];
};
}  // namespace

//! ----------------------------------------------------------------------
//! Construction, initialization, and destruction
//! ----------------------------------------------------------------------
PrmDbImpl::PrmDbImpl(const char* name) : PrmDbComponentBase(name), m_state(PrmDbFileLoadState::IDLE) {
    this->m_activeDb = this->m_dbStore1;
    this->m_stagingDb = this->m_dbStore2;

    this->clearDb(PrmDbType::DB_ACTIVE);
    this->clearDb(PrmDbType::DB_STAGING);
}

PrmDbImpl::~PrmDbImpl() {}

void PrmDbImpl::configure(const char* file) {
    FW_ASSERT(file != nullptr);
    this->m_fileName = file;
}

void PrmDbImpl::readParamFile() {
    // Assumed to run at initialization time
    // State should be IDLE upon entry
    FW_ASSERT(static_cast<FwAssertArgType>(m_state == PrmDbFileLoadState::IDLE));

    // Clear databases
    this->clearDb(PrmDbType::DB_ACTIVE);
    this->clearDb(PrmDbType::DB_STAGING);

    // Read parameter file to active database
    (void)readParamFileImpl(this->m_fileName, PrmDbType::DB_ACTIVE);
}

//! ----------------------------------------------------------------------
//! Port & Command Handlers
//! ----------------------------------------------------------------------

// If ports are no longer guarded, these accesses need to be protected from each other
// If there are a lot of accesses, perhaps an interrupt lock could be used instead of guarded ports

Fw::ParamValid PrmDbImpl::getPrm_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    // search for entry
    Fw::ParamValid stat = Fw::ParamValid::INVALID;

    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        if (this->m_activeDb[entry].used) {
            if (this->m_activeDb[entry].id == id) {
                val = this->m_activeDb[entry].val;
                stat = Fw::ParamValid::VALID;
                break;
            }
        }
    }

    // if unable to find parameter, send error message
    if (Fw::ParamValid::INVALID == stat.e) {
        this->log_WARNING_LO_PrmIdNotFound(id);
    }

    return stat;
}

void PrmDbImpl::setPrm_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    // Reject parameter updates during non-idle file load states
    if (m_state != PrmDbFileLoadState::IDLE) {
        this->log_WARNING_LO_PrmDbFileLoadInvalidAction(m_state, PrmDb_PrmLoadAction::SET_PARAMETER);
        return;
    }

    // Update the parameter in the active database
    PrmUpdateType update_status = updateAddPrmImpl(id, val, PrmDbType::DB_ACTIVE);

    // Issue relevant EVR
    if (update_status == PARAM_UPDATED) {
        this->log_ACTIVITY_HI_PrmIdUpdated(id);
    } else if (update_status == NO_SLOTS) {
        this->log_WARNING_HI_PrmDbFull(id);
    } else {
        this->log_ACTIVITY_HI_PrmIdAdded(id);
    }
}

void PrmDbImpl::pingIn_handler(FwIndexType portNum, U32 key) {
    // respond to ping
    this->pingOut_out(0, key);
}

void PrmDbImpl::PRM_SAVE_FILE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Reject PRM_SAVE_FILE command during non-idle file load states
    if (m_state != PrmDbFileLoadState::IDLE) {
        this->log_WARNING_LO_PrmDbFileLoadInvalidAction(m_state, PrmDb_PrmLoadAction::SAVE_FILE_COMMAND);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
        return;
    }

    FW_ASSERT(this->m_fileName.length() > 0);

    Os::File paramFile;
    WorkingBuffer buff;

    Os::File::Status stat = paramFile.open(this->m_fileName.toChar(), Os::File::OPEN_WRITE);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::OPEN, 0, stat);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->lock();
    t_dbStruct* db = getDbPtr(PrmDbType::DB_ACTIVE);
    FW_ASSERT(db != nullptr);

    // Traverse the parameter list, saving each entry

    U32 numRecords = 0;

    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        if (db[entry].used) {
            // write delimiter
            static const U8 delim = PRMDB_ENTRY_DELIMITER;
            FwSizeType writeSize = static_cast<FwSizeType>(sizeof(delim));
            stat = paramFile.write(&delim, writeSize, Os::File::WaitType::WAIT);
            if (stat != Os::File::OP_OK) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::DELIMITER, static_cast<I32>(numRecords), stat);
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            if (writeSize != sizeof(delim)) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::DELIMITER_SIZE, static_cast<I32>(numRecords),
                                                       static_cast<I32>(writeSize));
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            // serialize record size = id field + data
            U32 recordSize = static_cast<U32>(sizeof(FwPrmIdType) + db[entry].val.getSize());

            // reset buffer
            buff.resetSer();
            Fw::SerializeStatus serStat = buff.serializeFrom(recordSize);
            // should always work
            FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, static_cast<FwAssertArgType>(serStat));

            // write record size
            writeSize = static_cast<FwSizeType>(buff.getSize());
            stat = paramFile.write(buff.getBuffAddr(), writeSize, Os::File::WaitType::WAIT);
            if (stat != Os::File::OP_OK) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::RECORD_SIZE, static_cast<I32>(numRecords), stat);
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            if (writeSize != sizeof(recordSize)) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::RECORD_SIZE_SIZE, static_cast<I32>(numRecords),
                                                       static_cast<I32>(writeSize));
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }

            // reset buffer
            buff.resetSer();

            // serialize parameter id

            serStat = buff.serializeFrom(db[entry].id);
            // should always work
            FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, static_cast<FwAssertArgType>(serStat));

            // write parameter ID
            writeSize = static_cast<FwSizeType>(buff.getSize());
            stat = paramFile.write(buff.getBuffAddr(), writeSize, Os::File::WaitType::WAIT);
            if (stat != Os::File::OP_OK) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::PARAMETER_ID, static_cast<I32>(numRecords), stat);
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            if (writeSize != static_cast<FwSizeType>(buff.getSize())) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::PARAMETER_ID_SIZE, static_cast<I32>(numRecords),
                                                       static_cast<I32>(writeSize));
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }

            // write serialized parameter value

            writeSize = static_cast<FwSizeType>(db[entry].val.getSize());
            stat = paramFile.write(db[entry].val.getBuffAddr(), writeSize, Os::File::WaitType::WAIT);
            if (stat != Os::File::OP_OK) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::PARAMETER_VALUE, static_cast<I32>(numRecords),
                                                       stat);
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            if (writeSize != static_cast<FwSizeType>(db[entry].val.getSize())) {
                this->unLock();
                this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::PARAMETER_VALUE_SIZE,
                                                       static_cast<I32>(numRecords), static_cast<I32>(writeSize));
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            numRecords++;
        }  // end if record in use
    }  // end for each record

    this->unLock();
    this->log_ACTIVITY_HI_PrmFileSaveComplete(numRecords);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PrmDbImpl::PRM_LOAD_FILE_cmdHandler(FwOpcodeType opCode,
                                         U32 cmdSeq,
                                         const Fw::CmdStringArg& fileName,
                                         PrmDb_Merge merge) {
    // Reject PRM_LOAD_FILE command during non-idle file load states
    if (m_state != PrmDbFileLoadState::IDLE) {
        this->log_WARNING_LO_PrmDbFileLoadInvalidAction(m_state, PrmDb_PrmLoadAction::LOAD_FILE_COMMAND);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
        return;
    }

    // Set state to loading
    m_state = PrmDbFileLoadState::LOADING_FILE_UPDATES;

    // If reset is true, clear the staging database first
    if (merge == PrmDb_Merge::MERGE) {
        // Copy active to staging for merging
        dbCopy(PrmDbType::DB_STAGING, PrmDbType::DB_ACTIVE);
    } else {
        // reset staging db, all file contents will be loaded but no old parameters will be retained
        this->clearDb(PrmDbType::DB_STAGING);
    }

    // Load the file into staging database
    // The readParamFileImpl will emit the relevant EVR if the file load fails
    // and also if it succeeds will emit EVRs with the number of records
    PrmDbImpl::PrmLoadStatus success = PrmDbImpl::readParamFileImpl(fileName, PrmDbType::DB_STAGING);

    if (success == PrmLoadStatus::SUCCESS) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        m_state = PrmDbFileLoadState::FILE_UPDATES_STAGED;
    } else {
        this->log_WARNING_HI_PrmDbFileLoadFailed();
        // clear the staging DB and reset to an IDLE state in case of issues
        this->clearDb(PrmDbType::DB_STAGING);
        m_state = PrmDbFileLoadState::IDLE;
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void PrmDbImpl::PRM_COMMIT_STAGED_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Verify we are in the correct state
    if (m_state != PrmDbFileLoadState::FILE_UPDATES_STAGED) {
        this->log_WARNING_LO_PrmDbFileLoadInvalidAction(m_state, PrmDb_PrmLoadAction::COMMIT_STAGED_COMMAND);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    // Swap active and staging databases, safely w.r.t. prmGet
    this->lock();
    t_dbStruct* temp = this->m_activeDb;
    this->m_activeDb = this->m_stagingDb;
    this->unLock();
    this->m_stagingDb = temp;

    // Clear the new staging database
    this->clearDb(PrmDbType::DB_STAGING);

    // Set file load state to idle
    m_state = PrmDbFileLoadState::IDLE;

    this->log_ACTIVITY_HI_PrmDbCommitComplete();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

//! ----------------------------------------------------------------------
//! Helpers for construction/destruction, init, & handlers
//! ----------------------------------------------------------------------

PrmDbImpl::PrmLoadStatus PrmDbImpl::readParamFileImpl(const Fw::StringBase& fileName, PrmDbType dbType) {
    FW_ASSERT(dbType == PrmDbType::DB_ACTIVE or dbType == PrmDbType::DB_STAGING);
    FW_ASSERT(fileName.length() > 0);

    Fw::String dbString = getDbString(dbType);

    // load file. FIXME: Put more robust file checking, such as a CRC.
    Os::File paramFile;

    Os::File::Status stat = paramFile.open(fileName.toChar(), Os::File::OPEN_READ);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileReadError(PrmReadError::OPEN, 0, stat);
        return PrmLoadStatus::ERROR;
    }

    WorkingBuffer buff;

    U32 recordNumTotal = 0;
    U32 recordNumAdded = 0;
    U32 recordNumUpdated = 0;

    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        U8 delimiter;
        FwSizeType readSize = static_cast<FwSizeType>(sizeof(delimiter));

        // read delimiter
        Os::File::Status fStat = paramFile.read(&delimiter, readSize, Os::File::WaitType::WAIT);

        // check for end of file (read size 0)
        if (0 == readSize) {
            break;
        }

        if (fStat != Os::File::OP_OK) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::DELIMITER, static_cast<I32>(recordNumTotal), fStat);
            return PrmLoadStatus::ERROR;
        }

        if (sizeof(delimiter) != readSize) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::DELIMITER_SIZE, static_cast<I32>(recordNumTotal),
                                                  static_cast<I32>(readSize));
            return PrmLoadStatus::ERROR;
        }

        if (PRMDB_ENTRY_DELIMITER != delimiter) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::DELIMITER_VALUE, static_cast<I32>(recordNumTotal),
                                                  delimiter);
            return PrmLoadStatus::ERROR;
        }

        U32 recordSize = 0;

        // read record size
        readSize = sizeof(recordSize);

        fStat = paramFile.read(buff.getBuffAddr(), readSize, Os::File::WaitType::WAIT);
        if (fStat != Os::File::OP_OK) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::RECORD_SIZE, static_cast<I32>(recordNumTotal), fStat);
            return PrmLoadStatus::ERROR;
        }
        if (sizeof(recordSize) != readSize) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::RECORD_SIZE_SIZE, static_cast<I32>(recordNumTotal),
                                                  static_cast<I32>(readSize));
            return PrmLoadStatus::ERROR;
        }
        // set serialized size to read size
        Fw::SerializeStatus desStat = buff.setBuffLen(static_cast<Fw::Serializable::SizeType>(readSize));
        // should never fail
        FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat, static_cast<FwAssertArgType>(desStat));
        // reset deserialization
        buff.resetDeser();
        // deserialize, since record size is serialized in file
        desStat = buff.deserializeTo(recordSize);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat);

        // sanity check value. It can't be larger than the maximum parameter buffer size + id
        // or smaller than the record id
        if ((recordSize > FW_PARAM_BUFFER_MAX_SIZE + sizeof(U32)) or (recordSize < sizeof(U32))) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::RECORD_SIZE_VALUE, static_cast<I32>(recordNumTotal),
                                                  static_cast<I32>(recordSize));
            return PrmLoadStatus::ERROR;
        }

        // read the parameter ID
        FwPrmIdType parameterId = 0;
        readSize = static_cast<FwSizeType>(sizeof(FwPrmIdType));

        fStat = paramFile.read(buff.getBuffAddr(), readSize, Os::File::WaitType::WAIT);
        if (fStat != Os::File::OP_OK) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::PARAMETER_ID, static_cast<I32>(recordNumTotal), fStat);
            return PrmLoadStatus::ERROR;
        }
        if (sizeof(parameterId) != static_cast<FwSizeType>(readSize)) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::PARAMETER_ID_SIZE, static_cast<I32>(recordNumTotal),
                                                  static_cast<I32>(readSize));
            return PrmLoadStatus::ERROR;
        }

        // set serialized size to read parameter ID
        desStat = buff.setBuffLen(static_cast<Fw::Serializable::SizeType>(readSize));
        // should never fail
        FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat, static_cast<FwAssertArgType>(desStat));
        // reset deserialization
        buff.resetDeser();
        // deserialize, since parameter ID is serialized in file
        desStat = buff.deserializeTo(parameterId);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat);

        // copy parameter value from file into a temporary buffer
        Fw::ParamBuffer tmpParamBuffer;  // temporary param buffer to read parameter value from file
        readSize = recordSize - sizeof(parameterId);
        desStat = tmpParamBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readSize));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == desStat, static_cast<FwAssertArgType>(desStat));  // should never fail
        fStat = paramFile.read(tmpParamBuffer.getBuffAddr(), readSize);

        if (fStat != Os::File::OP_OK) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::PARAMETER_VALUE, static_cast<I32>(recordNumTotal),
                                                  fStat);
            return PrmLoadStatus::ERROR;
        }
        if (static_cast<U32>(readSize) != recordSize - sizeof(parameterId)) {
            this->log_WARNING_HI_PrmFileReadError(PrmReadError::PARAMETER_VALUE_SIZE, static_cast<I32>(recordNumTotal),
                                                  static_cast<I32>(readSize));
            return PrmLoadStatus::ERROR;
        }

        // Actually update or add parameter
        PrmUpdateType updateStatus = updateAddPrmImpl(parameterId, tmpParamBuffer, dbType);
        if (updateStatus == PARAM_ADDED) {
            recordNumAdded++;
        } else if (updateStatus == PARAM_UPDATED) {
            recordNumUpdated++;
        }

        if (updateStatus == NO_SLOTS) {
            this->log_WARNING_HI_PrmDbFull(parameterId);
        }
        recordNumTotal++;
    }

    this->log_ACTIVITY_HI_PrmFileLoadComplete(dbString, recordNumTotal, recordNumAdded, recordNumUpdated);
    return PrmLoadStatus::SUCCESS;
}

PrmDbImpl::PrmUpdateType PrmDbImpl::updateAddPrmImpl(FwPrmIdType id, Fw::ParamBuffer& val, PrmDbType prmDbType) {
    t_dbStruct* db = getDbPtr(prmDbType);

    PrmUpdateType updateStatus = NO_SLOTS;

    this->lock();
    // search for existing entry
    bool existingEntry = false;

    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        if ((db[entry].used) && (id == db[entry].id)) {
            db[entry].val = val;
            existingEntry = true;
            updateStatus = PARAM_UPDATED;
            break;
        }
    }

    // if there is no existing entry, add one
    if (!existingEntry) {
        for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
            if (!(db[entry].used)) {
                db[entry].val = val;
                db[entry].id = id;
                db[entry].used = true;
                updateStatus = PARAM_ADDED;
                break;
            }
        }
    }

    this->unLock();
    return updateStatus;
}

//! ----------------------------------------------------------------------
//! Helpers for database management
//! ----------------------------------------------------------------------

void PrmDbImpl::clearDb(PrmDbType prmDbType) {
    t_dbStruct* db = getDbPtr(prmDbType);
    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        db[entry].used = false;
        db[entry].id = 0;
    }
}

bool PrmDbImpl::dbEqual() {
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        if (!(this->m_dbStore1[i] == this->m_dbStore2[i])) {
            return false;
        }
    }
    return true;
}

void PrmDbImpl::dbCopy(PrmDbType dest, PrmDbType src) {
    for (FwSizeType i = 0; i < PRMDB_NUM_DB_ENTRIES; i++) {
        dbCopySingle(dest, src, i);
    }
    this->log_ACTIVITY_HI_PrmDbCopyAllComplete(getDbString(src), getDbString(dest));
}

void PrmDbImpl::dbCopySingle(PrmDbType dest, PrmDbType src, FwSizeType index) {
    t_dbStruct* srcPtr = getDbPtr(src);
    t_dbStruct* destPtr = getDbPtr(dest);

    FW_ASSERT(index < PRMDB_NUM_DB_ENTRIES);
    destPtr[index].used = srcPtr[index].used;
    destPtr[index].id = srcPtr[index].id;
    destPtr[index].val = srcPtr[index].val;
}

PrmDbImpl::t_dbStruct* PrmDbImpl::getDbPtr(PrmDbType dbType) {
    FW_ASSERT(dbType == PrmDbType::DB_ACTIVE or dbType == PrmDbType::DB_STAGING);
    if (dbType == PrmDbType::DB_ACTIVE) {
        return m_activeDb;
    }
    return m_stagingDb;
}

Fw::String PrmDbImpl::getDbString(PrmDbType dbType) {
    FW_ASSERT(dbType == PrmDbType::DB_ACTIVE or dbType == PrmDbType::DB_STAGING);
    if (dbType == PrmDbType::DB_ACTIVE) {
        return Fw::String("ACTIVE");
    }
    return Fw::String("STAGING");
}

}  // namespace Svc
