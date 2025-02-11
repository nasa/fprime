// ======================================================================
// \title  DpCatalog.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component implementation class
// ======================================================================

#include "FpConfig.hpp"
#include "Fw/Dp/DpContainer.hpp"
#include "Svc/DpCatalog/DpCatalog.hpp"

#include "Os/FileSystem.hpp"
#include "Os/File.hpp"
#include "Fw/Types/StringUtils.hpp"
#include <new> // placement new

namespace Svc {

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    DpCatalog ::
        DpCatalog(const char* const compName) :
        DpCatalogComponentBase(compName),
        m_initialized(false),
        m_dpTree(nullptr),
        m_freeListHead(nullptr),
        m_freeListFoot(nullptr),
        m_traverseStack(nullptr),
        m_currentNode(nullptr),
        m_currentXmitNode(nullptr),
        m_numDpRecords(0),
        m_numDpSlots(0),
        m_numDirectories(0),
        m_stateFileData(nullptr),
        m_stateFileEntries(0),
        m_memSize(0),
        m_memPtr(nullptr),
        m_allocatorId(0),
        m_allocator(nullptr),
        m_xmitInProgress(false),
        m_xmitCmdWait(false),
        m_xmitBytes(0),
        m_xmitOpCode(0),
        m_xmitCmdSeq(0)
    {

    }

    DpCatalog ::
        ~DpCatalog()
    {}

    void DpCatalog::configure(
        Fw::FileNameString directories[DP_MAX_DIRECTORIES],
        FwSizeType numDirs,
        Fw::FileNameString& stateFile,
        NATIVE_UINT_TYPE memId,
        Fw::MemAllocator& allocator
    ) {

        // Do some assertion checks
        FW_ASSERT(numDirs <= DP_MAX_DIRECTORIES, static_cast<FwAssertArgType>(numDirs));
        FW_ASSERT(stateFile.length());
        this->m_stateFile = stateFile;

        // request memory for catalog which is DP_MAX_FILES * slot size.
        // 
        // A "slot" consists of a set of three memory locations for each data product consisting
        // an entry in the binary tree, an entry in the binary tree traversal stack, and 
        // an entry in the state file data. These may not be fully used in a given 
        // situation based on the number of actual data products, but this provides room for the
        // maximum possible.
        static const FwSizeType slotSize = sizeof(DpBtreeNode) + sizeof(DpBtreeNode**) + sizeof(DpDstateFileEntry);
        this->m_memSize = DP_MAX_FILES * slotSize;
        bool notUsed; // we don't need to recover the catalog.
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
        // 3) Place the binary tree traverse stack in memory just after the binary
        // tree free list by indexing the free list as an array one element past the
        // end of the free list.
        //
        // 4) Place the state file data in memory after the binary tree traverse
        // stack by indexing the traverse stack to one element past the end of 
        // the traverse tree.

        if (
            (this->m_memSize >= sizeof(DpBtreeNode)) and
            (this->m_memPtr != nullptr)
            ) {
            // set the number of available record slots based on how much memory we actually got
            this->m_numDpSlots = this->m_memSize / slotSize; // Step 1.
            this->resetBinaryTree(); // Step 2
            // assign pointer for the stack - Step 3
            this->m_traverseStack = reinterpret_cast<DpBtreeNode**>(&this->m_freeListHead[this->m_numDpSlots]);
            this->resetTreeStack();
            // assign pointer for the state file storage - Step 4
            this->m_stateFileData = reinterpret_cast<DpDstateFileEntry*>(&this->m_traverseStack[this->m_numDpSlots]);
        }
        else {
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
            (void) new(&this->m_freeListHead[slot]) DpBtreeNode();
            this->m_freeListHead[slot].left = nullptr;
            this->m_freeListHead[slot].right = nullptr;
            // link the free list
            if (slot > 0) {
                this->m_freeListHead[slot-1].left = &this->m_freeListHead[slot];
            }
        }
        // set the foot of the free list
        this->m_freeListFoot = &this->m_freeListHead[this->m_numDpSlots - 1];
        // clear binary tree
        this->m_dpTree = nullptr;
        // reset number of records
        this->m_numDpRecords = 0;
        
    }

    void DpCatalog::resetStateFileData() {
        // clear state file data
        for (FwSizeType slot = 0; slot < this->m_numDpSlots; slot++) {
            this->m_stateFileData[slot].used = false; 
            this->m_stateFileData[slot].visited = false; 
            (void) new(&this->m_stateFileData[slot].entry.record) DpRecord();
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

        BYTE buffer[sizeof(FwIndexType)+DpRecord::SERIALIZED_SIZE];
        Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));

        // open the state file
        Os::File stateFile;
        Os::File::Status stat = stateFile.open(this->m_stateFile.toChar(), Os::File::OPEN_READ);
        if (stat != Os::File::OP_OK) {
            this->log_WARNING_HI_StateFileOpenError(this->m_stateFile, stat);
            return Fw::CmdResponse::EXECUTION_ERROR;
        }

        FwSignedSizeType fileLoc = 0;
        this->m_stateFileEntries = 0;

        // read entries from the state file
        for (FwSizeType entry = 0; entry < this->m_numDpSlots; entry++) {

            FwSignedSizeType size = sizeof(buffer);
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
                this->log_WARNING_HI_StateFileTruncated(this->m_stateFile, static_cast<I32>(fileLoc), static_cast<I32>(size));
                return Fw::CmdResponse::OK;
            }

            // reset the buffer for deserializing the entry
            Fw::SerializeStatus serStat = entryBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(size));
            // should always fit
            FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat,serStat);
            entryBuffer.resetDeser();

            // deserialization after this point should always work, since
            // the source buffer was specifically sized to hold the data

            // Deserialize the file directory index
            Fw::SerializeStatus status = entryBuffer.deserialize(this->m_stateFileData[entry].entry.dir);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == status,status);
            status = entryBuffer.deserialize(this->m_stateFileData[entry].entry.record);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == status,status);
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
            // check for a match
            if (                
                (this->m_stateFileData[line].entry.dir == entry.dir) and
                (this->m_stateFileData[line].entry.record.getid() == entry.record.getid()) and
                (this->m_stateFileData[line].entry.record.gettSec() == entry.record.gettSec()) and
                (this->m_stateFileData[line].entry.record.gettSub() == entry.record.gettSub()) and
                (this->m_stateFileData[line].entry.record.getpriority() == entry.record.getpriority())
                ) {
                // update the transmitted state
                entry.record.setstate(this->m_stateFileData[line].entry.record.getstate());
                entry.record.setblocks(this->m_stateFileData[line].entry.record.getblocks());
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
        Os::File::Status stat = stateFile.open(
            this->m_stateFile.toChar(), 
            Os::File::OPEN_CREATE,
            Os::FileInterface::OVERWRITE);
            
        if (stat != Os::File::OP_OK) {
            this->log_WARNING_HI_StateFileOpenError(this->m_stateFile, stat);
            return;
        }

        // buffer for writing entries
        BYTE buffer[sizeof(FwIndexType)+DpRecord::SERIALIZED_SIZE];
        Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));

        // write entries to the state file
        for (FwSizeType entry = 0; entry < this->m_numDpSlots; entry++) {
            // only write entries that were used
            if (
                (this->m_stateFileData[entry].used) and 
                (this->m_stateFileData[entry].visited)
                ) {
                // reset the buffer for serializing the entry
                entryBuffer.resetSer();
                // serialize the file directory index
                Fw::SerializeStatus serStat = entryBuffer.serialize(this->m_stateFileData[entry].entry.dir);
                // Should always fit
                FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat,serStat);
                serStat = entryBuffer.serialize(this->m_stateFileData[entry].entry.record);
                // Should always fit
                FW_ASSERT(Fw::FW_SERIALIZE_OK == serStat,serStat);
                // write the entry
                FwSignedSizeType size = entryBuffer.getBuffLength();
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
        FW_ASSERT(entry.dir < static_cast<FwIndexType>(this->m_numDirectories),
            entry.dir,
            static_cast<FwAssertArgType>(this->m_numDirectories)
        );

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
        BYTE buffer[sizeof(entry.dir)+sizeof(entry.record)];
        Fw::ExternalSerializeBuffer entryBuffer(buffer, sizeof(buffer));
        // reset the buffer for serializing the entry
        entryBuffer.resetSer();
        // serialize the file directory index
        Fw::SerializeStatus serStat = entryBuffer.serialize(entry.dir);
        // should fit
        FW_ASSERT(serStat == Fw::FW_SERIALIZE_OK,serStat);
        serStat = entryBuffer.serialize(entry.record);
        // should fit
        FW_ASSERT(serStat == Fw::FW_SERIALIZE_OK,serStat);
        // write the entry
        FwSignedSizeType size = entryBuffer.getBuffLength();
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
            this->log_WARNING_HI_NotInitialized();
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

        return Fw::CmdResponse::OK;
    }

    Fw::CmdResponse DpCatalog::fillBinaryTree() {

        // keep cumulative number of files
        FwSizeType totalFiles = 0;

        // file class instance for processing files
        Os::File dpFile;
        // Working buffer for DP headers
        U8 dpBuff[Fw::DpContainer::MIN_PACKET_SIZE]; // Header buffer
        Fw::Buffer hdrBuff(dpBuff, sizeof(dpBuff)); // buffer for container header decoding
        Fw::DpContainer container; // container object for extracting header fields

        // get file listings from file system
        for (FwSizeType dir = 0; dir < this->m_numDirectories; dir++) {
            // read in each directory and keep track of total
            this->log_ACTIVITY_LO_ProcessingDirectory(this->m_directories[dir]);
            FwSizeType filesRead = 0;
            U32 pendingFiles = 0;
            U64 pendingDpBytes = 0;
            U32 filesProcessed = 0;

            Os::Directory dpDir;
            Os::Directory::Status status = dpDir.open(this->m_directories[dir].toChar(), Os::Directory::OpenMode::READ);
            if (status != Os::Directory::OP_OK) {
                this->log_WARNING_HI_DirectoryOpenError(
                    this->m_directories[dir],
                    status
                );
                return Fw::CmdResponse::EXECUTION_ERROR;
            }
            status = dpDir.readDirectory(this->m_fileList, (this->m_numDpSlots - totalFiles), filesRead);

            if (status != Os::Directory::OP_OK) {
                this->log_WARNING_HI_DirectoryOpenError(
                    this->m_directories[dir],
                    status
                );
                return Fw::CmdResponse::EXECUTION_ERROR;
            }

            // Assert number of files isn't more than asked
            FW_ASSERT(
                filesRead <= this->m_numDpSlots - totalFiles,
                static_cast<FwAssertArgType>(filesRead),
                static_cast<FwAssertArgType>(this->m_numDpSlots - totalFiles));

            // extract metadata for each file
            for (FwNativeUIntType file = 0; file < filesRead; file++) {

                // only consider files with the DP extension
            
                FwSignedSizeType loc = Fw::StringUtils::substring_find(
                    this->m_fileList[file].toChar(),
                    this->m_fileList[file].length(),
                    DP_EXT,
                    Fw::StringUtils::string_length(DP_EXT,sizeof(DP_EXT))
                );

                if (-1 == loc) {
                    continue;
                }

                Fw::String fullFile;
                fullFile.format("%s/%s",
                    this->m_directories[dir].toChar(),
                    this->m_fileList[file].toChar()
                );

                this->log_ACTIVITY_LO_ProcessingFile(fullFile);

                // get file size
                FwSignedSizeType fileSize = 0;
                Os::FileSystem::Status sizeStat =
                    Os::FileSystem::getFileSize(fullFile.toChar(),fileSize);
                if (sizeStat != Os::FileSystem::OP_OK) {
                    this->log_WARNING_HI_FileSizeError(fullFile, sizeStat);
                    continue;
                }

                Os::File::Status stat = dpFile.open(fullFile.toChar(), Os::File::OPEN_READ);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_FileOpenError(fullFile, stat);
                    continue;
                }

                // Read DP header
                FwSignedSizeType size = Fw::DpContainer::Header::SIZE;

                stat = dpFile.read(dpBuff, size);
                if (stat != Os::File::OP_OK) {
                    this->log_WARNING_HI_FileReadError(fullFile, stat);
                    dpFile.close();
                    continue; // maybe next file is fine
                }

                // if full header isn't read, something's wrong with the file, so skip
                if (size != Fw::DpContainer::Header::SIZE) {
                    this->log_WARNING_HI_FileReadError(fullFile, Os::File::BAD_SIZE);
                    dpFile.close();
                    continue; // maybe next file is fine
                }

                // if all is well, don't need the file any more
                dpFile.close();

                // give buffer to container instance
                container.setBuffer(hdrBuff);

                // reset header deserialization in the container
                Fw::SerializeStatus desStat = container.deserializeHeader();
                if (desStat != Fw::FW_SERIALIZE_OK) {
                    this->log_WARNING_HI_FileHdrDesError(fullFile, desStat);
                }

                // add entry to catalog.
                DpStateEntry entry;
                entry.dir = static_cast<FwIndexType>(dir);
                entry.record.setid(container.getId());
                entry.record.setpriority(container.getPriority());
                entry.record.setstate(container.getState());
                entry.record.settSec(container.getTimeTag().getSeconds());
                entry.record.settSub(container.getTimeTag().getUSeconds());
                entry.record.setsize(static_cast<U64>(fileSize));

                // check the state file to see if there is transmit state
                this->getFileState(entry);

                // insert entry into sorted list. if can't insert, quit
                bool insertedOk = this->insertEntry(entry);
                if (not insertedOk) {
                    this->log_WARNING_HI_DpInsertError(entry.record);
                    // clean up and return
                    this->resetBinaryTree();
                    this->resetStateFileData();
                    break;
                }

                if (entry.record.getstate() == Fw::DpState::UNTRANSMITTED) {
                    pendingFiles++;
                    pendingDpBytes += entry.record.getsize();
                }

                // make sure we haven't exceeded the limit
                if (this->m_numDpRecords > this->m_numDpSlots) {
                    this->log_WARNING_HI_DpCatalogFull(entry.record);
                    break;
                }

                filesProcessed++;

            } // end for each file in a directory

            totalFiles += filesProcessed;

            this->log_ACTIVITY_HI_ProcessingDirectoryComplete(
                this->m_directories[dir],
                static_cast<U32>(totalFiles),
                pendingFiles,
                pendingDpBytes
            );

            // check to see if catalog is full
            // that means generated products exceed the catalog size
            if (totalFiles == this->m_numDpSlots) {
                this->log_WARNING_HI_CatalogFull(this->m_directories[dir]);
                break;
            }
        } // end for each directory

        return Fw::CmdResponse::OK;
 
    } // end fillBinaryTree()


    bool DpCatalog::insertEntry(DpStateEntry& entry) {

        // the tree is filled in the following priority order:
        // 1. DP priority - lower number is higher priority
        // 2. DP time - older is higher priority
        // 3. DP ID - lower number is higher priority

        // Higher priority is to the left of the tree

        // if the tree is empty, add the first entry
        if (this->m_dpTree == nullptr) {
            bool goodInsert = this->allocateNode(this->m_dpTree,entry);
            if (not goodInsert) {
                return false;
            }
        // otherwise, search depth-first to sort the entry
        } else {
            // to avoid recursion, loop through a max of the number of available records
            DpBtreeNode* node = this->m_dpTree;
            for (FwSizeType record = 0; record < this->m_numDpSlots; record++) {
                CheckStat stat = CheckStat::CHECK_CONT;
                // check priority. Lower is higher priority 
                if (entry.record.getpriority() == node->entry.record.getpriority()) {
                    // check time. Older is higher priority
                    if (entry.record.gettSec() == node->entry.record.gettSec()) {
                        // check ID. Lower is higher priority
                        stat = this->checkLeftRight(
                            entry.record.getid() < node->entry.record.getid(),
                            node,
                            entry
                        );
                    } else { // if seconds are not equal. Older is higher priority
                        stat = this->checkLeftRight(
                            entry.record.gettSec() < node->entry.record.gettSec(),
                            node,
                            entry
                        );
                    }
                } else { // if priority is not equal. Lower is higher priority.
                    stat = this->checkLeftRight(
                        entry.record.getpriority() < node->entry.record.getpriority(),
                        node,
                        entry
                    );
                } // end checking for left/right insertion

                // act on status
                if (stat == CheckStat::CHECK_ERROR) {
                    return false;
                } else if (stat == CheckStat::CHECK_OK) {
                    break;
                }
            } // end for each possible record
        }

        // increment the number of records

        this->m_numDpRecords++;

        return true;

    }

    DpCatalog::CheckStat DpCatalog::checkLeftRight(bool condition, DpBtreeNode* &node, const DpStateEntry& newEntry) {
        if (condition) {
            if (node->left == nullptr) {
                bool allocated = this->allocateNode(node->left,newEntry);
                if (not allocated) {
                    return CheckStat::CHECK_ERROR;
                }
                return CheckStat::CHECK_OK;
            } else {
                node = node->left;
                return CheckStat::CHECK_CONT;
            }
        } else {
            if (node->right == nullptr) {
                bool allocated = this->allocateNode(node->right,newEntry);
                if (not allocated) {
                    return CheckStat::CHECK_ERROR;
                }
                return CheckStat::CHECK_OK;
            } else {
                node = node->right;
                return CheckStat::CHECK_CONT;
            }
        }
    }


    bool DpCatalog::allocateNode(DpBtreeNode* &newNode, 
            const DpStateEntry& newEntry) {
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
        this->m_freeListHead = this->m_freeListHead->left;

        // initialize the new node
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->entry = newEntry;

        // we got one, so return success
        return true;

    }


    void DpCatalog::deleteEntry(DpStateEntry& entry) {

    }

    void DpCatalog::sendNextEntry() {

        // check some asserts
        FW_ASSERT(this->m_dpTree);
        FW_ASSERT(this->m_xmitInProgress);
        FW_ASSERT(this->m_traverseStack);

        // look in the tree for the next entry to send
        this->m_currentXmitNode = this->findNextTreeNode();

        if (this->m_currentXmitNode == nullptr) {
            // if no entry found, we are done
            this->m_xmitInProgress = false;
            this->log_ACTIVITY_HI_CatalogXmitCompleted(this->m_xmitBytes);
            if (this->m_xmitCmdWait) {
                this->cmdResponse_out(this->m_xmitOpCode,this->m_xmitCmdSeq,Fw::CmdResponse::OK);
            }
            return;
        } else {
            // build file name based on the found entry
            this->m_currXmitFileName.format(DP_FILENAME_FORMAT,
                this->m_directories[this->m_currentXmitNode->entry.dir].toChar(),
                this->m_currentXmitNode->entry.record.getid(),
                this->m_currentXmitNode->entry.record.gettSec(),
                this->m_currentXmitNode->entry.record.gettSub()
            );
            this->log_ACTIVITY_LO_SendingProduct(
                this->m_currXmitFileName,
                static_cast<U32>(this->m_currentXmitNode->entry.record.getsize()),
                this->m_currentXmitNode->entry.record.getpriority()
                );
            Svc::SendFileResponse resp = this->fileOut_out(0, this->m_currXmitFileName, this->m_currXmitFileName, 0, 0);
            if (resp.getstatus() != Svc::SendFileStatus::STATUS_OK) {
                // warn, but keep going since it may be an issue with this file but others could
                // make it
                this->log_WARNING_HI_DpFileSendError(this->m_currXmitFileName,resp.getstatus());
            }

        }

    } // end sendNextEntry()

    DpCatalog::DpBtreeNode* DpCatalog::findNextTreeNode() {

        // check some asserts
        FW_ASSERT(this->m_dpTree);
        FW_ASSERT(this->m_xmitInProgress);
        FW_ASSERT(this->m_traverseStack);

        DpBtreeNode* found = nullptr;

        // traverse the tree, finding nodes in order. Max iteration of the loop
        // would be the number of records in the tree
        for (FwSizeType record = 0; record < this->m_numDpRecords; record++) {
            // initialize found entry to nullptr
            found = nullptr;
            // check for current node to be null
            if (this->m_currentNode == nullptr) {
                // see if we fully traversed the tree
                if (this->m_currStackEntry < 0) {
                    // Step 5 - we are done
                    return nullptr;
                } else {
                    // Step 4 - if the current node is null, pop back up the stack
                    this->m_currentNode = this->m_traverseStack[this->m_currStackEntry--];
                    if (this->m_currentNode->entry.record.getstate() != Fw::DpState::TRANSMITTED) {
                        found = this->m_currentNode;
                    }// check if transmitted
                    this->m_currentNode = this->m_currentNode->right;
                    if (found != nullptr) {
                        return found;
                    }
                }
                break;
            } else {
                if (this->m_currentNode->left != nullptr) {
                    // Step 3 - push current entry on the stack
                    this->m_traverseStack[++this->m_currStackEntry] = this->m_currentNode;
                    this->m_currentNode = this->m_currentNode->left;
                } else {
                    // Step 4 - check to see if this node has already been transmitted, if so, pop back up the stack
                    if (this->m_currentNode->entry.record.getstate() != Fw::DpState::TRANSMITTED) {
                        // we found an entry, so set the return to the current node
                        found = this->m_currentNode;
                    } // check if transmitted
                    // go to the right node
                    this->m_currentNode = this->m_currentNode->right;
                    // if a node was found, return it
                    if (found != nullptr) {
                        return found;
                    }
                } // check if left is null
            } // end else current node is not null
        } // end for each possible node in the tree

        return found;
    }

    bool DpCatalog::checkInit() {
        if (not this->m_initialized) {
            this->log_WARNING_HI_ComponentNotInitialized();
            return false;
        }
        else if (0 == this->m_numDpSlots) {
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

    void DpCatalog ::
        fileDone_handler(
            NATIVE_INT_TYPE portNum,
            const Svc::SendFileResponse& resp
        )
    {
        // check some asserts
        FW_ASSERT(this->m_dpTree);
        FW_ASSERT(this->m_traverseStack);

        // check file status
        if (resp.getstatus() != Svc::SendFileStatus::STATUS_OK) {
            this->log_WARNING_HI_DpFileXmitError(this->m_currXmitFileName,resp.getstatus());
            this->m_xmitInProgress = false;
            this->cmdResponse_out(this->m_xmitOpCode,this->m_xmitCmdSeq,Fw::CmdResponse::EXECUTION_ERROR);
        }

        this->log_ACTIVITY_LO_ProductComplete(this->m_currXmitFileName);

        // mark the entry as transmitted
        this->m_currentXmitNode->entry.record.setstate(Fw::DpState::TRANSMITTED);
        // update the transmitted state in the state file
        this->appendFileState(this->m_currentXmitNode->entry);
        // add the size
        this->m_xmitBytes += this->m_currentXmitNode->entry.record.getsize();
        // send the next entry, if it exists
        this->sendNextEntry();
    }

    void DpCatalog ::
        pingIn_handler(
            NATIVE_INT_TYPE portNum,
            U32 key
        )
    {
        // return code for health ping
        this->pingOut_out(0, key);
    }

    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    void DpCatalog ::
        BUILD_CATALOG_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq
        )
    {
        // invoke helper
        this->cmdResponse_out(opCode, cmdSeq,this->doCatalogBuild());

    }

    void DpCatalog ::
        START_XMIT_CATALOG_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq,
            Fw::Wait wait
        )
    {

        Fw::CmdResponse resp = this->doCatalogXmit();

        if (resp != Fw::CmdResponse::OK) {
            this->cmdResponse_out(opCode, cmdSeq, resp); 
        } else {
            if (Fw::Wait::NO_WAIT == wait) {
                this->cmdResponse_out(opCode, cmdSeq, resp);
                this->m_xmitCmdWait = false;
                this->m_xmitOpCode = 0;
                this->m_xmitCmdSeq = 0;
            }
            else {
                this->m_xmitCmdWait = true;
                this->m_xmitOpCode = opCode;
                this->m_xmitCmdSeq = cmdSeq;
            }
        }

    }

    Fw::CmdResponse DpCatalog::doCatalogXmit() {

        // check initialization
        if (not this->checkInit()) {
            this->log_WARNING_HI_NotInitialized();
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

        // start transmission 
        this->m_xmitBytes = 0;

        // make sure we have valid pointers
        FW_ASSERT(this->m_dpTree);
        FW_ASSERT(this->m_traverseStack);

        // Traverse the tree using a stack to avoid recursion
        // https://codestandard.net/articles/binary-tree-inorder-traversal/

        this->resetTreeStack();
        this->m_xmitInProgress = true;
        // Step 3b - search for and send first entry
        this->sendNextEntry();
        return Fw::CmdResponse::OK;

    }

    void DpCatalog::resetTreeStack() {
        // See URL above
        // Step 1 - reset the stack
        this->m_currStackEntry = -1;
        // Step 2 - assign root of the tree to the current entry
        this->m_currentNode = this->m_dpTree;
    }

    void DpCatalog ::
        STOP_XMIT_CATALOG_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq
        )
    {
        if (not this->m_xmitInProgress) {
            this->log_WARNING_LO_XmitNotActive();
            // benign error, so don't fail the command
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        } else {
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        }
    }

    void DpCatalog ::
        CLEAR_CATALOG_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq
        )
    {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }


}
