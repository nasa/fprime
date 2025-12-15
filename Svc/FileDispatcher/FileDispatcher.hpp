// ======================================================================
// \title  FileDispatcher.hpp
// \author tcanham
// \brief  hpp file for FileDispatcher component implementation class
// ======================================================================

#ifndef Svc_FileDispatcher_HPP
#define Svc_FileDispatcher_HPP

#include "Svc/FileDispatcher/FileDispatcherComponentAc.hpp"

namespace Svc {

struct FileDispatcherEntry {
    Fw::String fileExt;                             // file extension for dispatch
    Svc::FileDispatcherCfg::FileDispatchPort port;  // port to dispatch to
    bool enabled;                                   // whether dispatching is enabled for this type
};

class FileDispatcher final : public FileDispatcherComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FileDispatcher object
    FileDispatcher(const char* const compName  //!< The component name
    );

    //! Destroy FileDispatcher object
    ~FileDispatcher();

    //! configure the component
    void configure(FileDispatcherEntry* entries,  //!< array of entries
                   FwSizeType numEntries          //!< number of entries
    );

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for fileRecv
    //!
    //! Port for receiving files to dispatch
    void fileAnnounceRecv_handler(FwIndexType portNum,       //!< The port number
                                  Fw::StringBase& file_name  //!< The successfully uplinked file
                                  ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command DISABLE_DISPATCH
    void ENABLE_DISPATCH_cmdHandler(
        FwOpcodeType opCode,                                 //!< The opcode
        U32 cmdSeq,                                          //!< The command sequence number
        Svc::FileDispatcherCfg::FileDispatchPort file_type,  //!< the file type dispatch to disable
        Fw::Enabled enable                                   //!< whether to enable or disable dispatch
        ) override;

    //! Handler implementation for pingIn
    //!
    //! Ping in
    void pingIn_handler(FwIndexType portNum,  //!< The port number
                        U32 key               //!< Value to return to pinger
                        ) override;

    //! table of dispatch entries
    FileDispatcherEntry m_dispatchTable[Svc::FileDispatcherCfg::FileDispatchPort::MAX_FILE_DISPATCH_PORTS];
};

}  // namespace Svc

#endif
