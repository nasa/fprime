// ======================================================================
// \title  DpCatalog.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component implementation class
// ======================================================================

#include "FpConfig.hpp"
#include "Svc/DpCatalog/DpCatalog.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  DpCatalog ::
    DpCatalog(const char* const compName) :
      DpCatalogComponentBase(compName)
  {

  }

  DpCatalog ::
    ~DpCatalog()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void DpCatalog ::
    pingIn_handler(
        NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void DpCatalog ::
    BUILD_CATALOG_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void DpCatalog ::
    START_XMIT_CATALOG_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void DpCatalog ::
    STOP_XMIT_CATALOG_cmdHandler(
        FwOpcodeType opCode,
        U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

}
