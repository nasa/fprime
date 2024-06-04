// ======================================================================
// \title  Version.cpp
// \author sreddy
// \brief  cpp file for Version component implementation class
// ======================================================================

#include "FpConfig.hpp"
#include "Svc/Version/Version.hpp"

#ifdef BUILD_UT
    #include "test/ut/versions/version.hpp" //autogenerated file containing hardcoded project and framework versions
#else
    #include "versions/version.hpp" //autogenerated file containing hardcoded project and framework versions
#endif

namespace Svc {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  Version ::
    Version(const char* const compName) :
      VersionComponentBase(compName)
  {
        m_startup_done = false;
        m_num_lib_elem = 0;
        m_num_cus_elem = 0;
        m_enable = false;
        Svc::VersionPortStrings::StringSize80 ver_str = "no_ver";
        // initialize all custom entries
        for (FwIndexType id = 0; id < Svc::VersionCfg::VersionEnum::NUM_CONSTANTS; id++) {
            //setver_enum is by default set to the first enum value, so not setting it here
            verId_db[id].setversion_value(ver_str);
            verId_db[id].setversion_status(VersionStatus::FAILURE);
        }
  }

  Version ::
    ~Version()
  {

  }
 
  void Version::config(bool enable) {
    //Set Verbosity for custom versions 
    m_enable = enable;

    
    //Setup and send startup TLM
    this->fwVersion_tlm();
    this->projectVersion_tlm();
    this->libraryVersion_tlm();
  }
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Version ::
    getVersion_handler(
        FwIndexType portNum,
        const Svc::VersionCfg::VersionEnum& version_id,
        Svc::VersionPortStrings::StringSize80& version_string,
        Svc::VersionStatus& status
    )
  {
        FW_ASSERT(version_id.isValid(),version_id.e);
        U8 version_slot = VersionSlot(version_id.e);
        version_string = this->verId_db[version_slot].getversion_value();
        status = this->verId_db[version_slot].getversion_status();
  }

  void Version ::
    setVersion_handler(
        FwIndexType portNum,
        const Svc::VersionCfg::VersionEnum& version_id,
        Svc::VersionPortStrings::StringSize80& version_string,
        Svc::VersionStatus& status
    )
  {
        FW_ASSERT(version_id.isValid(),version_id.e);
        VersionSlot ver_slot = VersionSlot(version_id.e);
        this->verId_db[ver_slot].setversion_enum(version_id);
        this->verId_db[ver_slot].setversion_value(version_string);
        this->verId_db[ver_slot].setversion_status(status);
        this->m_num_cus_elem++;
        this->customVersion_tlm(ver_slot);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

   void Version ::
    ENABLE_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Svc::VersionEnabled enable
    )
  {
    m_enable = (enable == VersionEnabled::ENABLED);
    
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  //Command handler to event versions
  void Version ::
    VERSION_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Svc::VersionType version_type
    )
  {
    //FW_ASSERT(version_type <= Svc::VersionType::ALL);
    switch(version_type) {
        case (Svc::VersionType::PROJECT):
            this->projectVersion_tlm();
            break;
        
        case (Svc::VersionType::FRAMEWORK):
            this->fwVersion_tlm();
            break;
        
        case (Svc::VersionType::LIBRARY):
            this->libraryVersion_tlm();
            break;
        
        case (Svc::VersionType::CUSTOM):
            this->customVersion_tlm_all();
            break;
        
        case (Svc::VersionType::ALL):
            this->projectVersion_tlm();
            this->fwVersion_tlm();
            this->libraryVersion_tlm();
            this->customVersion_tlm_all();
            break;
        default:
            FW_ASSERT(0,version_type);
            break; 
    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }
  // ----------------------------------------------------------------------
  // implementations for internal functions 
  // ----------------------------------------------------------------------
  //Process libsv
  void Version :: process_libraryVersion() {
     m_num_lib_elem = static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(Project::Version::LIBRARY_VERSIONS));
     //m_num_lib_elem = sizeof(Project::Version::LIBRARY_VERSIONS)/sizeof(Project::Version::LIBRARY_VERSIONS[0]);
     if(Project::Version::LIBRARY_VERSIONS[0] == nullptr) {
        m_num_lib_elem = 0;
     }
  }
  
  //functions to log tlm on versions
  void Version :: fwVersion_tlm() {
    Fw::LogStringArg fw_event = (Project::Version::FRAMEWORK_VERSION);
    this->log_ACTIVITY_LO_FrameworkVersion(fw_event);
    Fw::TlmString fw_tlm = (Project::Version::FRAMEWORK_VERSION);
    this->tlmWrite_FrameworkVersion(fw_tlm);
  }

  void Version :: projectVersion_tlm() {
    Fw::LogStringArg proj_event = Project::Version::PROJECT_VERSION;
    this->log_ACTIVITY_LO_ProjectVersion(proj_event);
    Fw::TlmString proj_tlm = Project::Version::PROJECT_VERSION;
    this->tlmWrite_ProjectVersion(proj_tlm);
  }

  void Version :: libraryVersion_tlm() {
    //Process libraries array
    this->process_libraryVersion();

    for (U8 i = 0; i < m_num_lib_elem; i++) {
        //Emit Event/TLM on library versions
        this->log_ACTIVITY_LO_LibraryVersions(Fw::LogStringArg(Project::Version::LIBRARY_VERSIONS[i]));
        //Write to Events
        switch(i) {
            case VER_SLOT_00:
                this->tlmWrite_LibraryVersion01(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_01:
                this->tlmWrite_LibraryVersion02(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_02:
                this->tlmWrite_LibraryVersion03(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_03:
                this->tlmWrite_LibraryVersion04(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_04:
                this->tlmWrite_LibraryVersion05(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_05:
                this->tlmWrite_LibraryVersion06(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_06:
                this->tlmWrite_LibraryVersion07(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_07:
                this->tlmWrite_LibraryVersion08(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_08:
                this->tlmWrite_LibraryVersion09(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            case VER_SLOT_09:
                this->tlmWrite_LibraryVersion10(Fw::TlmString(Project::Version::LIBRARY_VERSIONS[i]));
                break;
            default:
                //It is possible to have more than 10 library versions; however design agreed to only 
                // provide 10 TLM channels for it
                break;
        }
    }

  }

  void Version :: customVersion_tlm_all() {
    for ( U8 i = 0; 
          (m_enable == true) && (m_num_cus_elem != 0) && (i < Svc::VersionCfg::VersionEnum::NUM_CONSTANTS);
          i++) {
           Version::customVersion_tlm(VersionSlot(i)); 
    }
  }

  void Version :: customVersion_tlm(VersionSlot custom_slot) {
    
    //Process custom version TLM only if verbosity is enabled and there are any valid writes to it; 
    // it doesn't necessarily have to be consecutive
        if( (this->verId_db[custom_slot].getversion_value() != "no_ver")
            && m_enable == true
            && (m_num_cus_elem > 0)) { //Write TLM for valid writes
            
            //Emit Events/TLM on library versions
            this->log_ACTIVITY_LO_CustomVersions(this->verId_db[custom_slot].getversion_enum(), this->verId_db[custom_slot].getversion_value());

            //Write to TLM
            switch(custom_slot) {
                case VER_SLOT_00:
                    this->tlmWrite_CustomVersion01(verId_db[custom_slot]);
                    break;
                case VER_SLOT_01:
                    this->tlmWrite_CustomVersion02(verId_db[custom_slot]);
                    break;
                case VER_SLOT_02:
                    this->tlmWrite_CustomVersion03(verId_db[custom_slot]);
                    break;
                case VER_SLOT_03:
                    this->tlmWrite_CustomVersion04(verId_db[custom_slot]);
                    break;
                case VER_SLOT_04:
                    this->tlmWrite_CustomVersion05(verId_db[custom_slot]);
                    break;
                case VER_SLOT_05:
                    this->tlmWrite_CustomVersion06(verId_db[custom_slot]);
                    break;
                case VER_SLOT_06:
                    this->tlmWrite_CustomVersion07(verId_db[custom_slot]);
                    break;
                case VER_SLOT_07:
                    this->tlmWrite_CustomVersion08(verId_db[custom_slot]);
                    break;
                case VER_SLOT_08:
                    this->tlmWrite_CustomVersion09(verId_db[custom_slot]);
                    break;
                case VER_SLOT_09:
                    this->tlmWrite_CustomVersion10(verId_db[custom_slot]);
                    break;
                default:
                    //There are only 10 custom slots available 
                    FW_ASSERT(0,custom_slot);
                    break;
            }
        }
    }

}
