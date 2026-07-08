#ifndef SPACEPACKETSUBTOPOLOGY_DEFS_HPP
#define SPACEPACKETSUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include "SpacePacketConfig/SpacePacketSubtopologyConfig.hpp"
#include "Svc/Subtopologies/SpacePacket/Ports_ComBufferQueueEnumAc.hpp"
#include "Svc/Subtopologies/SpacePacket/Ports_ComPacketQueueEnumAc.hpp"
#include "Svc/Subtopologies/SpacePacket/SpacePacketConfig/FppConstantsAc.hpp"

namespace SpacePacket {
struct SubtopologyState {
    // Empty - no external state needed for SpacePacket subtopology
};

struct TopologyState {
    SubtopologyState spacePacket;
};
}  // namespace SpacePacket

#endif
