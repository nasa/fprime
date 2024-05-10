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

    PRIVATE:
        // ----------------------------------------------------------------------
        // Handler implementations for user-defined typed input ports
        // ----------------------------------------------------------------------

        //! Handler implementation for run
        //!
        void
        run_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                    U32 context                    /*!< The call order*/
        ) override;
        
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

        //! \struct t_dbStruct
        //! \brief PolyDb database structure
        //!
        //! This structure stores the latest values of the measurements.
        //! The statuses are all initialized to MeasurementStatus::STALE by the constructor.
        //!

        struct verArr {
            VersionStatus status; //!< last status of measurement
            VersPortStrings::StringSize80  val; //!< the last value of the measurement
        } verId_db[Svc::VersionCfg::VersionEnum::NUM_CONSTANTS];

    
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
          U32 cmdSeq //!< The command sequence number
      ) override;

    private:
    void Version_tlm();
    bool m_enable;       /*!<Send TLM when true>*/
    bool startup_done;
  };

}

#endif
