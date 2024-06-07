// ======================================================================
// \title  Router.cpp
// \author thomas-bc
// \brief  cpp file for Router component implementation class
// ======================================================================

#include "Fw/Logger/Logger.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Svc/Router/Router.hpp"
#include "FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Router ::Router(const char* const compName) : RouterComponentBase(compName) {}

Router ::~Router() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Router ::bufferIn_handler(NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // Read the packet type from the packet buffer
    FwPacketDescriptorType packetType = Fw::ComPacket::FW_PACKET_UNKNOWN;
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    {
        Fw::SerializeBufferBase& serial = fwBuffer.getSerializeRepr();
        status = serial.setBuffLen(fwBuffer.getSize());
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
        status = serial.deserialize(packetType);
    }

    // Whether to deallocate the packet buffer
    bool deallocate = true;

    // Process the packet
    if (status == Fw::FW_SERIALIZE_OK) {
        U8 *const packetData = fwBuffer.getData();
        const U32 packetSize = fwBuffer.getSize();
        switch (packetType) {
            // Handle a command packet
            case Fw::ComPacket::FW_PACKET_COMMAND: {
                // Allocate a com buffer on the stack
                Fw::ComBuffer com;
                // Copy the contents of the packet buffer into the com buffer
                status = com.setBuff(packetData, packetSize);
                if (status == Fw::FW_SERIALIZE_OK) {
                    // Review Note: Deframer did not check if the output port was connected, should it?
                    // Send the com buffer
                    commandOut_out(0, com, 0);
                }
                else {
                    Fw::Logger::logMsg(
                        "[ERROR] Serializing com buffer failed with status %d\n",
                        status
                    );
                }
                break;
            }
            // Handle a file packet
            case Fw::ComPacket::FW_PACKET_FILE: {
                // If the file uplink output port is connected,
                // send the file packet. Otherwise take no action.
                if (isConnected_fileOut_OutputPort(0)) {
                    // Shift the packet buffer to skip the packet type
                    // The FileUplink component does not expect the packet
                    // type to be there.
                    fwBuffer.setData(packetData + sizeof(packetType));
                    fwBuffer.setSize(packetSize - sizeof(packetType));
                    // Send the packet buffer
                    fileOut_out(0, fwBuffer);
                    // Transfer ownership of the buffer to the receiver
                    deallocate = false;
                }
                break;
            }
            // Take no action for other packet types
            default:
                break;
        }
    }
    else {
        Fw::Logger::logMsg(
            "[ERROR] Deserializing packet type failed with status %d\n",
            status
        );
    }

    if (deallocate) {
        // Deallocate the packet buffer
        bufferDeallocate_out(0, fwBuffer);
    }

}

void Router ::cmdResponseIn_handler(
    NATIVE_INT_TYPE portNum,
    FwOpcodeType opcode,
    U32 cmdSeq,
    const Fw::CmdResponse& response
) {
    // Nothing to do
}
}  // namespace Svc
