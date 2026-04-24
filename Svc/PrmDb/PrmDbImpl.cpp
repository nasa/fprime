/*
 * PrmDbImpl.cpp
 *
 *  Created on: March 9, 2015
 *      Author: Timothy Canham
 */

#include <Fw/Types/Assert.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>

#include <Os/File.hpp>
extern "C" {
#include <Utils/Hash/libcrc/lib_crc.h>
}

#include <cstdio>
#include <cstring>

static_assert(std::numeric_limits<FwSizeType>::max() >= PRMDB_NUM_DB_ENTRIES,
              "PRMDB_NUM_DB_ENTRIES must fit within range of FwSizeType");

namespace Svc {

// anonymous namespace for buffer declaration
namespace {
class WorkingBuffer : public Fw::LinearBufferBase {
  public:
    FwSizeType getCapacity() const { return sizeof(m_buff); }

    U8* getBuffAddr() { return m_buff; }

    const U8* getBuffAddr() const { return m_buff; }

  private:
    // Set to max of parameter buffer + id
    U8 m_buff[FW_PARAM_BUFFER_MAX_SIZE + sizeof(FwPrmIdType)];
    static_assert(sizeof(m_buff) >= sizeof(U32), "Size of parameter buffer storage must be >= sizeof(U32)");
};
}  // namespace

//! ----------------------------------------------------------------------
//! Construction, initialization, and destruction
//! ----------------------------------------------------------------------
PrmDbImpl::PrmDbImpl(const char* name) : PrmDbComponentBase(name), m_state(PrmDbFileLoadState::IDLE) {
    this->m_activeDb = &this->m_dbStore1;
    this->m_stagingDb = &this->m_dbStore2;

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
    auto success = this->m_activeDb->find(id, val);

    switch (success.e) {
        case Fw::Success::FAILURE:
            // if unable to find parameter, send error message
            this->log_WARNING_LO_PrmIdNotFound(id);
            return Fw::ParamValid::INVALID;
        case Fw::Success::SUCCESS:
            return Fw::ParamValid::VALID;
        default:
            FW_ASSERT(0, success.e);
            return Fw::ParamValid::INVALID;
    }
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

U32 PrmDbImpl::computeCrc(U32 crc, const BYTE* buff, FwSizeType size) {
    // Note: The crc parameter accepts any U32 value as valid input.
    // This is correct behavior for CRC32 accumulation functions where:
    // - Initial CRC values are typically 0x00000000 or 0xFFFFFFFF
    // - Intermediate CRC values (from prior computeCrc calls) can be any U32 value

    // Check for null pointer before dereferencing
    if (buff == nullptr) {
        // Return the input CRC unchanged if buffer is null
        return crc;
    }

    // Check for zero size to avoid unnecessary processing
    if (size == 0) {
        return crc;
    }
    for (FwSizeType byte = 0; byte < size; byte++) {
        crc = static_cast<U32>(update_crc_32(crc, static_cast<char>(buff[byte])));
    }
    return crc;
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

    // write placeholder for the CRC
    U32 crc = 0xFFFFFFFF;
    buff.resetSer();
    Fw::SerializeStatus serStat = buff.serializeFrom(crc);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, static_cast<FwAssertArgType>(serStat));
    FwSizeType writeSize = static_cast<FwSizeType>(buff.getSize());
    stat = paramFile.write(buff.getBuffAddr(), writeSize, Os::File::WaitType::WAIT);

    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::CRC_PLACE, 0, stat);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    this->lock();
    auto db = getDbPtr(PrmDbType::DB_ACTIVE);
    FW_ASSERT(db != nullptr);

    // Traverse the parameter list, saving each entry

    U32 numRecords = 0;

    for (const auto& entry : *db) {
        // write delimiter
        static const U8 delim = PRMDB_ENTRY_DELIMITER;
        writeSize = static_cast<FwSizeType>(sizeof(delim));
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

        // add delimiter to CRC
        crc = this->computeCrc(crc, &delim, sizeof(delim));

        // serialize record size = id field + data
        U32 recordSize = static_cast<U32>(sizeof(FwPrmIdType) + entry.getValue().getSize());

        // reset buffer
        buff.resetSer();
        serStat = buff.serializeFrom(recordSize);
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

        // add recordSize to CRC
        crc = this->computeCrc(crc, buff.getBuffAddr(), writeSize);

        // reset buffer
        buff.resetSer();

        // serialize parameter id

        serStat = buff.serializeFrom(entry.getKey());
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

        // add parameter ID to CRC
        crc = this->computeCrc(crc, buff.getBuffAddr(), writeSize);

        // write serialized parameter value

        writeSize = static_cast<FwSizeType>(entry.getValue().getSize());
        stat = paramFile.write(entry.getValue().getBuffAddr(), writeSize, Os::File::WaitType::WAIT);
        if (stat != Os::File::OP_OK) {
            this->unLock();
            this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::PARAMETER_VALUE, static_cast<I32>(numRecords), stat);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }
        if (writeSize != static_cast<FwSizeType>(entry.getValue().getSize())) {
            this->unLock();
            this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::PARAMETER_VALUE_SIZE, static_cast<I32>(numRecords),
                                                   static_cast<I32>(writeSize));
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            return;
        }

        // add serialized parameter value to crc
        crc = this->computeCrc(crc, entry.getValue().getBuffAddr(), writeSize);

        numRecords++;
    }

    this->unLock();

    // save current location of pointer in paramFile
    FwSizeType currPosInParamFile;

    stat = paramFile.position(currPosInParamFile);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::CURR_POSITION, 0, stat);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // seek to beginning and write CRC value
    stat = paramFile.seek(0, Os::File::SeekType::ABSOLUTE);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::SEEK_ZERO, 0, stat);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }
    buff.resetSer();
    serStat = buff.serializeFrom(crc);

    FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, static_cast<FwAssertArgType>(serStat));
    writeSize = static_cast<FwSizeType>(buff.getSize());

    stat = paramFile.write(buff.getBuffAddr(), writeSize, Os::File::WaitType::WAIT);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::CRC_REAL, 0, stat);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // Restore pointer to previously saved location
    stat = paramFile.seek(static_cast<FwSignedSizeType>(currPosInParamFile), Os::File::SeekType::ABSOLUTE);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileWriteError(PrmWriteError::SEEK_POSITION, 0, stat);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

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
    PrmDbStore* temp = this->m_activeDb;
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
    //===========================================================================
    // read CRC from beginning of file
    WorkingBuffer buff;
    U32 fileCrc;
    FwSizeType readSize = static_cast<FwSizeType>(sizeof(fileCrc));

    // Read raw CRC bytes from file
    stat = paramFile.read(buff.getBuffAddr(), readSize);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileReadError(PrmReadError::CRC, static_cast<I32>(0), stat);
        return PrmLoadStatus::ERROR;
    }

    if (readSize != sizeof(fileCrc)) {
        this->log_WARNING_HI_PrmFileReadError(PrmReadError::CRC_SIZE, static_cast<I32>(0), static_cast<I32>(readSize));
        return PrmLoadStatus::ERROR;
    }

    // Deserialize the CRC in a portable way
    Fw::SerializeStatus serStat = buff.setBuffLen(readSize);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, static_cast<FwAssertArgType>(serStat));
    buff.resetDeser();
    serStat = buff.deserializeTo(fileCrc);
    FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, static_cast<FwAssertArgType>(serStat));

    U32 crc = 0xFFFFFFFF;
    // read into CRC buffer for checking

    Os::File::Status status = paramFile.calculateCrc(crc);
    if (status != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileReadError(PrmReadError::CRC_BUFFER, static_cast<I32>(0), status);
        return PrmLoadStatus::ERROR;
    }

    if (fileCrc != crc) {
        this->log_WARNING_HI_PrmFileBadCrc(fileCrc, crc);
        return PrmLoadStatus::ERROR;
    }

    // seek back to just after CRC
    stat = paramFile.seek(sizeof(fileCrc), Os::File::SeekType::ABSOLUTE);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_PrmFileReadError(PrmReadError::SEEK_ZERO, 0, stat);
        return PrmLoadStatus::ERROR;
    }
    //===========================================================================

    U32 recordNumTotal = 0;
    U32 recordNumAdded = 0;
    U32 recordNumUpdated = 0;

    for (FwSizeType entry = 0; entry < PRMDB_NUM_DB_ENTRIES; entry++) {
        U8 delimiter;
        readSize = static_cast<FwSizeType>(sizeof(delimiter));

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
    auto* db = getDbPtr(prmDbType);

    PrmUpdateType updateStatus = NO_SLOTS;

    this->lock();

    auto prevSize = db->getSize();
    switch (db->insert(id, val)) {
        case Fw::Success::FAILURE:
            updateStatus = NO_SLOTS;
            break;
        case Fw::Success::SUCCESS:
            if (prevSize < db->getSize()) {
                updateStatus = PARAM_ADDED;
            } else {
                FW_ASSERT(prevSize == db->getSize(), static_cast<FwAssertArgType>(prevSize),
                          static_cast<FwAssertArgType>(db->getSize()));
                updateStatus = PARAM_UPDATED;
            }
            break;
        default:
            FW_ASSERT(false);
    }

    this->unLock();
    return updateStatus;
}

//! ----------------------------------------------------------------------
//! Helpers for database management
//! ----------------------------------------------------------------------

void PrmDbImpl::clearDb(PrmDbType prmDbType) {
    getDbPtr(prmDbType)->clear();
}

void PrmDbImpl::dbCopy(PrmDbType dest, PrmDbType src) {
    *getDbPtr(dest) = *getDbPtr(src);
    this->log_ACTIVITY_HI_PrmDbCopyAllComplete(getDbString(src), getDbString(dest));
}

PrmDbImpl::PrmDbStore* PrmDbImpl::getDbPtr(PrmDbType dbType) {
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
