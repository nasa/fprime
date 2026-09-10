// ======================================================================
// \title  ClearTextDecryptor.cpp
// \author lestarch-autobot
// \brief  cpp file for ClearTextDecryptor component implementation class
// ======================================================================

#include "Svc/Ccsds/ClearTextDecryptor/ClearTextDecryptor.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ClearTextDecryptor ::ClearTextDecryptor(const char* const compName) : ClearTextDecryptorComponentBase(compName) {}

ClearTextDecryptor ::~ClearTextDecryptor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ClearTextDecryptor ::decryptIn_handler(FwIndexType portNum,
                                            U16 securityAssociationIndex,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    // Make the absence of security visible: a flight configuration must never route traffic here
    this->log_WARNING_HI_NullCipherInUse(securityAssociationIndex);
    this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, data, context);
}

void ClearTextDecryptor ::decryptReturnIn_handler(FwIndexType portNum,
                                                  Fw::Buffer& data,
                                                  const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
