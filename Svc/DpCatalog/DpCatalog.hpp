// ======================================================================
// \title  DpCatalog.hpp
// \author tcanham
// \brief  hpp file for DpCatalog component implementation class
// ======================================================================

#ifndef Svc_DpCatalog_HPP
#define Svc_DpCatalog_HPP

#include "Svc/DpCatalog/DpCatalogComponentAc.hpp"
#include <Fw/Types/MemAllocator.hpp>

#include <DpCfg.hpp>
#include <DpCatalogCfg.hpp>

namespace Svc {

  class DpCatalog :
    public DpCatalogComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct DpCatalog object
      DpCatalog(
          const char* const compName //!< The component name
      );

      //! Destroy DpCatalog object
      ~DpCatalog();


      //! Configure component
      //! FIXME: Put memory computation size here for users
      void configure(
          FwNativeUIntType maxDpFiles, //!< The max number of data product files to track
          Fw::String directories[DP_MAX_DIRECTORIES], //!< list of directories to scan
          FwNativeUIntType numDirs, //!< number of supplied directories
          Fw::MemAllocator allocator //!< Allocator to supply memory for catalog
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for pingIn
      //!
      //! Ping input port
      void pingIn_handler(
          NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

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
      );

      //! Handler implementation for command START_XMIT_CATALOG
      //!
      //! Start transmitting catalog
      void START_XMIT_CATALOG_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      //! Handler implementation for command STOP_XMIT_CATALOG
      //!
      //! Stop transmitting catalog
      void STOP_XMIT_CATALOG_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq //!< The command sequence number
      );

      // ----------------------------------
      // Pivate data structures
      // ----------------------------------
      struct DpRecord {
          bool entry; //!< entry exists
          FwNativeUIntType dir; //!< index to m_directory directory name where DP is stored
          FwNativeUIntType id; //!< DP priority for sorting
          FwNativeUIntType tSec; //!< DP time in seconds
          FwNativeUIntType tSub; //!< DP time in seconds
          FwNativeUIntType priority; //!< DP priority for sorting
      };

      // private helpers

      // Private data
      Fw::String m_directories[DP_MAX_DIRECTORIES]; //!< Stores DP directories

  };

}

#endif
