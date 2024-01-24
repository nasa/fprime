// ======================================================================
// \title  DpCatalog.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component implementation class
// ======================================================================

#include "FpConfig.hpp"
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
        m_numDirectories(0),
        m_memSize(0),
        m_memPtr(nullptr),
        m_allocatorId(0),
        m_allocator(nullptr)
    {

    }

    DpCatalog ::
        ~DpCatalog()
    {}

    void DpCatalog::configure(
        FwSizeType maxDpFiles,
        Fw::String directories[DP_MAX_DIRECTORIES],
        FwSizeType numDirs,
        NATIVE_UINT_TYPE memId,
        Fw::MemAllocator& allocator
    ) {

        // Do some assertion checks
        FW_ASSERT(numDirs < DP_MAX_DIRECTORIES, numDirs);

        // request memory for catalog
        this->m_memSize = maxDpFiles * (sizeof(DpStateEntry) + sizeof(DpSortedList));
        bool notUsed; // we don't need to recover the catalog.
        // request memory
        this->m_memPtr = allocator.allocate(memId,this->m_memSize,notUsed);
        // adjust to actual size if less allocated and only initialize
        // if there is enough room for at least one record and memory 
        // was allocated
        if (
                (this->m_memSize >= (sizeof(DpSortedList) + sizeof(DpStateEntry))) and
                (this->m_memPtr != nullptr)
            ) {
            this->m_numDpRecords = this->m_memSize/(sizeof(DpSortedList) + sizeof(DpStateEntry));
            // intialize data structures
            // state entry pointers will be at the beginning of the memory
            this->m_dpList = static_cast<DpStateEntry*>(this->m_memPtr);
            // sorted list will be after state structures
            this->m_sortedDpList = reinterpret_cast<DpSortedList*>(&this->m_dpList[this->m_numDpRecords]);
            for (FwSizeType record = 0; record < this->m_numDpRecords; record++) {
                // overlay new instance of the DpState entry on the memory
                (void) new(&this->m_dpList[record]) DpStateEntry();
                this->m_dpList[record].entry = false;
                this->m_dpList[record].dir = -1;
                // intialize sorted list entry to empty
                this->m_sortedDpList[record].recPtr = nullptr;
            }
        } else {
            // if we don't have enough memory, set the number of records 
            // to zero for later detection
            this->m_numDpRecords = 0;
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

        // keep cumulative number of files
        FwSizeType totalFiles = 0;

        // array for directory listing
        Fw::String listing[this->m_numDpRecords];
        // get file listings from file system
        for (FwSizeType dir=0; dir<this->m_numDirectories; dir++) {
            U32 filesRead = 0;
            Os::FileSystem::Status stat =
                Os::FileSystem::readDirectory(
                    this->m_directories[dir].toChar(),
                    this->m_numDpRecords-totalFiles,
                    listing,
                    filesRead
                );
            if (stat != Os::FileSystem::OP_OK) {
                this->log_WARNING_HI_DirectoryOpenError(
                    this->m_directories[dir],
                    stat
                );
                return Fw::CmdResponse::EXECUTION_ERROR;
            }

            totalFiles += filesRead;
        }



        return Fw::CmdResponse::OK;
    }


    bool DpCatalog::checkInit() {
        if (not this->m_initialized) {
            this->log_WARNING_HI_ComponentNotIntialized();
            return false;
        } else if (0 == this->m_numDpRecords) {
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
            this->m_allocator->deallocate(this->m_allocatorId,this->m_memPtr);
        }

    }


    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

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
            U32 cmdSeq
        )
    {
        // TODO
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
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

}
