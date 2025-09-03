#ifndef COMCCSDSSUBTOPOLOGY_DEFS_HPP
#define COMCCSDSSUBTOPOLOGY_DEFS_HPP

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include <Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp>
#include "FramingCcsdsConfig/FramingCcsdsSubtopologyConfig.hpp"
#include "Svc/Subtopologies/FramingCcsds/FramingCcsdsConfig/FppConstantsAc.hpp"

namespace FramingCcsds {
struct SubtopologyState {
    // Empty - no external state needed for FramingCcsds subtopology
};

struct TopologyState {
    SubtopologyState comCcsds;
};
}  // namespace FramingCcsds

#endif
