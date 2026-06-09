#ifndef COMFPRIMESUBTOPOLOGY_DEFS_HPP
#define COMFPRIMESUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include <Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp>
#include "ComFprimeConfig/ComFprimeSubtopologyConfig.hpp"
#include "Svc/Subtopologies/ComFprime/ComFprimeConfig/FppConstantsAc.hpp"
#include "Svc/Subtopologies/ComFprime/Ports_ComBufferQueueEnumAc.hpp"
#include "Svc/Subtopologies/ComFprime/Ports_ComPacketQueueEnumAc.hpp"

namespace ComFprime {
struct SubtopologyState {
    // Empty - no external state needed for ComFprime subtopology
};

struct TopologyState {
    SubtopologyState comFprime;
};
}  // namespace ComFprime

#endif
