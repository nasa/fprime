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
#include <new> // placement new

namespace Svc {

    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    DpCatalog ::
        DpCatalog(const char* const compName) :
        DpCatalogComponentBase(compName),
        m_initialized(false),
        m_dpList(nullptr),
        m_sortedDpList(nullptr),
        m_numDpRecords(0),
        m_numDpSlots(0),
        m_numDirectories(0),
        m_memSize(0),
        m_memPtr(nullptr),
        m_allocatorId(0),
        m_allocator(nullptr),
        m_xmitInProgress(false),
        m_currXmitRecord(nullptr),
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
        NATIVE_UINT_TYPE memId,
        Fw::MemAllocator& allocator
    ) {

        // Do some assertion checks
        FW_ASSERT(numDirs <= DP_MAX_DIRECTORIES, static_cast<FwAssertArgType>(numDirs));

        // request memory for catalog
        this->m_memSize = DP_MAX_FILES * (sizeof(DpStateEntry) + sizeof(DpSortedList));
        bool notUsed; // we don't need to recover the catalog.
        // request memory
        this->m_memPtr = allocator.allocate(memId, this->m_memSize, notUsed);
        // adjust to actual size if less allocated and only initialize
        // if there is enough room for at least one record and memory
        // was allocated
        if (
            (this->m_memSize >= (sizeof(DpSortedList) + sizeof(DpStateEntry))) and
            (this->m_memPtr != nullptr)
            ) {
            // set the number of available record slots
            this->m_numDpSlots = this->m_memSize / (sizeof(DpSortedList) + sizeof(DpStateEntry));
            // initialize data structures
            // state entry pointers will be at the beginning of the memory
            this->m_dpList = static_cast<DpStateEntry*>(this->m_memPtr);
            // sorted list will be after state structures
            this->m_sortedDpList = reinterpret_cast<DpSortedList*>(&this->m_dpList[this->m_numDpSlots]);
            for (FwSizeType slot = 0; slot < this->m_numDpSlots; slot++) {
                // overlay new instance of the DpState entry on the memory
                (void) new(&this->m_dpList[slot]) DpStateEntry();
                this->m_dpList[slot].entry = false;
                this->m_dpList[slot].dir = -1;
                // initialize sorted list entry to empty
                this->m_sortedDpList[slot].recPtr = nullptr;
                this->m_sortedDpList[slot].sent = false;
            }
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

    Fw::CmdResponse DpCatalog::doCatalogBuild() {

        // check initialization
        if (not this->checkInit()) {
            return Fw::CmdResponse::EXECUTION_ERROR;
        }

        // make sure a downlink is not in progress
        if (this->m_xmitInProgress) {
            this->log_WARNING_LO_DpXmitInProgress();
            return Fw::CmdResponse::EXECUTION_ERROR;
        }

        // keep cumulative number of files
        FwSizeType totalFiles = 0;

        // file class instance for processing files
        Os::File dpFile;
        // Working buffer for DP headers
        U8 dpBuff[Fw::DpContainer::MIN_PACKET_SIZE]; // FIXME: replace magic number
        Fw::Buffer hdrBuff(dpBuff, sizeof(dpBuff)); // buffer for container header decoding
        Fw::DpContainer container; // container object for extracting header fields

        // get file listings from file system
        for (FwSizeType dir = 0; dir < this->m_numDirectories; dir++) {
            // read in each directory and keep track of total
            this->log_ACTIVITY_LO_ProcessingDirectory(this->m_directories[dir]);
            U32 filesRead = 0;
            U32 pendingFiles = 0;
            U64 pendingDpBytes = 0;

            Os::FileSystem::Status fsStat =
                Os::FileSystem::readDirectory(
                    this->m_directories[dir].toChar(),
                    static_cast<U32>(this->m_numDpSlots - totalFiles),
                    this->m_fileList,
                    filesRead
                );
            if (fsStat != Os::FileSystem::OP_OK) {
                this->log_WARNING_HI_DirectoryOpenError(
                    this->m_directories[dir],
                    fsStat
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
                entry.entry = true;

                // assign the entry to the stored list
                this->m_dpList[this->m_numDpRecords] = entry;

                // insert entry into sorted list. if can't insert, quit
                if (not this->insertEntry(this->m_dpList[this->m_numDpRecords])) {
                    this->log_WARNING_HI_DpInsertError(entry.record);
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

            } // end for each file in a directory

            totalFiles += filesRead;

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

        this->log_ACTIVITY_HI_CatalogBuildComplete();

        return Fw::CmdResponse::OK;
    }

    bool DpCatalog::insertEntry(DpStateEntry& entry) {

        // Prototype version: Just add the record to the end of the list
        this->m_sortedDpList[this->m_numDpRecords].recPtr = &entry;
        this->m_sortedDpList[this->m_numDpRecords].sent = false;
        this->m_numDpRecords++;

        return true;

    }

    void DpCatalog::deleteEntry(DpStateEntry& entry) {

    }

    void DpCatalog::sendNextEntry() {

        // Make sure that pointer is valid
        FW_ASSERT(this->m_sortedDpList);

        // Demo code: Walk through list and send first file
        for (FwSizeType record = 0; record < this->m_numDpRecords; record++) {
            if (not this->m_sortedDpList[record].sent) {
                // make sure the entry is used
                if (this->m_sortedDpList[record].recPtr != nullptr) {
                    // store pointer to record for fileDone callback
                    this->m_currXmitRecord = &this->m_sortedDpList[record];
                    // build file name
                    this->m_currXmitFileName.format(DP_FILENAME_FORMAT,
                        this->m_directories[this->m_sortedDpList[record].recPtr->dir].toChar(),
                        this->m_sortedDpList[record].recPtr->record.getid(),
                        this->m_sortedDpList[record].recPtr->record.gettSec(),
                        this->m_sortedDpList[record].recPtr->record.gettSub()
                    );
                    this->log_ACTIVITY_LO_SendingProduct(
                        this->m_currXmitFileName,
                        static_cast<U32>(this->m_sortedDpList[record].recPtr->record.getsize()),
                        this->m_sortedDpList[record].recPtr->record.getpriority()
                        );
                    this->fileOut_out(0, this->m_currXmitFileName, this->m_currXmitFileName, 0, 0);
                    this->m_xmitBytes += this->m_sortedDpList[record].recPtr->record.getsize();
                    // quit looking when we find an entry
                    return;
                }
                // clean up record
                this->m_sortedDpList[record].sent = true;
            }
        }

        // if none were found, finish transmission
        this->log_ACTIVITY_HI_CatalogXmitCompleted(this->m_xmitBytes);
        this->m_xmitInProgress = false;
        this->m_xmitBytes = 0;
        this->m_currXmitRecord = nullptr;

        if (this->m_xmitCmdWait) {
            this->m_xmitCmdWait = false;
            this->cmdResponse_out(this->m_xmitOpCode, this->m_xmitCmdSeq, Fw::CmdResponse::OK);
        }

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
        if (this->m_currXmitRecord) {
            this->m_currXmitRecord->sent = true;
            this->log_ACTIVITY_LO_ProductComplete(this->m_currXmitFileName);
        }

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
        this->cmdResponse_out(opCode, cmdSeq, this->doCatalogBuild());
    }

    void DpCatalog ::
        START_XMIT_CATALOG_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq,
            Fw::Wait wait
        )
    {

        Fw::CmdResponse resp = this->doCatalogXmit();

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

    Fw::CmdResponse DpCatalog::doCatalogXmit() {

        this->m_xmitBytes = 0;
        this->sendNextEntry();
        return Fw::CmdResponse::OK;

    }


    void DpCatalog ::
        STOP_XMIT_CATALOG_cmdHandler(
            FwOpcodeType opCode,
            U32 cmdSeq
        )
    {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
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
