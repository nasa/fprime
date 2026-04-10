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

FprimeRouter ::FprimeRouter(const char* const compName) : FprimeRouterComponentBase(compName) {}

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
            // Return ownership of the incoming packetBuffer with an empty context
            ComCfg::FrameContext emptyContext;
            this->dataReturnOut_out(0, packetBuffer, emptyContext);
            break;
        }
        // Handle a file packet
        case Fw::ComPacketType::FW_PACKET_FILE: {
            // If the file uplink output port is connected, send the file packet directly.
            // Ownership is passed to the receiver and will come back on fileBufferReturnIn,
            // at which point we return it to the deframer via dataReturnOut.
            if (this->isConnected_fileOut_OutputPort(0)) {
                this->fileOut_out(0, packetBuffer);
            } else {
                // Port not connected, return the buffer immediately with an empty context
                ComCfg::FrameContext emptyContext;
                this->dataReturnOut_out(0, packetBuffer, emptyContext);
            }
            break;
        }
        default: {
            // Packet type is not known to the F Prime protocol. If the unknownDataOut port is
            // connected, forward packet and context for further processing.
            // Ownership is passed to the receiver and will come back on fileBufferReturnIn,
            // at which point we return it to the deframer via dataReturnOut.
            if (this->isConnected_unknownDataOut_OutputPort(0)) {
                this->unknownDataOut_out(0, packetBuffer, context);
            } else {
                // Port not connected, return the buffer immediately with an empty context
                ComCfg::FrameContext emptyContext;
                this->dataReturnOut_out(0, packetBuffer, emptyContext);
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
    // Return ownership of the buffer to the deframer with an empty context
    ComCfg::FrameContext context;
    this->dataReturnOut_out(0, fwBuffer, context);
}

}  // namespace Svc
