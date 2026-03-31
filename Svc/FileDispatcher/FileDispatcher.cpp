// ======================================================================
// \title  FileDispatcher.cpp
// \author tcanham
// \brief  cpp file for FileDispatcher component implementation class
// ======================================================================

#include "Svc/FileDispatcher/FileDispatcher.hpp"
#include "Fw/Types/StringUtils.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FileDispatcher ::FileDispatcher(const char* const compName) : FileDispatcherComponentBase(compName) {
    // disable entries
    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE; i++) {
        this->m_dispatchTable.entries[i].enabled = false;
    }
    this->m_dispatchTable.numEntries = 0;
}

FileDispatcher ::~FileDispatcher() {}

void FileDispatcher ::configure(const FileDispatcherTable& table) {
    // validate table
    FW_ASSERT(table.numEntries <= Svc::FileDispatcherCfg::FILE_DISPATCHER_MAX_TABLE_SIZE);
    for (FwSizeType entry = 0; entry < table.numEntries; entry++) {
        FW_ASSERT(table.entries[entry].port.isValid(),
                  table.entries[entry].port.e);  // valid output port
        FW_ASSERT(table.entries[entry].fileExt.length() > 0,
                  static_cast<FwAssertArgType>(table.entries[entry].fileExt.length()));  // non-zero length
        // Copy over table entry
        this->m_dispatchTable.entries[entry].port = table.entries[entry].port;
        this->m_dispatchTable.entries[entry].fileExt = table.entries[entry].fileExt;
        this->m_dispatchTable.entries[entry].enabled = table.entries[entry].enabled;
    }
    // Set number of entries
    this->m_dispatchTable.numEntries = table.numEntries;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void FileDispatcher ::fileAnnounceRecv_handler(FwIndexType portNum, Fw::StringBase& file_name) {
    // determine file extension and dispatch accordingly

    // walk table to find match
    for (FwSizeType i = 0; i < this->m_dispatchTable.numEntries; i++) {
        if (!this->m_dispatchTable.entries[i].enabled) {
            continue;
        }

        auto loc = Fw::StringUtils::substring_find_last(file_name.toChar(), file_name.length(),
                                                        this->m_dispatchTable.entries[i].fileExt.toChar(),
                                                        this->m_dispatchTable.entries[i].fileExt.length());

        if ((loc != -1) &&  // matches at all
            (file_name.length() - this->m_dispatchTable.entries[i].fileExt.length() ==
             static_cast<FwSizeType>(loc))  // match at end of string
        ) {
            // dispatch on this port
            this->fileDispatch_out(this->m_dispatchTable.entries[i].port.e, file_name);
            this->log_ACTIVITY_HI_FileDispatched(file_name, this->m_dispatchTable.entries[i].port);
        }
    }
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void FileDispatcher ::ENABLE_DISPATCH_cmdHandler(FwOpcodeType opCode,
                                                 U32 cmdSeq,
                                                 Svc::FileDispatcherCfg::FileDispatchPort file_type,
                                                 Fw::Enabled enable) {
    for (FwSizeType i = 0; i < this->m_dispatchTable.numEntries; i++) {
        if (this->m_dispatchTable.entries[i].port == file_type) {
            this->m_dispatchTable.entries[i].enabled = (enable == Fw::Enabled::ENABLED);
        }
    }
    this->log_ACTIVITY_HI_FileDispatchState(file_type, enable);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void FileDispatcher ::pingIn_handler(FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

}  // namespace Svc
