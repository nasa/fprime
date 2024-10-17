/*
 * ComPacket.hpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#ifndef COMPACKET_HPP_
#define COMPACKET_HPP_

#include <Fw/Types/Serializable.hpp>

// Packet format:
// |32-bit packet type|packet type-specific data|

namespace Fw {

    class ComPacket: public Serializable {
        public:

            typedef enum {
                FW_PACKET_COMMAND, // !< Command packet type - incoming
                FW_PACKET_TELEM, // !< Telemetry packet type - outgoing
                FW_PACKET_LOG, // !< Log type - outgoing
                FW_PACKET_FILE, // !< File type - incoming and outgoing
                FW_PACKET_PACKETIZED_TLM, // !< Packetized telemetry packet type
                FW_PACKET_DP, //!< Data product packet
                FW_PACKET_IDLE, // !< Idle packet
                FW_PACKET_UNKNOWN = 0xFF // !< Unknown packet
            } ComPacketType;

            ComPacket();
            virtual ~ComPacket();

        protected:
            ComPacketType m_type;
            SerializeStatus serializeBase(SerializeBufferBase& buffer) const ; // called by derived classes to serialize common fields
            SerializeStatus deserializeBase(SerializeBufferBase& buffer); // called by derived classes to deserialize common fields
    };

} /* namespace Fw */

#endif /* COMPACKET_HPP_ */
