// ======================================================================
// \title  ClearTextEncryptor.cpp
// \author lestarch-autobot
// \brief  cpp file for ClearTextEncryptor component implementation class
// ======================================================================

#include "Svc/Ccsds/ClearTextEncryptor/ClearTextEncryptor.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ClearTextEncryptor ::ClearTextEncryptor(const char* const compName) : ClearTextEncryptorComponentBase(compName) {}

ClearTextEncryptor ::~ClearTextEncryptor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ClearTextEncryptor ::encryptIn_handler(FwIndexType portNum,
                                            U16 securityAssociationIndex,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    this->encryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, data, context);
}

void ClearTextEncryptor ::encryptReturnIn_handler(FwIndexType portNum,
                                                  Fw::Buffer& data,
                                                  const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
