// ======================================================================
// \title  IdlePacket.cpp
// \brief  CCSDS idle packet helpers (SPP and EPP) shared by CCSDS framers
// ======================================================================

#include "Svc/Ccsds/Utils/IdlePacket.hpp"
#include "Fw/Types/Assert.hpp"
#include "Svc/Ccsds/Types/EppLengthOfLengthEnumAc.hpp"
#include "Svc/Ccsds/Types/EppProtocolIdEnumAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "config/ApidEnumAc.hpp"
#include "config/PvnEnumAc.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {
namespace IdlePacket {

Fw::SerializeStatus serialize(Fw::SerialBufferBase& serializer, FwSizeType size) {
    FW_ASSERT(size >= MIN_SIZE, static_cast<FwAssertArgType>(size));
    FW_ASSERT(size <= MAX_SIZE, static_cast<FwAssertArgType>(size));
    // Length token is defined as the number of bytes of payload data minus 1
    const FwSizeType lengthToken = size - SpacePacketHeader::SERIALIZED_SIZE - 1;

    SpacePacketHeader header;
    header.set_packetIdentification(static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET));
    // Sequence Flags = 0b11 (unsegmented) & unused sequence count
    header.set_packetSequenceControl(static_cast<U16>(0x3 << SpacePacketSubfields::SeqFlagsOffset));
    header.set_packetDataLength(static_cast<U16>(lengthToken));

    Fw::SerializeStatus status = serializer.serializeFrom(header);
    for (FwSizeType i = SpacePacketHeader::SERIALIZED_SIZE; (status == Fw::FW_SERIALIZE_OK) && (i < size); i++) {
        status = serializer.serializeFrom(DATA_PATTERN);
    }
    return status;
}

Fw::SerializeStatus serializeEpp(Fw::SerialBufferBase& serializer, FwSizeType size) {
    FW_ASSERT(size >= EPP_MIN_SIZE, static_cast<FwAssertArgType>(size));
    FW_ASSERT(size <= EPP_MAX_SIZE, static_cast<FwAssertArgType>(size));

    // First octet: Packet Version Number '111' | Protocol ID 'idle' | Length of Length (4.1.2.1)
    constexpr U8 pvnAndIdleBits = static_cast<U8>(
        (static_cast<U8>(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL) << EPPSubfields::packetVersionOffset) |
        (static_cast<U8>(EppProtocolId::Idle) << EPPSubfields::protocolIdOffset));

    // Use the smallest header that can carry the size; each Packet Length field holds the
    // header-inclusive total packet length (4.1.2.8.2). AosDeframer::sizeEppPacket does not read it:
    // an Idle Protocol ID marks the last packet in the frame
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    FwSizeType headerLength = 0;
    if (size == 1) {
        // One-octet idle packet: the header is the whole packet and there is no Packet Length field
        status = serializer.serializeFrom(static_cast<U8>(pvnAndIdleBits | EppLengthOfLength::Zero));
        headerLength = 1;
    } else if (size <= std::numeric_limits<U8>::max()) {
        // Two-octet header: a one-octet Packet Length field
        status = serializer.serializeFrom(static_cast<U8>(pvnAndIdleBits | EppLengthOfLength::One));
        if (status == Fw::FW_SERIALIZE_OK) {
            status = serializer.serializeFrom(static_cast<U8>(size));
        }
        headerLength = 2;
    } else {
        // Four-octet header: a zero User Defined Field / Protocol ID Extension octet and a two-octet
        // Packet Length field. EPP_MAX_SIZE bounds the size, so the eight-octet header (Length of
        // Length 'Four') is never needed
        status = serializer.serializeFrom(static_cast<U8>(pvnAndIdleBits | EppLengthOfLength::Two));
        if (status == Fw::FW_SERIALIZE_OK) {
            status = serializer.serializeFrom(static_cast<U8>(0));
        }
        if (status == Fw::FW_SERIALIZE_OK) {
            status = serializer.serializeFrom(static_cast<U16>(size));
        }
        headerLength = 4;
    }

    // Fill the rest with idle pattern
    for (FwSizeType i = headerLength; (status == Fw::FW_SERIALIZE_OK) && (i < size); i++) {
        status = serializer.serializeFrom(DATA_PATTERN);
    }
    return status;
}

}  // namespace IdlePacket
}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc
