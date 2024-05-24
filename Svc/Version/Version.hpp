// ======================================================================
// \title  Version.hpp
// \author sreddy
// \brief  hpp file for Version component implementation class
// ======================================================================

#ifndef Svc_Version_HPP
#define Svc_Version_HPP

#include "Svc/Version/VersionComponentAc.hpp"

namespace Svc {

  class Version :
    public VersionComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct Version object
      explicit Version(
          const char* const compName //!< The component name
      );

      //! Destroy Version object
      ~Version();

      //! configure version's verbosity and startup
      void Config (bool enable);

    PRIVATE:
        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        // No longer using a scheduler but driven by command only
        //! Handler implementation for run
        //!
        //void
        //run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
        //            U32 context                    /*!< The call order*/
        //) override;
        
        //! Handler implementation for getVersion
        //!
        //! Mutexed Port to get values
        void getVersion_handler(
            FwIndexType portNum, //!< The port number
            const Svc::VersionCfg::VersionEnum& version_id, //!< The entry to access
            Svc::VersPortStrings::StringSize80& version_string, //!< The value to be passed
            Svc::VersionStatus& status //!< The command response argument
        ) override;

        //! Handler implementation for setVersion
        //!
        //! Mutexed Port to set values
        void setVersion_handler(
            FwIndexType portNum, //!< The port number
            const Svc::VersionCfg::VersionEnum& version_id, //!< The entry to access
            Svc::VersPortStrings::StringSize80& version_string, //!< The value to be passed
            Svc::VersionStatus& status //!< The command response argument
        ) override;

        //! \struct verId_db
        //! \brief Custom Version database structure
        //!
        //! This structure stores the latest values of the measurements.
        //! The statuses are all initialized to MeasurementStatus::STALE by the constructor.
        //!
        /*
        struct CusVerArr {
            Svc::VersionCfg::VersionEnum version_id; //!< The entry to access
            VersPortStrings::StringSize80  val; //!< the last value of the measurement
            VersionStatus status; //!< last status of measurement
        } */
        CusVerDb verId_db[Svc::VersionCfg::VersionEnum::NUM_CONSTANTS];

    
    private:
      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------

      //! Handler implementation for command ENABLE
      //!
      //! A command to enable or disable Version telemetry
      void ENABLE_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          Svc::VersionEnabled enable //!< whether or not Version telemetry is enabled
      ) override;

      //! Handler implementation for command VERSION
      //!
      //! Report version as EVR
      void VERSION_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          Svc::VersionType version_type //!< which version type EVR is requested
      ) override;

    PRIVATE:
    
   // An enumeration for TLM slot access
    enum VerSlot {
        VER_SLOT_00 = 0,
        VER_SLOT_01,
        VER_SLOT_02,
        VER_SLOT_03,
        VER_SLOT_04,
        VER_SLOT_05,
        VER_SLOT_06,
        VER_SLOT_07,
        VER_SLOT_08,
        VER_SLOT_09
    };

    void proc_libver();
    void proc_cusver();
    void FwVer_tlm();
    void ProjVer_tlm();
    void LibVer_tlm();
    void CusVer_tlm(VerSlot cus_slot);
    void CusVer_tlm_all();
    bool m_enable;       /*!<Send TLM when true>*/
    bool startup_done;
    U32 num_lib_elem; //number of library versions
    U32 num_cus_elem; //number of custom versions
    //const char* lib_ver_arr[]; // Store library versions internally


  };

}

#endif
