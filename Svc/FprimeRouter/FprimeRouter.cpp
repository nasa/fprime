// ======================================================================
// \title  FprimeRouter.cpp
// \author thomas-bc
// \brief  cpp file for FprimeRouter component implementation class
// ======================================================================

#include "Svc/FprimeRouter/FprimeRouter.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Logger/Logger.hpp"
#include "config/ApidEnumAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FprimeRouter ::FprimeRouter(const char* const compName) : FprimeRouterComponentBase(compName) {
    // Mark every table entry unused (key == nullptr)
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(this->m_bufferContextTable); i++) {
        this->m_bufferContextTable[i].key = nullptr;
    }
}

FprimeRouter ::~FprimeRouter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FprimeRouter ::dataIn_handler(FwIndexType portNum, Fw::Buffer& packetBuffer, const ComCfg::FrameContext& context) {
    Fw::SerializeStatus status;
    Fw::ComPacketType packetType = context.get_apid();
    // Route based on received APID (packet type)
    switch (packetType) {
        // Handle a command packet
        case Fw::ComPacketType::FW_PACKET_COMMAND: {
            // Allocate a com buffer on the stack
            Fw::ComBuffer com;
            // Copy the contents of the packet buffer into the com buffer
            status = com.setBuff(packetBuffer.getData(), packetBuffer.getSize());
            if (status == Fw::FW_SERIALIZE_OK) {
                // Send the com buffer - critical functionality so it is considered an error not to
                // have the port connected. This is why we don't check isConnected() before sending.
                this->commandOut_out(0, com, 0);
            } else {
                this->log_WARNING_HI_SerializationError(status);
            }
            // The command buffer was copied into the com buffer above, so ownership of the
            // incoming packetBuffer is returned immediately with the context it was received with.
            this->dataReturnOut_out(0, packetBuffer, context);
            break;
        }
        // Handle a file packet
        case Fw::ComPacketType::FW_PACKET_FILE: {
            // If the file uplink output port is connected, send the file packet directly.
            // Ownership is passed to the receiver and will come back on fileBufferReturnIn,
            // at which point we return it to the deframer via dataReturnOut. fileOut carries
            // only Fw::Buffer, so remember the context here to restore it on return.
            if (this->isConnected_fileOut_OutputPort(0)) {
                if (this->insertContext(packetBuffer, context) == Fw::Success::FAILURE) {
                    this->log_WARNING_HI_FileOutContextTableFull();
                }
                this->fileOut_out(0, packetBuffer);
            } else {
                // Port not connected, return the buffer immediately with its context
                this->dataReturnOut_out(0, packetBuffer, context);
            }
            break;
        }
        default: {
            // Packet type is not known to the F Prime protocol. If the unknownDataOut port is
            // connected, forward packet and context for further processing.
            // Ownership is passed to the receiver and will come back on fileBufferReturnIn,
            // at which point we return it to the deframer via dataReturnOut. The return path
            // (fileBufferReturnIn) carries no context, so remember it here to restore on return.
            if (this->isConnected_unknownDataOut_OutputPort(0)) {
                if (this->insertContext(packetBuffer, context) == Fw::Success::FAILURE) {
                    this->log_WARNING_HI_UnknownDataOutContextTableFull();
                }
                this->unknownDataOut_out(0, packetBuffer, context);
            } else {
                // Port not connected, return the buffer immediately with its context
                this->dataReturnOut_out(0, packetBuffer, context);
            }
            break;
        }
    }
}

void FprimeRouter ::cmdResponseIn_handler(FwIndexType portNum,
                                          FwOpcodeType opcode,
                                          U32 cmdSeq,
                                          const Fw::CmdResponse& response) {
    // Nothing to do
}

void FprimeRouter ::fileBufferReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // Restore the context that was saved when this buffer was handed off on
    // fileOut/unknownDataOut
    ComCfg::FrameContext context;
    if (this->takeContext(fwBuffer, context) == Fw::Success::FAILURE) {
        // Buffer not found in the table: return with an empty context (already default)
        this->log_WARNING_HI_BufferContextNotFound();
    }
    this->dataReturnOut_out(0, fwBuffer, context);
}

// ----------------------------------------------------------------------
// Buffer-to-context association table helpers
// ----------------------------------------------------------------------

Fw::Success FprimeRouter ::insertContext(const Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    const U8* key = buffer.getData();
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(this->m_bufferContextTable); i++) {
        if (this->m_bufferContextTable[i].key == nullptr) {
            this->m_bufferContextTable[i].key = key;
            this->m_bufferContextTable[i].context = context;
            return Fw::Success::SUCCESS;
        }
    }
    // Table full
    return Fw::Success::FAILURE;
}

Fw::Success FprimeRouter ::takeContext(const Fw::Buffer& buffer, ComCfg::FrameContext& context) {
    const U8* key = buffer.getData();
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(this->m_bufferContextTable); i++) {
        if (this->m_bufferContextTable[i].key == key) {
            context = this->m_bufferContextTable[i].context;
            this->m_bufferContextTable[i].key = nullptr;
            return Fw::Success::SUCCESS;
        }
    }
    // Not found
    return Fw::Success::FAILURE;
}

}  // namespace Svc
