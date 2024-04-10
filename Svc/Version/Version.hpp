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
      Version(
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
  };

}

#endif
