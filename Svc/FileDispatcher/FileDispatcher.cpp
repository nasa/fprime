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
    for (FwSizeType i = 0; i < Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS; i++) {
        this->m_dispatchTable[i].enabled = false;
    }
}

FileDispatcher ::~FileDispatcher() {}

void FileDispatcher ::configure(FileDispatcherEntry* entries, FwSizeType numEntries) {
    FW_ASSERT(entries != nullptr);
    FW_ASSERT(numEntries > 0);
    FW_ASSERT(numEntries <= Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS);

    // copy entries into table
    for (FwSizeType i = 0; i < numEntries && i < Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS; i++) {
        this->m_dispatchTable[i] = entries[i];
    }
    // disable remaining entries
    for (FwSizeType i = numEntries; i < Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS; i++) {
        this->m_dispatchTable[i].enabled = false;
    }
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void FileDispatcher ::fileAnnounceRecv_handler(FwIndexType portNum, Fw::StringBase& file_name) {
    // determine file extension and dispatch accordingly

    // walk table to find match
    for (FwIndexType i = 0; i < Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS; i++) {
        if (this->m_dispatchTable[i].enabled) {
            if (Fw::StringUtils::substring_find(
                file_name.toChar(),file_name.length(),
                    this->m_dispatchTable[i].fileExt.toChar(),
                    this->m_dispatchTable[i].fileExt.length()
                ) != -1) {
                // dispatch on this port
                this->fileDispatch_out(this->m_dispatchTable[i].port, file_name);
                this->log_ACTIVITY_HI_FileDispatched( file_name, static_cast<Svc::FileDispatcherCfg::FileDispatchPort::T>(i));
                return; // dispatched, return   
            }
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
    this->m_dispatchTable[file_type].enabled = enable;
    this->log_ACTIVITY_LO_FileDispatchStateEvent(file_type, enable);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
