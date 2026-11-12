// ======================================================================
// \title  IdlePacket.hpp
// \brief  CCSDS idle packet helpers (SPP and EPP) shared by CCSDS framers
// ======================================================================
#ifndef SVC_CCSDS_UTILS_IDLE_PACKET_HPP
#define SVC_CCSDS_UTILS_IDLE_PACKET_HPP

#include <limits>
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {

//! \brief CCSDS idle packets used to fill unused frame data field space:
//! SPP (idle APID) and EPP (Encapsulation Idle Packet, CCSDS 133.1-B-3)
namespace IdlePacket {

//! Fill pattern for idle packet data
constexpr U8 DATA_PATTERN = 0x44;

//! Minimum idle packet size: header plus one byte of idle data
constexpr FwSizeType MIN_SIZE = SpacePacketHeader::SERIALIZED_SIZE + 1;

//! Maximum idle packet size: header plus the largest data length the SPP length field can express
constexpr FwSizeType MAX_SIZE =
    SpacePacketHeader::SERIALIZED_SIZE + 1 + static_cast<FwSizeType>(std::numeric_limits<U16>::max());

//! \brief Serialize an idle packet of exactly `size` bytes (header and idle data) into `serializer`
//!
//! \param serializer: destination serializer
//! \param size: total idle packet size in bytes; must be at least MIN_SIZE
//! \return serialization status
Fw::SerializeStatus serialize(Fw::SerialBufferBase& serializer, FwSizeType size);

//! Minimum EPP idle packet size: the one-octet packet (CCSDS 133.1-B-3 4.1.2.8.2)
constexpr FwSizeType EPP_MIN_SIZE = 1;

//! Maximum EPP idle packet size: the largest header-inclusive total the two-octet
//! Packet Length field of the four-octet header can express
constexpr FwSizeType EPP_MAX_SIZE = std::numeric_limits<U16>::max();

//! \brief Serialize an Encapsulation Idle Packet of exactly `size` bytes into `serializer`
//!
//! Uses the smallest header that can carry the size: the one-octet packet for `size` 1, a
//! two-octet header up to 255, and a four-octet header beyond that; Packet Length fields hold
//! the header-inclusive total (CCSDS 133.1-B-3 4.1.2.8.2)
//!
//! \param serializer: destination serializer
//! \param size: total idle packet size in bytes; must be within [EPP_MIN_SIZE, EPP_MAX_SIZE]
//! \return serialization status
Fw::SerializeStatus serializeEpp(Fw::SerialBufferBase& serializer, FwSizeType size);

}  // namespace IdlePacket
}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc

#endif  // SVC_CCSDS_UTILS_IDLE_PACKET_HPP
