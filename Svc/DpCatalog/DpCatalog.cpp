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

namespace Svc {
static_assert(DP_MAX_DIRECTORIES > 0, "Configuration DP_MAX_DIRECTORIES must be positive");
static_assert(DP_MAX_FILES > 0, "Configuration DP_MAX_FILES must be positive");
// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

DpCatalog ::DpCatalog(const char* const compName)
    : DpCatalogComponentBase(compName),
      m_initialized(false),
      m_dpTree(nullptr),
      m_freeListHead(nullptr),
      m_currentNode(nullptr),
      m_currentXmitNode(nullptr),
      m_numDpSlots(0),
      m_numDirectories(0),
      m_stateFileData(nullptr),
      m_stateFileEntries(0),
      m_memSize(0),
      m_memPtr(nullptr),
      m_allocatorId(0),
      m_allocator(nullptr),
      m_catalogBuilt(false),
      m_xmitInProgress(false),
      m_xmitCmdWait(false),
      m_xmitBytes(0),
      m_xmitOpCode(0),
      m_xmitCmdSeq(0),
      m_pendingFiles(0),
      m_pendingDpBytes(0),
      m_remainActive(false) {}

DpCatalog ::~DpCatalog() {}

void DpCatalog::configure(Fw::FileNameString directories[DP_MAX_DIRECTORIES],
                          FwSizeType numDirs,
                          Fw::FileNameString& stateFile,
                          FwEnumStoreType memId,
                          Fw::MemAllocator& allocator) {
    // Do some assertion checks
    FW_ASSERT(numDirs <= DP_MAX_DIRECTORIES, static_cast<FwAssertArgType>(numDirs));

    this->m_stateFile = stateFile;

    // request memory for catalog which is DP_MAX_FILES * slot size.
    //
    // A "slot" consists of a set of two memory locations for each data product consisting
    // an entry in the binary tree and
    // an entry in the state file data. These may not be fully used in a given
    // situation based on the number of actual data products, but this provides room for the
    // maximum possible.
    static const FwSizeType slotSize = sizeof(DpBtreeNode) + sizeof(DpDstateFileEntry);
    this->m_memSize = DP_MAX_FILES * slotSize;
    bool notUsed;  // we don't need to recover the catalog.
    // request memory. this->m_memSize will be modified if there is less than we requested
    this->m_memPtr = allocator.allocate(memId, this->m_memSize, notUsed);
    // adjust to actual size if less allocated and only initialize
    // if there is enough room for at least one record and memory
    // was allocated.

    // Since we are given a monolithic block of memory, the data structures
    // are interspersed in the memory using the following method:
    //
    // 1) Recompute how many slots can fit in the provided memory if we
    // don't get the full amount requested. This allows for graceful degradation
    // if there are memory issues.
    //
    // 2) Place the binary tree free list at the beginning of the memory.
    //
    // 3) Place the state file data in memory after the binary free list
    // by indexing the free list to one element past the end of
    // the free list.

    if ((this->m_memSize >= slotSize) and (this->m_memPtr != nullptr)) {
        // set the number of available record slots based on how much memory we actually got
        this->m_numDpSlots = this->m_memSize / slotSize;  // Step 1.
        this->resetBinaryTree();                          // Step 2
        // assign pointer for the state file storage - Step 3
        this->m_stateFileData = reinterpret_cast<DpDstateFileEntry*>(&this->m_freeListHead[this->m_numDpSlots]);
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

void DpCatalog::resetBinaryTree() {
    // initialize data structures in the free list
    // Step 2 in memory partition (see configure() comments)
    FW_ASSERT(this->m_memPtr);
    this->m_freeListHead = static_cast<DpBtreeNode*>(this->m_memPtr);
    for (FwSizeType slot = 0; slot < this->m_numDpSlots; slot++) {
        // overlay new instance of the DpState entry on the memory
        (void)new (&this->m_freeListHead[slot]) DpBtreeNode();
        this->m_freeListHead[slot].left = nullptr;
        this->m_freeListHead[slot].right = nullptr;
        // link the free list
        if (slot > 0) {
            this->m_freeListHead[slot - 1].left = &this->m_freeListHead[slot];
        }
    }
    // clear binary tree
    this->m_dpTree = nullptr;
    // reset number of records
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
            return Fw::CmdResponse::OK;
        }

        // reset the buffer for deserializing the entry
        Fw::SerializeStatus serStat = entryBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(size));
        // should always fit
        FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat, serStat);
        entryBuffer.resetDeser();

        // deserialization after this point should always work, since
        // the source buffer was specifically sized to hold the data

        // Deserialize the file directory index
        Fw::SerializeStatus status = entryBuffer.deserializeTo(this->m_stateFileData[entry].entry.dir);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status, status);
        status = entryBuffer.deserializeTo(this->m_stateFileData[entry].entry.record);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status, status);
        this->m_stateFileData[entry].used = true;
        this->m_stateFileData[entry].visited = false;

        // increment the file location
        fileLoc += size;
        this->m_stateFileEntries++;
    }

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

    // reset free list for entries
    this->resetBinaryTree();

    // fill the binary tree with DP files
    response = this->fillBinaryTree();
    if (response != Fw::CmdResponse::OK) {
        // clean up the binary tree
        this->resetBinaryTree();
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
    for (FwSizeType dir = 0; dir < this->m_numDirectories; dir++) {
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

            FwSignedSizeType loc =
                Fw::StringUtils::substring_find(this->m_fileList[file].toChar(), this->m_fileList[file].length(),
                                                DP_EXT, Fw::StringUtils::string_length(DP_EXT, sizeof(DP_EXT)));

            if (-1 == loc) {
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

    Os::File::Status stat = dpFile.open(fullFile.toChar(), Os::File::OPEN_READ);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_FileOpenError(fullFile, stat);
        return 0;
    }

    // Read DP header
    FwSizeType size = Fw::DpContainer::Header::SIZE;

    stat = dpFile.read(dpBuff, size);
    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(fullFile, stat);
        dpFile.close();
        return 0;
    }

    // if full header isn't read, something's wrong with the file, so skip
    if (size != Fw::DpContainer::Header::SIZE) {
        this->log_WARNING_HI_FileReadError(fullFile, Os::File::BAD_SIZE);
        dpFile.close();
        return 0;
    }

    // if all is well, don't need the file any more
    dpFile.close();

    // give buffer to container instance
    container.setBuffer(hdrBuff);

    // reset header deserialization in the container
    Fw::SerializeStatus desStat = container.deserializeHeader();
    if (desStat != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileHdrDesError(fullFile, desStat);
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

    // insert entry into sorted list. if can't insert, quit
    DpBtreeNode* addedEntry = this->insertEntry(entry);
    if (addedEntry == nullptr) {
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

    Fw::FileNameString addedFileName;
    addedFileName.format(DP_FILENAME_FORMAT, this->m_directories[dir].toChar(), entry.record.get_id(),
                         entry.record.get_tSec(), entry.record.get_tSub());

    this->log_ACTIVITY_HI_DpFileAdded(addedFileName);

    // Compute relative priority to current exploration node
    // For Handling adding a node to a catalog that has
    // already moved past the inserted node's priority
    if (this->m_currentNode == nullptr) {
        this->m_currentNode = addedEntry;
    } else if (entry < this->m_currentNode->entry) {
        this->m_currentNode = addedEntry;
    }

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

DpCatalog::DpBtreeNode* DpCatalog::insertEntry(DpStateEntry& entry) {
    // the tree is filled in the following priority order:
    // 1. DP priority - lower number is higher priority
    // 2. DP time - older is higher priority
    // 3. DP ID - lower number is higher priority

    // Higher priority is to the left of the tree

    // if the tree is empty, add the first entry
    if (this->m_dpTree == nullptr) {
        bool goodInsert = this->allocateNode(this->m_dpTree, entry);
        if (not goodInsert) {
            return nullptr;
        }

        return this->m_dpTree;

        // otherwise, search depth-first to sort the entry
    } else {
        // to avoid recursion, loop through a max of the number of available records
        DpBtreeNode* node = this->m_dpTree;
        for (FwSizeType record = 0; record < this->m_numDpSlots; record++) {
            CheckStat stat = CheckStat::CHECK_CONT;
            stat = this->checkLeftRight(entry < node->entry, node, entry);

            // act on status
            if (stat == CheckStat::CHECK_ERROR) {
                return nullptr;
            } else if (stat == CheckStat::CHECK_OK) {
                return node;
            }
        }  // end for each possible record

        return nullptr;
    }
}

DpCatalog::CheckStat DpCatalog::checkLeftRight(bool condition, DpBtreeNode*& node, const DpStateEntry& newEntry) {
    if (condition) {
        if (node->left == nullptr) {
            bool allocated = this->allocateNode(node->left, newEntry);
            if (not allocated) {
                return CheckStat::CHECK_ERROR;
            }
            node->left->parent = node;
            // Let the caller know where node ended up
            node = node->left;
            return CheckStat::CHECK_OK;
        } else {
            node = node->left;
            return CheckStat::CHECK_CONT;
        }
    } else {
        if (node->right == nullptr) {
            bool allocated = this->allocateNode(node->right, newEntry);
            if (not allocated) {
                return CheckStat::CHECK_ERROR;
            }
            node->right->parent = node;
            // Let the caller know where node ended up
            node = node->right;
            return CheckStat::CHECK_OK;
        } else {
            node = node->right;
            return CheckStat::CHECK_CONT;
        }
    }
}

bool DpCatalog::allocateNode(DpBtreeNode*& newNode, const DpStateEntry& newEntry) {
    // should always be null since we are allocating an empty slot
    FW_ASSERT(newNode == nullptr);
    // make sure there is an entry from the free list
    if (this->m_freeListHead == nullptr) {
        this->log_WARNING_HI_DpCatalogFull(newEntry.record);
        return false;
    }

    // get a new node from the free list
    newNode = this->m_freeListHead;
    // move the head of the free list to the next node
    // If we've at the bottom of the free list, head will now be nullptr
    this->m_freeListHead = this->m_freeListHead->left;

    // initialize the new node
    newNode->left = nullptr;
    newNode->right = nullptr;
    newNode->parent = nullptr;
    newNode->entry = newEntry;

    // we got one, so return success
    return true;
}

void DpCatalog::deallocateNode(DpBtreeNode* node) {
    DpBtreeNode* parent = node->parent;

    // since nodes are deallocated after xmit, left should be gone
    // However, left node could be added during xmit
    if (node->left != nullptr) {
        // Since we aren't limited to adding just 1 node during file transfer
        // the left child might not be a leaf
        // Instead, find the node of closest (but higher) priority to this node
        // This is the lowest priority node on the left branch
        // Which is the rightmost node of the left branch
        DpBtreeNode* rightmostNode = node->left;

        // (i.e. node->left->right->right ... ->right until we hit null)

        // bounded while loop (in case we're linked onto the free list somehow)
        for (FwSizeType record = 0; record < this->m_numDpSlots && rightmostNode->right != nullptr; record++) {
            rightmostNode = rightmostNode->right;

            // I really hope these never fire
            FW_ASSERT(rightmostNode != this->m_freeListHead);
            FW_ASSERT(rightmostNode != nullptr);
        }

        FW_ASSERT(rightmostNode != nullptr);
        FW_ASSERT(rightmostNode->parent != nullptr);

        // We can then swap the node to be deallocated w/ the rightmost
        // (since it is immediately higher priority than us)

        // Make the "parent" of the deallocated node point at the rightmost
        if (parent == nullptr) {
            // this is the root node: has no parent, but needs the root pointer to shift
            this->m_dpTree = rightmostNode;
        } else {
            // patch onto the appropriate parent branch
            if (parent->left == node) {
                parent->left = rightmostNode;
            } else {
                parent->right = rightmostNode;
            }
        }

        // Handle the children of the rightmost node
        if (rightmostNode == node->left) {
            // The rightmost node is the immediate left child of the deallocated node
            // Since it only has left children, shift the left branch up
            // with node->left taking the place of the deallocated node

            // Nothing we need to do
            // Just avoid the infinite loop that would occur in this case
            // on the other branch
        } else {
            // If the rightmost node isn't the node to be deallocated's left child,
            // we can stitch its left branch onto its parent in its place
            rightmostNode->parent->right = rightmostNode->left;

            // Now connect the deallocated node's left branch onto rightmostNode
            rightmostNode->left = node->left;
            node->left->parent = rightmostNode;
        }

        // Regardless, connect the deallocated node's right branch onto rightmostNode
        rightmostNode->right = node->right;

        if (node->right != nullptr) {
            node->right->parent = rightmostNode;
        }

        // Now that we're done using the parent of rightmost node
        // Point at actual parent or nullptr if this is the new root
        rightmostNode->parent = parent;

        // Ensure the Left node no longer points at us
        FW_ASSERT(node->left->parent != node);
    } else {
        // This node only had a right branch
        // cut out this node and shift the right branch up

        // The root node has no parent, but needs the tree root pointer to shift
        if (parent == nullptr) {
            this->m_dpTree = node->right;
        } else {
            // Patch the right branch onto
            // the appropriate parent branch of this node
            if (parent->left == node) {
                parent->left = node->right;
            } else {
                parent->right = node->right;
            }
        }

        // If there is a right branch
        // Point it at the parent of the removed node
        if (node->right != nullptr) {
            FW_ASSERT(node->right->parent != nullptr);
            node->right->parent = parent;
        }
    }

    // clear out the entry
    node->entry = {};
    // point this node @ the old head of the free list
    node->left = m_freeListHead;

    // Ensure the Right node no longer points at us
    if (node->right != nullptr) {
        FW_ASSERT(node->right->parent != node);
    }

    // clear out our right reference
    node->right = nullptr;

    DpBtreeNode* oldFreeListHead = this->m_freeListHead;
    // make this node the new head of the free list
    this->m_freeListHead = node;

    // Node is the head of the free list
    FW_ASSERT(this->m_freeListHead == node);

    node->parent = nullptr;

    // Ensure this Node only points at next in free list
    FW_ASSERT(node->left == oldFreeListHead);
    FW_ASSERT(node->right == nullptr);
    FW_ASSERT(node->parent == nullptr);
}

void DpCatalog::sendNextEntry() {
    // Use xmit flag to break upon STOP_XMIT_CATALOG
    if (this->m_xmitInProgress != true) {
        return;
    }

    // look in the tree for the next entry to send
    this->m_currentXmitNode = this->findNextTreeNode();

    if (this->m_currentXmitNode == nullptr) {
        // if no entry found, we are done
        this->m_xmitInProgress = false;
        this->log_ACTIVITY_HI_CatalogXmitCompleted(this->m_xmitBytes);
        this->dispatchWaitedResponse(Fw::CmdResponse::OK);
        return;
    } else {
        // build file name based on the found entry
        this->m_currXmitFileName.format(
            DP_FILENAME_FORMAT, this->m_directories[this->m_currentXmitNode->entry.dir].toChar(),
            this->m_currentXmitNode->entry.record.get_id(), this->m_currentXmitNode->entry.record.get_tSec(),
            this->m_currentXmitNode->entry.record.get_tSub());
        this->log_ACTIVITY_LO_SendingProduct(this->m_currXmitFileName,
                                             static_cast<U32>(this->m_currentXmitNode->entry.record.get_size()),
                                             this->m_currentXmitNode->entry.record.get_priority());
        Svc::SendFileResponse resp = this->fileOut_out(0, this->m_currXmitFileName, this->m_currXmitFileName, 0, 0);
        if (resp.get_status() != Svc::SendFileStatus::STATUS_OK) {
            // warn, but keep going since it may be an issue with this file but others could
            // make it
            this->log_WARNING_HI_DpFileSendError(this->m_currXmitFileName, resp.get_status());
        }
    }

}  // end sendNextEntry()

DpCatalog::DpBtreeNode* DpCatalog::findNextTreeNode() {
    // check some asserts
    FW_ASSERT(this->m_xmitInProgress);

    if (this->m_dpTree == nullptr) {
        // We've run out of entries, we are done
        this->m_xmitInProgress = false;
        return nullptr;
    }

    // start back at the top
    if (this->m_currentNode == nullptr) {
        this->m_currentNode = this->m_dpTree;
    }

    // Nav left until nullptr
    // Leads to highest priority node
    // bounded while loop (in case we're linked onto the free list somehow)
    for (FwSizeType record = 0; record < this->m_numDpSlots && this->m_currentNode->left != nullptr; record++) {
        this->m_currentNode = this->m_currentNode->left;

        // I really hope these never fire
        FW_ASSERT(this->m_currentNode != this->m_freeListHead);
        FW_ASSERT(this->m_currentNode != nullptr);
    }

    // save the high prio & find next best
    DpBtreeNode* found = this->m_currentNode;

    // On the next cycle, explore the node to the right, if available,
    // otherwise the node above
    if (this->m_currentNode->right != nullptr) {
        this->m_currentNode = this->m_currentNode->right;
    } else {
        this->m_currentNode = this->m_currentNode->parent;
    }

    return found;
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
    }

    // Catalog cleared while this file was sent
    if (!this->m_catalogBuilt) {
        return;
    }

    // Since catalog built flag is true
    // we should have a tree w/ at least one element
    FW_ASSERT(this->m_dpTree);

    // Reduce pending
    this->m_pendingDpBytes -= this->m_currentXmitNode->entry.record.get_size();
    this->m_pendingFiles--;
    // Log File Complete & pending
    this->log_ACTIVITY_LO_ProductComplete(this->m_currXmitFileName, this->m_pendingFiles, this->m_pendingDpBytes);

    // mark the entry as transmitted
    this->m_currentXmitNode->entry.record.set_state(Fw::DpState::TRANSMITTED);
    // update the transmitted state in the state file
    this->appendFileState(this->m_currentXmitNode->entry);
    // add the size
    this->m_xmitBytes += this->m_currentXmitNode->entry.record.get_size();
    // deallocate this node
    this->deallocateNode(this->m_currentXmitNode);
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
            this->m_currentNode = this->m_dpTree;
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
    this->resetBinaryTree();
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
