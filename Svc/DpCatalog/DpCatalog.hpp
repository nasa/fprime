// ======================================================================
// \title  DpCatalog.hpp
// \author tcanham
// \brief  hpp file for DpCatalog component implementation class
// ======================================================================

#ifndef Svc_DpCatalog_HPP
#define Svc_DpCatalog_HPP

#include "Svc/DpCatalog/DpCatalogComponentAc.hpp"
#include "Svc/DpCatalog/DpRecordSerializableAc.hpp"

#include <Fw/Types/MemAllocator.hpp>

#include <DpCfg.hpp>
#include <DpCatalogCfg.hpp>
#include <Fw/Types/FileNameString.hpp>

namespace Svc {

    class DpCatalog :
        public DpCatalogComponentBase
    {

    public:

        // ----------------------------------------------------------------------
        // Component construction and destruction
        // ----------------------------------------------------------------------

        /// @brief DpCatalog constructor
        /// @param compName component name
        DpCatalog(
            const char* const compName //!< The component name
        );

        /// @brief DpCatalog destructor
        ~DpCatalog();

        /// @brief Configure the DpCatalog
        /// @param maxDpFiles The max number of data product files to track
        /// @param directories list of directories to scan
        /// @param numDirs number of supplied directories
        /// @param stateFile file to store transmit state. Provide a zero-length string if no state tracking
        /// @param memId  memory ID for allocator
        /// @param allocator Allocator to supply memory for catalog. 
        ///        Instance must survive for shutdown to use for reclaiming memory
        void configure(
            Fw::FileNameString directories[DP_MAX_DIRECTORIES],
            FwSizeType numDirs,
            Fw::FileNameString& stateFile,
            NATIVE_UINT_TYPE memId,
            Fw::MemAllocator& allocator
        );

        // @brief clean up component. 
        // Deallocates memory.       
        void shutdown();

    PRIVATE:

        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        //! Handler implementation for fileDone
        //!
        //! File Downlink send complete port
        void fileDone_handler(
            FwIndexType portNum, //!< The port number
            const Svc::SendFileResponse& resp
        ) override;

        //! Handler implementation for pingIn
        //!
        //! Ping input port
        void pingIn_handler(
            FwIndexType portNum, //!< The port number
            U32 key //!< Value to return to pinger
        ) override;

    PRIVATE:

        // ----------------------------------------------------------------------
        // Handler implementations for commands
        // ----------------------------------------------------------------------

        //! Handler implementation for command BUILD_CATALOG
        //!
        //! Build catalog from data product directory
        void BUILD_CATALOG_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq //!< The command sequence number
        ) override;

        //! Handler implementation for command START_XMIT_CATALOG
        //!
        //! Start transmitting catalog
        void START_XMIT_CATALOG_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq, //!< The command sequence number
            Fw::Wait wait //!< have START_XMIT command wait for catalog to complete transmitting
        ) override;

        //! Handler implementation for command STOP_XMIT_CATALOG
        //!
        //! Stop transmitting catalog
        void STOP_XMIT_CATALOG_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq //!< The command sequence number
        ) override;

        //! Handler implementation for command CLEAR_CATALOG
        //!
        //! clear existing catalog
        void CLEAR_CATALOG_cmdHandler(
            FwOpcodeType opCode, //!< The opcode
            U32 cmdSeq //!< The command sequence number
        ) override;


        // ----------------------------------
        // Private data structures
        // ----------------------------------

        struct DpStateEntry {
            FwIndexType dir; //!< index to m_directories entry that has directory name where DP exists
            DpRecord record; //!< data product metadata
        };

        struct DpDstateFileEntry {
            bool used; //!< if the entry is used
            bool visited; //!< used for state file state; indicates that the entry was found in the search of current data products
            DpStateEntry entry; //!< state entry from file
        };

        /// @brief A list sorted in priority order for downlink
        struct DpBtreeNode {
            DpStateEntry entry; //!< pointer to DP record          
            DpBtreeNode* left; //!< left child. Also used for free list
            DpBtreeNode* right; //!< right child
        };

        // ----------------------------------
        // Private helpers
        // ----------------------------------

        /// @brief insert an entry into the sorted list; if it exists, update the metadata
        /// @param entry new entry
        /// @return failed if couldn't find a slot FIXME: Should we just assert? We should never run out.
        bool insertEntry(DpStateEntry& entry);

        /// @brief delete an entry from the sorted list
        /// @param entry new entry
        void deleteEntry(DpStateEntry& entry);

        /// @brief enumeration for check and insert function
        enum CheckStat {
            CHECK_OK, //!< check passed and inserted. Can break loop
            CHECK_CONT, //!< check passed and find another node to check
            CHECK_ERROR, //!< check failed to allocate an entry. Quit function
        };

        /// @brief check for left/right insertion
        CheckStat checkLeftRight(bool condition, DpBtreeNode* &node, const DpStateEntry& newEntry);

        /// @brief reset the free list
        void resetBinaryTree();

        /// #brief fill  the binary tree from DP files
        Fw::CmdResponse fillBinaryTree();

        /// @brief reset the tree stack
        void resetTreeStack();

        /// @brief reset the state file data
        void resetStateFileData();

        /// @brief get file state from the stored state file
        /// @param entry entry to update from file state
        void getFileState(DpStateEntry& entry);

        /// @brief prune the state file data and write the remaining entries back
        void pruneAndWriteStateFile();

        /// @brief load state data from file
        Fw::CmdResponse loadStateFile();

        /// @brief get file state from the stored state file
        /// @param entry entry to add to state file
        void appendFileState(const DpStateEntry& entry);

        /// @brief add an entry to the tree
        /// @param entry entry to add
        /// @return true if a node could be allocated
        bool allocateNode(
            DpBtreeNode* &newNode, 
            const DpStateEntry& newEntry);

        /// @brief send the next entry to file downlink
        void sendNextEntry();

        /// @brief find the next entry in the tree
        /// @return pointer if an entry was found, nullptr if no more entries
        /// @param entry entry to return
        DpBtreeNode* findNextTreeNode();

        /// @brief check to see if component successfully initialized
        /// @return bool if it was initialized
        bool checkInit();

        /// @brief build catalog. Shared between command and port
        /// @return command response for pass/fail
        Fw::CmdResponse doCatalogBuild();

        /// @brief start transmitting catalog. Shared between command and port
        /// @return command response for pass/fail
        Fw::CmdResponse doCatalogXmit();

        // ----------------------------------
        // Private data
        // ----------------------------------
        bool m_initialized; //!< set when the component has been initialized

        DpBtreeNode* m_dpTree; //!< The head of the binary tree
        DpBtreeNode* m_freeListHead; //!< The head of the free list
        DpBtreeNode* m_freeListFoot; //!< The foot of the free list
        DpBtreeNode** m_traverseStack; //!< pointer to memory for stack for traversing tree
        DpBtreeNode* m_currentNode; //!< current node for traversing tree
        DpBtreeNode* m_currentXmitNode; //!< node being currently transmitted

        FwSizeType m_numDpRecords; //!< Stores the actual number of records.
        FwSizeType m_numDpSlots; //!< Stores the available number of record slots.

        Fw::FileNameString m_directories[DP_MAX_DIRECTORIES]; //!< List of supplied DP directories
        FwSizeType m_numDirectories; //!< number of supplied directories
        Fw::String m_fileList[DP_MAX_FILES]; //!< working array of files/directory

        Fw::FileNameString m_stateFile; //!< file to store transmit state
        DpDstateFileEntry* m_stateFileData; //!< DP state loaded from file
        FwSizeType m_stateFileEntries; //!< size of state file data

        NATIVE_UINT_TYPE m_memSize; //!< size of allocated buffer
        void* m_memPtr; //!< stored for shutdown
        NATIVE_UINT_TYPE m_allocatorId; //!< stored for shutdown
        Fw::MemAllocator* m_allocator; //!< stored for shutdown

        bool m_xmitInProgress; //!< set if DP files are in the process of being sent
        FwIndexType m_currStackEntry; //!< current stack entry for traversing tree
        Fw::FileNameString m_currXmitFileName; //!< current file being transmitted
        bool m_xmitCmdWait; //!< true if waiting for transmission complete to complete xmit command
        U64 m_xmitBytes; //!< bytes transmitted for downlink session
        FwOpcodeType m_xmitOpCode; //!< stored xmit command opcode
        U32 m_xmitCmdSeq; //!< stored command sequence id

    };

}

#endif
