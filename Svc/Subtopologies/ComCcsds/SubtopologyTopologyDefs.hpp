#ifndef COMCCSDSSUBTOPOLOGY_DEFS_HPP
#define COMCCSDSSUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include <Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp>
#include "ComCcsdsConfig/ComCcsdsSubtopologyConfig.hpp"
#include "Svc/Subtopologies/ComCcsds/ComCcsdsConfig/FppConstantsAc.hpp"

namespace ComCcsds {
struct SubtopologyState {
    // Empty - no external state needed for ComCcsds subtopology
};

struct TopologyState {
    SubtopologyState comCcsds;
};
}  // namespace ComCcsds

#endif
