// ======================================================================
// \title  IdlePacket.hpp
// \brief  Space Packet Protocol idle packet helpers shared by CCSDS framers
// ======================================================================
#ifndef SVC_CCSDS_UTILS_IDLE_PACKET_HPP
#define SVC_CCSDS_UTILS_IDLE_PACKET_HPP

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {

//! \brief SPP idle packet (idle APID) used to fill unused frame data field space
namespace IdlePacket {

//! Fill pattern for idle packet data
constexpr U8 DATA_PATTERN = 0x44;

//! Minimum idle packet size: header plus one byte of idle data
constexpr FwSizeType MIN_SIZE = SpacePacketHeader::SERIALIZED_SIZE + 1;

//! \brief Serialize an idle packet of exactly `size` bytes (header and idle data) into `serializer`
//!
//! \param serializer: destination serializer
//! \param size: total idle packet size in bytes; must be at least MIN_SIZE
//! \return serialization status
Fw::SerializeStatus serialize(Fw::SerialBufferBase& serializer, FwSizeType size);

}  // namespace IdlePacket
}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc

#endif  // SVC_CCSDS_UTILS_IDLE_PACKET_HPP
