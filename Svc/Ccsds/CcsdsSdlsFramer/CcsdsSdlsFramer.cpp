// ======================================================================
// \title  CcsdsSdlsFramer.cpp
// \author devin
// \brief  cpp file for CcsdsSdlsFramer component implementation class
// ======================================================================

#include "Svc/Ccsds/CcsdsSdlsFramer/CcsdsSdlsFramer.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

CcsdsSdlsFramer ::CcsdsSdlsFramer(const char* const compName) : CcsdsSdlsFramerComponentBase(compName) {}

CcsdsSdlsFramer ::~CcsdsSdlsFramer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void CcsdsSdlsFramer ::bufferReturnIn_handler(FwIndexType portNum,
                                              Fw::Buffer& data,
                                              const ComCfg::FrameContext& context) {
    // The encryption helper has returned the original data buffer: send it back upstream
    this->dataReturnOut_out(0, data, context);
}

void CcsdsSdlsFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void CcsdsSdlsFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Determine the security association index: use the context's value when set, otherwise the SA_INDEX parameter
    const U16 unsetSaIndex = ComCfg::FrameContext().get_saIndex();
    U16 saIndex = context.get_saIndex();
    if (saIndex == unsetSaIndex) {
        Fw::ParamValid valid = Fw::ParamValid::INVALID;
        saIndex = this->paramGet_SA_INDEX(valid);
        FW_ASSERT((valid == Fw::ParamValid::VALID) || (valid == Fw::ParamValid::DEFAULT),
                  static_cast<FwAssertArgType>(valid.e));
    }

    // Copy context and record the security association index used for encryption
    ComCfg::FrameContext newContext = context;
    newContext.set_saIndex(saIndex);

    this->encryptOut_out(0, saIndex, data, newContext);
}

void CcsdsSdlsFramer ::dataReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    // dataReturnIn is the allocated frame buffer coming back from the dataOut port
    this->bufferDeallocate_out(0, data);
}

void CcsdsSdlsFramer ::encryptIn_handler(FwIndexType portNum,
                                         const Svc::Ccsds::SdlsStatus& status,
                                         Fw::Buffer& data,
                                         const ComCfg::FrameContext& context) {
    if (status != Svc::Ccsds::SdlsStatus::SUCCESS) {
        this->log_WARNING_HI_EncryptionFailed(status);
        // Drop the frame: return ownership of the buffer to the encryption subsystem
        this->encryptReturnOut_out(0, data, context);
        return;
    }

    FW_ASSERT(data.getSize() <= std::numeric_limits<Fw::Buffer::SizeType>::max() - sizeof(U16),
              static_cast<FwAssertArgType>(data.getSize()));
    const Fw::Buffer::SizeType frameSize = static_cast<Fw::Buffer::SizeType>(data.getSize() + sizeof(U16));

    // Allocate the frame buffer used to prepend the security association index to the encrypted data
    Fw::Buffer frameBuffer = this->bufferAllocate_out(0, frameSize);
    if ((!frameBuffer.isValid()) || (frameBuffer.getSize() < frameSize)) {
        this->log_WARNING_HI_BufferAllocationFailed(frameSize);
        // Drop the frame: return the undersized allocation (when valid) and the encrypted data buffer
        if (frameBuffer.isValid()) {
            this->bufferDeallocate_out(0, frameBuffer);
        }
        this->encryptReturnOut_out(0, data, context);
        return;
    }

    auto frameSerializer = frameBuffer.getSerializer();
    Fw::SerializeStatus serializeStatus = frameSerializer.serializeFrom(context.get_saIndex());
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);
    serializeStatus = frameSerializer.serializeFrom(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    // Trim to actual frame size in case the allocator returned a larger buffer
    frameBuffer.setSize(frameSize);

    // Return ownership of the encrypted data buffer to the encryption helper, then send the frame
    this->encryptReturnOut_out(0, data, context);
    this->dataOut_out(0, frameBuffer, context);
}

}  // namespace Ccsds

}  // namespace Svc
