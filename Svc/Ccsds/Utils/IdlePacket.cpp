// ======================================================================
// \title  IdlePacket.cpp
// \brief  Space Packet Protocol idle packet helpers shared by CCSDS framers
// ======================================================================

#include "Svc/Ccsds/Utils/IdlePacket.hpp"
#include <limits>
#include "Fw/Types/Assert.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "config/ApidEnumAc.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {
namespace IdlePacket {

Fw::SerializeStatus serialize(Fw::SerialBufferBase& serializer, FwSizeType size) {
    FW_ASSERT(size >= MIN_SIZE, static_cast<FwAssertArgType>(size));
    // Length token is defined as the number of bytes of payload data minus 1
    const FwSizeType lengthToken = size - SpacePacketHeader::SERIALIZED_SIZE - 1;
    FW_ASSERT(lengthToken <= std::numeric_limits<U16>::max(), static_cast<FwAssertArgType>(lengthToken));

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

}  // namespace IdlePacket
}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc
