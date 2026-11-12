// ======================================================================
// \title  Version.hpp
// \author sreddy
// \brief  hpp file for Version component implementation class
// ======================================================================

#ifndef Svc_Version_HPP
#define Svc_Version_HPP

#include "Svc/Version/VersionComponentAc.hpp"

namespace Svc {

class Version final : public VersionComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Version object
    explicit Version(const char* const compName  //!< The component name
    );

    //! Destroy Version object
    ~Version();

    //! configure version's verbosity
    void config(bool enable);

    //! send initial version tlm and events
    void start();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for getVersion
    //!
    //! Mutexed Port to get values
    void getVersion_handler(FwIndexType portNum,                             //!< The port number
                            const Svc::VersionCfg::VersionEnum& version_id,  //!< The entry to access
                            Fw::StringBase& version_string,                  //!< The value to be passed
                            Svc::VersionStatus& status                       //!< The command response argument
                            ) override;

    //! Handler implementation for setVersion
    //!
    //! Mutexed Port to set values
    void setVersion_handler(FwIndexType portNum,                             //!< The port number
                            const Svc::VersionCfg::VersionEnum& version_id,  //!< The entry to access
                            Fw::StringBase& version_string,                  //!< The value to be passed
                            Svc::VersionStatus& status                       //!< The command response argument
                            ) override;

    //! Handler implementation for schedIn
    //!
    //! Scheduled port to periodically re-emit all version information
    void schedIn_handler(FwIndexType portNum,  //!< The port number
                         U32 context           //!< The call order
                         ) override;

    //! \struct verId_db
    //! \brief Custom Version database structure
    //!
    //! Stores the latest custom version values set through the setVersion port.
    //! The constructor initializes every entry to "no_ver" with status VersionStatus::FAILURE.
    //!
    CustomVersionDb verId_db[Svc::VersionCfg::VersionEnum::NUM_CONSTANTS];

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command ENABLE
    //!
    //! A command to enable or disable Version telemetry
    void ENABLE_cmdHandler(FwOpcodeType opCode,               //!< The opcode
                           U32 cmdSeq,                        //!< The command sequence number
                           const Svc::VersionEnabled& enable  //!< whether or not Version telemetry is enabled
                           ) override;

    //! Handler implementation for command VERSION
    //!
    //! Report version as event
    void VERSION_cmdHandler(FwOpcodeType opCode,                  //!< The opcode
                            U32 cmdSeq,                           //!< The command sequence number
                            const Svc::VersionType& version_type  //!< which version type event is requested
                            ) override;

  private:
    // An enumeration for TLM slot access
    enum VersionSlot {
        VER_SLOT_00 = 0,
        VER_SLOT_01 = 1,
        VER_SLOT_02 = 2,
        VER_SLOT_03 = 3,
        VER_SLOT_04 = 4,
        VER_SLOT_05 = 5,
        VER_SLOT_06 = 6,
        VER_SLOT_07 = 7,
        VER_SLOT_08 = 8,
        VER_SLOT_09 = 9
    };

    // function to log framework version channel, and event when sendEvent is true
    void fwVersion_tlm(bool sendEvent = true);
    // function to log project version channel, and event when sendEvent is true
    void projectVersion_tlm(bool sendEvent = true);
    // function to log library version channels, and events when sendEvent is true
    void libraryVersion_tlm(bool sendEvent = true);
    // function to log custom version channels (if verbosity is enabled), and events when sendEvent is true
    void customVersion_tlm(VersionSlot custom_slot, bool sendEvent = true);
    void customVersion_tlm_all(bool sendEvent = true);
    bool m_enable;             /*!<Send TLM when true>*/
    U8 m_num_custom_elements;  // number of custom versions
};

}  // namespace Svc

#endif
