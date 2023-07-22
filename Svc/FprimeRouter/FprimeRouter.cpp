// ======================================================================
// \title  FprimeRouter.cpp
// \author mstarch
// \brief  cpp file for FprimeRouter component implementation class
// ======================================================================

#include <FpConfig.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/FprimeRouter/FprimeRouter.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FprimeRouter ::FprimeRouter(const char* const compName) : FprimeRouterComponentBase(compName) {}

FprimeRouter ::~FprimeRouter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FprimeRouter ::cmdResponseIn_handler(const NATIVE_INT_TYPE portNum,
                                          FwOpcodeType opCode,
                                          U32 cmdSeq,
                                          const Fw::CmdResponse& response) {
    // Specifically does nothing
}

void FprimeRouter ::deframedIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& packetBuffer, Fw::Buffer& context) {
    // The FprimeRouting scheme specifically does not use context nor will the context be deallocated
    // As such, the context passed-in must be invalid
    FW_ASSERT(!context.isValid());

    // Read the packet type from the packet buffer
    FwPacketDescriptorType packetType = Fw::ComPacket::FW_PACKET_UNKNOWN;
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    {
        Fw::SerializeBufferBase& serial = packetBuffer.getSerializeRepr();
        status = serial.setBuffLen(packetBuffer.getSize());
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        status = serial.deserialize(packetType);
    }

    // Whether to deallocate the packet buffer
    bool deallocate = true;

    // Process the packet
    if (status == Fw::FW_SERIALIZE_OK) {
        U8* const packetData = packetBuffer.getData();
        const U32 packetSize = packetBuffer.getSize();
        switch (packetType) {
            // Handle a command packet
            case Fw::ComPacket::FW_PACKET_COMMAND: {
                // Allocate a com buffer on the stack
                Fw::ComBuffer com;
                // Copy the contents of the packet buffer into the com buffer
                status = com.setBuff(packetData, packetSize);
                if (status == Fw::FW_SERIALIZE_OK) {
                    // Send the com buffer
                    comOut_out(0, com, 0);
                } else {
                    Fw::Logger::logMsg("[ERROR] Serializing com buffer failed with status %d\n", status);
                }
                break;
            }
            // Handle a file packet
            case Fw::ComPacket::FW_PACKET_FILE: {
                // If the file uplink output port is connected,
                // send the file packet. Otherwise take no action.
                if (isConnected_fileBufferOut_OutputPort(0)) {
                    // Shift the packet buffer to skip the packet type
                    // The FileUplink component does not expect the packet
                    // type to be there.
                    packetBuffer.setData(packetData + sizeof(packetType));
                    packetBuffer.setSize(packetSize - sizeof(packetType));
                    // Send the packet buffer
                    fileBufferOut_out(0, packetBuffer);
                    // Transfer ownership of the buffer to the receiver
                    deallocate = false;
                }
                break;
            }
            // Take no action for other packet types
            default:
                break;
        }
    } else {
        Fw::Logger::logMsg("[ERROR] Deserializing packet type failed with status %d\n", status);
    }

    if (deallocate) {
        // Deallocate the packet buffer
        bufferDeallocate_out(0, packetBuffer);
    }
}

}  // end namespace Svc
