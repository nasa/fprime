// ======================================================================
// \title  CcsdsSdlsDeframer.cpp
// \author mstarch
// \brief  cpp file for CcsdsSdlsDeframer component implementation class
// ======================================================================

#include "Svc/Ccsds/CcsdsSdlsDeframer/CcsdsSdlsDeframer.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CcsdsSdlsDeframer ::CcsdsSdlsDeframer(const char* const compName) : CcsdsSdlsDeframerComponentBase(compName) {}

CcsdsSdlsDeframer ::~CcsdsSdlsDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void CcsdsSdlsDeframer ::bufferReturnIn_handler(FwIndexType portNum,
                                                Fw::Buffer& data,
                                                const ComCfg::FrameContext& context) {
    // The decryption helper has returned the original frame buffer: send it back upstream
    this->dataReturnOut_out(0, data, context);
}

void CcsdsSdlsDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    if (data.getSize() < sizeof(U16)) {
        // The frame is not long enough to contain the security association index, so we cannot process it
        this->log_WARNING_HI_InsufficientLength();
        this->dataReturnOut_out(0, data, context);  // Drop the frame
    } else {
        U16 saIndex = 0;
        Fw::SerializeStatus deserializeStatus = data.getDeserializer().deserializeTo(saIndex);
        FW_ASSERT(deserializeStatus == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(deserializeStatus));

        // Copy context and set the security association index to the value we just deserialized from the frame
        ComCfg::FrameContext newContext = context;
        newContext.set_saIndex(saIndex);

        // Remove the security association index: the decryption helper receives only the iv/data
        data.advance(static_cast<FwSignedSizeType>(sizeof(U16)));

        this->decryptOut_out(0, saIndex, data, newContext);
    }
}

void CcsdsSdlsDeframer ::dataReturnIn_handler(FwIndexType portNum,
                                              Fw::Buffer& data,
                                              const ComCfg::FrameContext& context) {
    // Pass the data back to the decryption subsystem when it is returned from the rest of the chain
    this->decryptReturnOut_out(0, data, context);
}

void CcsdsSdlsDeframer ::decryptIn_handler(FwIndexType portNum,
                                           const Svc::Ccsds::SdlsStatus& status,
                                           Fw::Buffer& data,
                                           const ComCfg::FrameContext& context) {
    if (status != Svc::Ccsds::SdlsStatus::SUCCESS) {
        this->log_WARNING_HI_DecryptionFailed(status);
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Svc::Ccsds::FrameError::SDLS_DECRYPTION_FAILURE);
        }
        // Return ownership of the failed buffer to the decryption subsystem
        this->decryptReturnOut_out(0, data, context);
    } else {
        // Once the decryption has finished, we can route the decryption output to the dataOut port for further
        // processing
        this->dataOut_out(0, data, context);
    }
}

}  // namespace Ccsds

}  // namespace Svc
