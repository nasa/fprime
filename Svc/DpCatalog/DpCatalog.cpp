// ======================================================================

// \title  DpCatalog.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component implementation class
// ======================================================================

#include "Svc/DpCatalog/DpCatalog.hpp"
#include "Fw/Dp/DpContainer.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

#include <new>  // placement new
#include "Fw/Types/StringUtils.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {
static_assert(DP_MAX_DIRECTORIES > 0, "Configuration DP_MAX_DIRECTORIES must be positive");
static_assert(DP_MAX_FILES > 0, "Configuration DP_MAX_FILES must be positive");
// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

DpCatalog ::DpCatalog(const char* const compName) : DpCatalogComponentBase(compName) {
    // Members are default-initialized via in-class initializers in the header.
}

void DpCatalog::configure(Fw::FileNameString directories[DP_MAX_DIRECTORIES],
                          FwSizeType numDirs,
                          Fw::FileNameString& stateFile,
                          FwEnumStoreType memId,
                          Fw::MemAllocator& allocator) {
    // Do some assertion checks
    FW_ASSERT(numDirs <= DP_MAX_DIRECTORIES, static_cast<FwAssertArgType>(numDirs));

    this->m_stateFile = stateFile;

    // Request memory for state file data storage.
    // RedBlackTreeSet storage is allocated as a member variable, so we only need
    // to allocate memory for the state file tracking array.
    static const FwSizeType slotSize = sizeof(DpDstateFileEntry);
    this->m_memSize = DP_MAX_FILES * slotSize;
    bool notUsed;  // we don't need to recover the catalog.
    // request memory. this->m_memSize will be modified if there is less than we requested
    this->m_memPtr = allocator.allocate(memId, this->m_memSize, notUsed);

    // Initialize if there is enough room for at least one record and memory was allocated
    if ((this->m_memSize >= slotSize) and (this->m_memPtr != nullptr)) {
        // set the number of available record slots based on how much memory we actually got
        this->m_numDpSlots = this->m_memSize / slotSize;
        // Initialize the catalog
        this->resetCatalog();
        // assign pointer for the state file storage
        this->m_stateFileData = static_cast<DpDstateFileEntry*>(this->m_memPtr);
    } else {
        // if we don't have enough memory, set the number of records
        // to zero for later detection
        this->m_numDpSlots = 0;
    }

    // assign directory names
    for (FwSizeType dir = 0; dir < numDirs; dir++) {
        this->m_directories[dir] = directories[dir];
    }
    this->m_numDirectories = numDirs;

    // store allocator
    this->m_allocator = &allocator;
    this->m_allocatorId = memId;
    this->m_initialized = true;
}

void DpCatalog::resetCatalog() {
    // Clear the catalog
    this->m_dpCatalog.clear();
    // Clear transmission state
    this->m_hasCurrentXmit = false;
    // Reset counters
    this->m_pendingFiles = 0;
    this->m_pendingDpBytes = 0;
    // Mark the catalog as un-built
    this->m_catalogBuilt = false;
}

void DpCatalog::resetStateFileData() {
    // clear state file data
    for (FwSizeType slot = 0; slot < this->m_numDpSlots; slot++) {
        this->m_stateFileData[slot].used = false;
        this->m_stateFileData[slot].visited = false;
        (void)new (&this->m_stateFileData[slot].entry.record) DpRecord();
    }
    this->m_stateFileEntries = 0;
}

Fw::CmdResponse DpCatalog::loadStateFile() {
    FW_ASSERT(this->m_stateFileData);

    // Make sure that a file was specified
    if (this->m_stateFile.length() == 0) {
        this->log_WARNING_LO_NoStateFileSpecified();
        return Fw::CmdResponse::OK;
    }

    // buffer for reading entries

    BYTE buffer[sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));

    // open the state file
    Os::File stateFile;
    Os::File::Status stat = stateFile.open(this->m_stateFile.toChar(), Os::File::OPEN_READ);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_StateFileOpenError(this->m_stateFile, stat);
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    FwSizeType fileLoc = 0;
    this->m_stateFileEntries = 0;

    // read entries from the state file
    for (FwSizeType entry = 0; entry < this->m_numDpSlots; entry++) {
        FwSizeType size = static_cast<FwSizeType>(sizeof(buffer));
        // read the directory index
        stat = stateFile.read(buffer, size);
        if (stat != Os::File::OP_OK) {
            this->log_WARNING_HI_StateFileReadError(this->m_stateFile, stat, static_cast<I32>(fileLoc));
            stateFile.close();
            return Fw::CmdResponse::EXECUTION_ERROR;
        }

        if (0 == size) {
            // no more entries
            break;
        }

        // check to see if the full entry was read. If not,
        // abandon it and finish. We can at least operate on
        // the entries that were read.
        if (size != sizeof(buffer)) {
            this->log_WARNING_HI_StateFileTruncated(this->m_stateFile, static_cast<I32>(fileLoc),
                                                    static_cast<I32>(size));
            stateFile.close();
            return Fw::CmdResponse::OK;
        }

        // reset the buffer for deserializing the entry
        Fw::SerializeStatus serStat = entryBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(size));
        // should always fit
        FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, serStat);
        entryBuffer.resetDeser();

        // deserialization after this point should always work, since
        // the source buffer was specifically sized to hold the data

        // Deserialize the file directory index. If an error occurs processing the file,
        // generate event and return EXECUTION_ERROR.
        Fw::SerializeStatus status = entryBuffer.deserializeTo(this->m_stateFileData[entry].entry.dir);
        if (status != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileCorruptedDataError(this->m_stateFile, static_cast<I32>(status));
            stateFile.close();
            return Fw::CmdResponse::EXECUTION_ERROR;
        }
        status = entryBuffer.deserializeTo(this->m_stateFileData[entry].entry.record);
        if (status != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileCorruptedDataError(this->m_stateFile, static_cast<I32>(status));
            stateFile.close();
            return Fw::CmdResponse::EXECUTION_ERROR;
        }
        this->m_stateFileData[entry].used = true;
        this->m_stateFileData[entry].visited = false;

        // increment the file location
        fileLoc += size;
        this->m_stateFileEntries++;
    }
    stateFile.close();
    return Fw::CmdResponse::OK;
}

void DpCatalog::getFileState(DpStateEntry& entry) {
    FW_ASSERT(this->m_stateFileData);
    // search the file state data for the entry
    for (FwSizeType line = 0; line < this->m_stateFileEntries; line++) {
        // check for a match (compare dir, then id, priority, & time)
        if (this->m_stateFileData[line].entry.dir == entry.dir && this->m_stateFileData[line].entry == entry) {
            // update the transmitted state
            entry.record.set_state(this->m_stateFileData[line].entry.record.get_state());
            entry.record.set_blocks(this->m_stateFileData[line].entry.record.get_blocks());
            // mark it as visited for later pruning if necessary
            this->m_stateFileData[line].visited = true;
            return;
        }
    }
}

void DpCatalog::pruneAndWriteStateFile() {
    FW_ASSERT(this->m_stateFileData);

    // There is a chance that a data product file can disappear after
    // the state file is written from the last catalog build and transmit.
    // This function will walk the state file data and write back only
    // the entries that were visited during the last catalog build. This will
    // remove any entries that are no longer valid.

    // open the state file
    Os::File stateFile;
    // we open it as a new file so we don't accumulate invalid entries
    Os::File::Status stat =
        stateFile.open(this->m_stateFile.toChar(), Os::File::OPEN_CREATE, Os::FileInterface::OVERWRITE);

    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_StateFileOpenError(this->m_stateFile, stat);
        return;
    }

    // buffer for writing entries
    BYTE buffer[sizeof(FwIndexType) + DpRecord::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));

    // write entries to the state file
    for (FwSizeType entry = 0; entry < this->m_numDpSlots; entry++) {
        // only write entries that were used
        if ((this->m_stateFileData[entry].used) and (this->m_stateFileData[entry].visited)) {
            // reset the buffer for serializing the entry
            entryBuffer.resetSer();
            // serialize the file directory index
            Fw::SerializeStatus serStat = entryBuffer.serializeFrom(this->m_stateFileData[entry].entry.dir);
            // Should always fit
            FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, serStat);
            serStat = entryBuffer.serializeFrom(this->m_stateFileData[entry].entry.record);
            // Should always fit
            FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, serStat);
            // write the entry
            FwSizeType size = entryBuffer.getSize();
            // Protect against overflow
            stat = stateFile.write(buffer, size);
            if (stat != Os::File::OP_OK) {
                this->log_WARNING_HI_StateFileWriteError(this->m_stateFile, stat);
                stateFile.close();
                return;
            }
        }
    }

    // close the state file
    stateFile.close();
}

void DpCatalog::appendFileState(const DpStateEntry& entry) {
    FW_ASSERT(this->m_stateFileData);
    FW_ASSERT(entry.dir < static_cast<FwIndexType>(this->m_numDirectories), static_cast<FwAssertArgType>(entry.dir),
              static_cast<FwAssertArgType>(this->m_numDirectories));

    // We will append state to the existing state file
    // TODO: Have to handle case where state file has partially transmitted
    // state already

    // open the state file
    Os::File stateFile;
    // we open it as a new file so we don't accumulate invalid entries
    Os::File::Status stat = stateFile.open(this->m_stateFile.toChar(), Os::File::OPEN_APPEND);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_StateFileOpenError(this->m_stateFile, stat);
        return;
    }

    // buffer for writing entries
    BYTE buffer[sizeof(entry.dir) + sizeof(entry.record)];
    Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));
    // reset the buffer for serializing the entry
    entryBuffer.resetSer();
    // serialize the file directory index
    Fw::SerializeStatus serStat = entryBuffer.serializeFrom(entry.dir);
    // should fit
    FW_ASSERT(serStat == Fw::FW_SERIALIZE_OK, serStat);
    serStat = entryBuffer.serializeFrom(entry.record);
    // should fit
    FW_ASSERT(serStat == Fw::FW_SERIALIZE_OK, serStat);
    // write the entry
    FwSizeType size = entryBuffer.getSize();
    stat = stateFile.write(buffer, size);
    if (stat != Os::File::OP_OK) {
        stateFile.close();
        this->log_WARNING_HI_StateFileWriteError(this->m_stateFile, stat);
        return;
    }

    // close the state file
    stateFile.close();
}

Fw::CmdResponse DpCatalog::doCatalogBuild() {
    // check initialization
    if (not this->checkInit()) {
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // check that initialization got memory
    if (0 == this->m_numDpSlots) {
        this->log_WARNING_HI_NoDpMemory();
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // make sure a downlink is not in progress
    if (this->m_xmitInProgress) {
        this->log_WARNING_LO_DpXmitInProgress();
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // reset state file data
    this->resetStateFileData();

    // load state data from file
    Fw::CmdResponse response = this->loadStateFile();

    // reset catalog
    this->resetCatalog();

    // fill the catalog with DP files
    response = this->fillBinaryTree();
    if (response != Fw::CmdResponse::OK) {
        // clean up the catalog
        this->resetCatalog();
        this->resetStateFileData();
        return response;
    }

    // prune and rewrite the state file
    this->pruneAndWriteStateFile();

    this->log_ACTIVITY_HI_CatalogBuildComplete();

    // Flag so addToCat knows it is good to go
    this->m_catalogBuilt = true;

    return Fw::CmdResponse::OK;
}

Fw::CmdResponse DpCatalog::fillBinaryTree() {
    // keep cumulative number of files
    FwSizeType totalFiles = 0;

    // get file listings from file system
    // double bounds to appease static analysis
    for (FwSizeType dir = 0; dir < this->m_numDirectories && dir < static_cast<FwSizeType>(DP_MAX_DIRECTORIES); dir++) {
        // read in each directory and keep track of total
        this->log_ACTIVITY_LO_ProcessingDirectory(this->m_directories[dir]);
        FwSizeType filesRead = 0;
        U32 filesProcessed = 0;

        Os::Directory dpDir;
        Os::Directory::Status status = dpDir.open(this->m_directories[dir].toChar(), Os::Directory::OpenMode::READ);
        if (status != Os::Directory::OP_OK) {
            this->log_WARNING_HI_DirectoryOpenError(this->m_directories[dir], status);
            return Fw::CmdResponse::EXECUTION_ERROR;
        }
        status = dpDir.readDirectory(this->m_fileList, (this->m_numDpSlots - totalFiles), filesRead);

        if (status != Os::Directory::OP_OK) {
            this->log_WARNING_HI_DirectoryOpenError(this->m_directories[dir], status);
            return Fw::CmdResponse::EXECUTION_ERROR;
        }

        // Assert number of files isn't more than asked
        FW_ASSERT(filesRead <= this->m_numDpSlots - totalFiles, static_cast<FwAssertArgType>(filesRead),
                  static_cast<FwAssertArgType>(this->m_numDpSlots - totalFiles));

        // extract metadata for each file
        for (FwSizeType file = 0; file < filesRead; file++) {
            // only consider files with the DP extension

            const FwSizeType fileNameLength = this->m_fileList[file].length();
            const FwSizeType dpExtLength = Fw::StringUtils::string_length(DP_EXT, sizeof(DP_EXT));
            const FwSignedSizeType loc = Fw::StringUtils::substring_find_last(this->m_fileList[file].toChar(),
                                                                              fileNameLength, DP_EXT, dpExtLength);

            // Only accept files whose final suffix is the data product extension
            if ((-1 == loc) || (static_cast<FwSizeType>(loc) + dpExtLength != fileNameLength)) {
                continue;
            }

            Fw::String fullFile;
            fullFile.format("%s/%s", this->m_directories[dir].toChar(), this->m_fileList[file].toChar());

            int ret = processFile(fullFile, dir);
            if (ret < 0) {
                break;
            }

            filesProcessed += static_cast<U32>(ret);

        }  // end for each file in a directory

        totalFiles += filesProcessed;

        this->log_ACTIVITY_HI_ProcessingDirectoryComplete(this->m_directories[dir], static_cast<U32>(totalFiles),
                                                          this->m_pendingFiles, this->m_pendingDpBytes);

        // check to see if catalog is full
        // that means generated products exceed the catalog size
        if (totalFiles == this->m_numDpSlots) {
            this->log_WARNING_HI_CatalogFull(this->m_directories[dir]);
            break;
        }
    }  // end for each directory

    return Fw::CmdResponse::OK;

}  // end fillBinaryTree()

FwSizeType DpCatalog::determineDirectory(Fw::String fullFile) {
    // Grab the directory string (up until the final slash)
    // Could be found directly w/ a dirname func or regex
    FwSignedSizeType loc = Fw::StringUtils::substring_find_last(
        fullFile.toChar(), fullFile.length(), DIRECTORY_DELIMITER,
        Fw::StringUtils::string_length(DIRECTORY_DELIMITER, sizeof(DIRECTORY_DELIMITER)));

    // Seems like the logic works so long as the path styles match (i.e. relative vs absolute)
    // Full path resolution might be a worthwhile add

    // No directory delimiter found; return DP_MAX_DIRECTORIES to signal failure
    if (-1 == loc) {
        return DP_MAX_DIRECTORIES;
    }

    for (FwSizeType dir = 0; dir < this->m_numDirectories; dir++) {
        const Fw::FileNameString& dir_string = this->m_directories[dir];

        // Compare both strings up to location of final slash
        // StringUtils::substring_find will return zero if both paths agree
        // memory safe since both are fixed width strings
        // and loc is before the fixed width
        if (Fw::StringUtils::substring_find(dir_string.toChar(), dir_string.length(), fullFile.toChar(),
                                            static_cast<FwSizeType>(loc)) == 0) {
            return dir;
        }
    }

    // No directory matched
    return DP_MAX_DIRECTORIES;
}

int DpCatalog::processFile(Fw::String fullFile, FwSizeType dir) {
    FW_ASSERT(dir < static_cast<FwSizeType>(DP_MAX_DIRECTORIES), static_cast<FwAssertArgType>(dir));
    // file class instance for processing files
    Os::File dpFile;

    // Working buffer for DP headers
    U8 dpBuff[Fw::DpContainer::MIN_PACKET_SIZE];  // Header buffer
    Fw::Buffer hdrBuff(dpBuff, sizeof(dpBuff));   // buffer for container header decoding
    Fw::DpContainer container;                    // container object for extracting header fields

    this->log_ACTIVITY_LO_ProcessingFile(fullFile);

    // get file size
    FwSizeType fileSize = 0;
    Os::FileSystem::Status sizeStat = Os::FileSystem::getFileSize(fullFile.toChar(), fileSize);
    if (sizeStat != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_FileSizeError(fullFile, sizeStat);
        return 0;
    }

    if (fileSize < Fw::DpContainer::MIN_PACKET_SIZE) {
        this->log_WARNING_HI_FileReadError(fullFile, Os::File::BAD_SIZE);
        return 0;
    }

    Os::File::Status stat = dpFile.open(fullFile.toChar(), Os::File::OPEN_READ);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_FileOpenError(fullFile, stat);
        return 0;
    }

    // Read DP header and header hash
    FwSizeType size = Fw::DpContainer::MIN_PACKET_SIZE;

    stat = dpFile.read(dpBuff, size);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(fullFile, stat);
        dpFile.close();
        return 0;
    }

    // if full header and hashes aren't read, something's wrong with the file, so skip
    if (size != Fw::DpContainer::MIN_PACKET_SIZE) {
        this->log_WARNING_HI_FileReadError(fullFile, Os::File::BAD_SIZE);
        dpFile.close();
        return 0;
    }

    // if all is well, don't need the file any more
    dpFile.close();

    // give buffer to container instance
    container.setBuffer(hdrBuff);

    // make sure the header metadata matches its stored hash before trusting it
    Utils::HashBuffer storedHash;
    Utils::HashBuffer computedHash;
    Fw::Success::T hashStatus = container.checkHeaderHash(storedHash, computedHash);
    if (hashStatus != Fw::Success::SUCCESS) {
        this->log_WARNING_HI_FileHdrError(fullFile, DpHdrField::CRC, computedHash.asBigEndianU32(),
                                          storedHash.asBigEndianU32());
        return 0;
    }

    // reset header deserialization in the container
    Fw::SerializeStatus desStat = container.deserializeHeader();
    if (desStat != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileHdrDesError(fullFile, desStat);
        return 0;
    }

    const FwSizeType dataSize = container.getDataSize();
    const FwSizeType expectedDataSize = fileSize - Fw::DpContainer::MIN_PACKET_SIZE;
    if (dataSize != expectedDataSize) {
        this->log_WARNING_HI_FileReadError(fullFile, Os::File::BAD_SIZE);
        return 0;
    }

    Fw::FileNameString canonicalFileName;
    canonicalFileName.format(DP_FILENAME_FORMAT, this->m_directories[dir].toChar(), container.getId(),
                             container.getTimeTag().getSeconds(), container.getTimeTag().getUSeconds());
    if (canonicalFileName != fullFile) {
        this->log_WARNING_HI_InvalidFileName(fullFile, canonicalFileName);
        return 0;
    }

    // skip adding an already transmitted file
    if (container.getState() == Fw::DpState::TRANSMITTED) {
        this->log_ACTIVITY_HI_DpFileSkipped(fullFile);
        return 0;
    }

    // add entry to catalog.
    DpStateEntry entry;
    entry.dir = static_cast<FwIndexType>(dir);
    entry.record.set_id(container.getId());
    entry.record.set_priority(container.getPriority());
    entry.record.set_state(container.getState());
    entry.record.set_tSec(container.getTimeTag().getSeconds());
    entry.record.set_tSub(container.getTimeTag().getUSeconds());
    entry.record.set_size(static_cast<U64>(fileSize));

    // check the state file to see if there is transmit state
    this->getFileState(entry);

    // insert entry into sorted catalog. if can't insert, quit
    bool inserted = this->insertEntry(entry);
    if (!inserted) {
        this->log_WARNING_HI_DpInsertError(entry.record);
        // return and hope new slots open up later
        return -1;
    }

    // increment our counters
    this->m_pendingFiles++;
    this->m_pendingDpBytes += entry.record.get_size();

    // make sure we haven't exceeded the limit
    if (this->m_pendingFiles > this->m_numDpSlots) {
        this->log_WARNING_HI_DpCatalogFull(entry.record);
        return -1;
    }

    this->log_ACTIVITY_HI_DpFileAdded(canonicalFileName);

    // No need to track iterator state - begin() always gives us the highest priority entry
    // and we remove entries as we transmit them

    return 1;
}

// ----------------------------------------------------------------------
// DpStateEntry Comparison Ops
// ----------------------------------------------------------------------
int DpCatalog::DpStateEntry::compareEntries(const DpStateEntry& left, const DpStateEntry& right) {
    // check priority. Lower is higher priority
    if (left.record.get_priority() < right.record.get_priority()) {
        return -1;
    } else if (left.record.get_priority() > right.record.get_priority()) {
        return 1;
    }

    // check time. Older is higher priority
    else if (left.record.get_tSec() < right.record.get_tSec()) {
        return -1;
    } else if (left.record.get_tSec() > right.record.get_tSec()) {
        return 1;
    }

    // check subsecond time. Older is higher priority
    else if (left.record.get_tSub() < right.record.get_tSub()) {
        return -1;
    } else if (left.record.get_tSub() > right.record.get_tSub()) {
        return 1;
    }

    // check ID. Lower is higher priority
    else if (left.record.get_id() < right.record.get_id()) {
        return -1;
    } else if (left.record.get_id() > right.record.get_id()) {
        return 1;
    }

    // if ids are equal we have two nodes with the same value
    else {
        return 0;
    }
}

bool DpCatalog::DpStateEntry::operator==(const DpStateEntry& other) const {
    return compareEntries(*this, other) == 0;
}
bool DpCatalog::DpStateEntry::operator!=(const DpStateEntry& other) const {
    return compareEntries(*this, other) != 0;
}

bool DpCatalog::DpStateEntry::operator>(const DpStateEntry& other) const {
    return compareEntries(*this, other) > 0;
}
bool DpCatalog::DpStateEntry::operator<(const DpStateEntry& other) const {
    return compareEntries(*this, other) < 0;
}

bool DpCatalog::insertEntry(DpStateEntry& entry) {
    // Insert into the RedBlackTreeSet
    // The tree maintains sorting by priority, time, and ID via DpStateEntry comparison operators
    Fw::Success status = this->m_dpCatalog.insert(entry);
    return (status == Fw::Success::SUCCESS);
}

void DpCatalog::sendNextEntry() {
    // Use xmit flag to break upon STOP_XMIT_CATALOG
    if (this->m_xmitInProgress != true) {
        return;
    }

    // Look for the next entry to send
    DpStateEntry entry;
    if (!this->findNextEntry(entry)) {
        // if no entry found, we are done
        this->m_xmitInProgress = false;
        this->log_ACTIVITY_HI_CatalogXmitCompleted(this->m_xmitBytes);
        this->dispatchWaitedResponse(Fw::CmdResponse::OK);
        return;
    }

    // Save current entry for fileDone_handler
    this->m_currentXmitEntry = entry;
    this->m_hasCurrentXmit = true;

    // Build file name based on the found entry
    this->m_currXmitFileName.format(DP_FILENAME_FORMAT, this->m_directories[entry.dir].toChar(), entry.record.get_id(),
                                    entry.record.get_tSec(), entry.record.get_tSub());
    this->log_ACTIVITY_LO_SendingProduct(this->m_currXmitFileName, static_cast<U32>(entry.record.get_size()),
                                         entry.record.get_priority());
    Svc::SendFileResponse resp = this->fileOut_out(0, this->m_currXmitFileName, this->m_currXmitFileName, 0, 0);
    if (resp.get_status() != Svc::SendFileStatus::STATUS_OK) {
        // warn, but keep going since it may be an issue with this file but others could
        // make it
        this->log_WARNING_HI_DpFileSendError(this->m_currXmitFileName, resp.get_status());
    }
}  // end sendNextEntry()

bool DpCatalog::findNextEntry(DpStateEntry& entry) {
    // If catalog is empty, return false
    if (this->m_dpCatalog.getSize() == 0) {
        return false;
    }

    // Get the highest priority entry (begin() returns highest priority)
    // Since we remove entries as we transmit them, begin() always gives us the next entry
    typename Fw::RedBlackTreeSet<DpStateEntry, DP_MAX_FILES>::ConstIterator iter = this->m_dpCatalog.begin();

    // Verify iterator is valid
    if (iter == this->m_dpCatalog.end()) {
        return false;
    }

    // Get the entry
    entry = *iter;

    return true;
}

bool DpCatalog::checkInit() {
    if (not this->m_initialized) {
        this->log_WARNING_HI_ComponentNotInitialized();
        return false;
    } else if (0 == this->m_numDpSlots) {
        this->log_WARNING_HI_ComponentNoMemory();
        return false;
    }

    return true;
}

void DpCatalog::shutdown() {
    // only try to deallocate if both pointers are non-zero
    // it's a way to more gracefully shut down if there are missing
    // pointers
    if ((this->m_allocator != nullptr) and (this->m_memPtr != nullptr)) {
        this->m_allocator->deallocate(this->m_allocatorId, this->m_memPtr);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DpCatalog ::fileDone_handler(FwIndexType portNum, const Svc::SendFileResponse& resp) {
    // check file status
    if (resp.get_status() != Svc::SendFileStatus::STATUS_OK) {
        this->log_WARNING_HI_DpFileXmitError(this->m_currXmitFileName, resp.get_status());
        this->m_xmitInProgress = false;
        this->dispatchWaitedResponse(Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // Catalog cleared while this file was sent
    if (!this->m_catalogBuilt) {
        return;
    }

    // Should have a valid current transmit entry
    FW_ASSERT(this->m_hasCurrentXmit);

    // Reduce pending
    this->m_pendingDpBytes -= this->m_currentXmitEntry.record.get_size();
    this->m_pendingFiles--;
    // Log File Complete & pending
    this->log_ACTIVITY_LO_ProductComplete(this->m_currXmitFileName, this->m_pendingFiles, this->m_pendingDpBytes);

    // mark the entry as transmitted
    this->m_currentXmitEntry.record.set_state(Fw::DpState::TRANSMITTED);
    // update the transmitted state in the state file
    this->appendFileState(this->m_currentXmitEntry);
    // add the size
    this->m_xmitBytes += this->m_currentXmitEntry.record.get_size();

    // Remove from catalog
    Fw::Success status = this->m_dpCatalog.remove(this->m_currentXmitEntry);
    FW_ASSERT(status == Fw::Success::SUCCESS);

    this->m_hasCurrentXmit = false;

    // send the next entry, if it exists
    this->sendNextEntry();
}

void DpCatalog ::pingIn_handler(FwIndexType portNum, U32 key) {
    // return code for health ping
    this->pingOut_out(0, key);
}

void DpCatalog ::addToCat_handler(FwIndexType portNum,
                                  const Fw::StringBase& fileName,
                                  FwDpPriorityType priority,
                                  FwSizeType size) {
    // check initialization
    if (not this->checkInit()) {
        return;
    }

    // check that initialization got memory
    if (0 == this->m_numDpSlots) {
        this->log_WARNING_HI_NoDpMemory();
        return;
    }

    // Check the catalog has been built
    if (not this->m_catalogBuilt) {
        this->log_ACTIVITY_HI_NotLoaded(fileName);
        return;
    }

    // Both of these are grabbed from the header
    (void)priority;
    (void)size;

    // Since this is a runtime addition
    // Check if file is in one of our directories
    FwSizeType dir = this->determineDirectory(fileName);

    // Not in one of our directories; skip this file
    if (dir == DP_MAX_DIRECTORIES) {
        this->log_WARNING_HI_DirectoryNotManaged(fileName);
        return;
    }

    // ret > 0 := success
    int ret = processFile(fileName, dir);

    if (ret > 0) {
        // If we already finished, sendNext only if remainingActive
        if (!this->m_xmitInProgress && this->m_remainActive) {
            this->m_xmitInProgress = true;
            this->sendNextEntry();
        }
        // Otherwise, Current File finishing will invoke sendNextFile & find the right file
        // Or will be manually tx-ed at next command

        // prune and rewrite the state file
        this->pruneAndWriteStateFile();
    }
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void DpCatalog ::BUILD_CATALOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // invoke helper
    this->cmdResponse_out(opCode, cmdSeq, this->doCatalogBuild());
}

void DpCatalog ::START_XMIT_CATALOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, Fw::Wait wait, bool remainActive) {
    Fw::CmdResponse resp = this->doCatalogXmit();
    this->m_remainActive = remainActive;

    if (resp != Fw::CmdResponse::OK) {
        this->cmdResponse_out(opCode, cmdSeq, resp);
    } else {
        if (Fw::Wait::NO_WAIT == wait) {
            this->cmdResponse_out(opCode, cmdSeq, resp);
            this->m_xmitCmdWait = false;
            this->m_xmitOpCode = 0;
            this->m_xmitCmdSeq = 0;
        } else {
            this->m_xmitCmdWait = true;
            this->m_xmitOpCode = opCode;
            this->m_xmitCmdSeq = cmdSeq;
        }
    }
}

Fw::CmdResponse DpCatalog::doCatalogXmit() {
    // check initialization
    if (not this->checkInit()) {
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // check that initialization got memory
    if (0 == this->m_numDpSlots) {
        this->log_WARNING_HI_NoDpMemory();
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // make sure a downlink is not in progress
    if (this->m_xmitInProgress) {
        this->log_WARNING_LO_DpXmitInProgress();
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // Check the catalog has been built
    if (not this->m_catalogBuilt) {
        this->log_WARNING_HI_XmitUnbuiltCatalog();
        return Fw::CmdResponse::EXECUTION_ERROR;
    }

    // start transmission
    this->m_xmitBytes = 0;

    this->m_xmitInProgress = true;
    // Step 3b - search for and send first entry
    this->sendNextEntry();
    return Fw::CmdResponse::OK;
}

void DpCatalog ::STOP_XMIT_CATALOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    if (not this->m_xmitInProgress) {
        this->log_WARNING_LO_XmitNotActive();
        // benign error, so don't fail the command
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_ACTIVITY_HI_CatalogXmitStopped(this->m_xmitBytes);
        // Disarm the flag so next sendNextEntry stops transmission
        this->m_xmitInProgress = false;
        // Respond to original cmd to start xmit
        // (if we haven't already)
        this->dispatchWaitedResponse(Fw::CmdResponse::OK);

        // Respond to this command
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
}

void DpCatalog ::CLEAR_CATALOG_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->resetCatalog();
    this->resetStateFileData();

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void DpCatalog ::dispatchWaitedResponse(Fw::CmdResponse response) {
    if (this->m_xmitCmdWait) {
        this->cmdResponse_out(this->m_xmitOpCode, this->m_xmitCmdSeq, response);

        // Prevent a Duplicate Cmd Response
        this->m_xmitCmdWait = false;
        this->m_xmitOpCode = 0;
        this->m_xmitCmdSeq = 0;
    }
}

}  // namespace Svc
